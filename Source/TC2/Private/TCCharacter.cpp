// Fill out your copyright notice in the Description page of Project Settings.

#include "TCCharacter.h"
#include "TC2/Log.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Niagara/Classes/NiagaraSystem.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "TCPlayerState.h"
#include "TCPlayerController.h"
#include "TCObstacle.h"
#include "TCMatineeCameraShake.h"
#include "TCRunGameMode.h"

static int HealthBarMaxWidth = 70;
static int HealthBarHeight = 8;

// Sets default values
ATCCharacter::ATCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    // mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshFinder(
		TEXT("/Game/Capsule/SM_Capsule.SM_Capsule"));
	USkeletalMesh* SkeletalMesh = SkeletalMeshFinder.Object;
	GetMesh()->SetSkeletalMesh(SkeletalMesh);
    GetMesh()->SetRelativeScale3D(FVector(.6));
    GetMesh()->SetRelativeLocation(FVector(0, 0, -68));

    GetCapsuleComponent()->SetCapsuleHalfHeight(68);
	GetCapsuleComponent()->SetCapsuleRadius(31.5);

    // spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName(TEXT("SpringArm")));
	SpringArm->SetupAttachment(RootComponent);

	// camera
	Camera = CreateDefaultSubobject<UCameraComponent>(FName(TEXT("Camera")));
	Camera->SetupAttachment(SpringArm);

    // name
	NameComponent = CreateDefaultSubobject<UTextRenderComponent>(FName(TEXT("Name")));
	NameComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	NameComponent->SetWorldSize(32);
	NameComponent->SetRelativeLocation(FVector(0, 0, 100));
	NameComponent->SetCastShadow(false);
	NameComponent->SetupAttachment(RootComponent);

    // health bar
	const FStringClassReference HealthBarClassRef(TEXT("/Game/Blueprints/Widgets/WBP_HealthBar.WBP_HealthBar_C"));
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("HealthBar")));
	HealthBarComponent->SetWidgetClass(HealthBarClassRef.TryLoadClass<UUserWidget>());
	HealthBarComponent->SetDrawSize(FVector2D(HealthBarMaxWidth, HealthBarHeight));
	HealthBarComponent->SetRelativeLocation(FVector(0, 0, 110));
	HealthBarComponent->SetCastShadow(false);
	HealthBarComponent->SetupAttachment(RootComponent);

	// alert
	const FStringClassReference AlertClassRef(TEXT("/Game/Blueprints/Widgets/WBP_Alert.WBP_Alert_C"));
	AlertComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Alert"));
	AlertComponent->SetWidgetClass(AlertClassRef.TryLoadClass<UUserWidget>());
	AlertComponent->SetDrawSize(FVector2D(80, 80));
	AlertComponent->SetRelativeLocation(FVector(0, 0, 200));
	AlertComponent->SetHiddenInGame(true);
	AlertComponent->SetCastShadow(false);
	AlertComponent->SetupAttachment(RootComponent);

    // dust
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(RootComponent);

    //
    GetCharacterMovement()->JumpZVelocity = 620;
    JumpMaxHoldTime = .3;
}

// Called when the game starts or when spawned
void ATCCharacter::BeginPlay()
{
    Super::BeginPlay();

    FOnTimelineFloat RiseTimelineFunc;
    RiseTimelineFunc.BindUFunction(this, FName(TEXT("OnTimelineGravityJumpRise")));
    TimelineGravityJumpRise.AddInterpFloat(CurveGravityJumpRise, RiseTimelineFunc);

    FOnTimelineFloat FallTimelineFunc;
    FallTimelineFunc.BindUFunction(this, FName(TEXT("OnTimelineGravityJumpFall")));
    TimelineGravityJumpFall.AddInterpFloat(CurveGravityJumpFall, FallTimelineFunc);

    OnActorHit.AddDynamic(this, &ATCCharacter::OnHit);

    WidgetsFaceToCamera();

    // note: set username in Blueprint
}

void ATCCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (State == ETCCharacterState::Die) return;

    if (GetActorLocation().Z <= -300) {
        if (State != ETCCharacterState::Die)
		    SetState(ETCCharacterState::Die);
		return;
	}

    this->DeltaSec = DeltaTime;

    switch (State) {
    case ETCCharacterState::JumpRise:
        TimelineGravityJumpRise.TickTimeline(DeltaSec);

        if (GetVelocity().Z < 0) {
            TimelineGravityJumpRise.Stop();

            SetState(ETCCharacterState::JumpFall); // continue to `JumpFall` case

            TimelineGravityJumpFall.PlayFromStart();
        }
        else { break; }
    case ETCCharacterState::JumpFall:
        TimelineGravityJumpFall.TickTimeline(DeltaSec);

        break;

    default:
        break;
    }

    if (GetVelocity().Size() <= 4) {
		if (bShowDust) {
			SetShowDust(false);
		}
	} else {
		if (!bShowDust) {
			SetShowDust(true);
		}
	}

    UpdateSpeed();
}

void ATCCharacter::OnRep_PlayerState()
{
    LOG_FUNC_WARN()
	Super::OnRep_PlayerState();
}

// Called to bind functionality to input
void ATCCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ATCCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATCCharacter::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATCCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATCCharacter::StopJumping);
}

void ATCCharacter::Jump()
{
    Super::Jump();

    SetState(ETCCharacterState::JumpRise);

    TimelineGravityJumpRise.PlayFromStart();
}

void ATCCharacter::StopJumping()
{
    Super::StopJumping(); // 状态更新在 Tick 中判断，当角色处于至高点时，状态置为 JumpFall
}

void ATCCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	TimelineGravityJumpFall.Stop();

	GetCharacterMovement()->GravityScale = 1;

	SetState(ETCCharacterState::Landed);
}

void ATCCharacter::MoveForward(const float Value)
{
    if (Value == 0)
        ActionState = ETCPlayerActionState::PA_None;

    if (Controller && (Value != 0 || MoveForwardScale != Value))
    {
        if (State != ETCCharacterState::JumpRise && State != ETCCharacterState::JumpFall)
        {
            if (State != ETCCharacterState::Run) // 为只调用一次RPC，此处用状态判断
            {
                SetState(ETCCharacterState::Run);
            }
        }

        MoveForwardScale = UKismetMathLibrary::FInterpTo(MoveForwardScale, Value, DeltaSec, 4);

        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        // add movement in that direction
        AddMovementInput(Direction, MoveForwardScale);
    }
}

void ATCCharacter::MoveRight(const float Value)
{
    if (Value == 0)
        ActionState = ETCPlayerActionState::PA_None;

    if (Controller && (Value != 0 || MoveRightScale != Value))
    {
        if (State != ETCCharacterState::JumpRise && State != ETCCharacterState::JumpFall)
        {
            if (State != ETCCharacterState::Run) // 为只调用一次RPC，此处用状态判断
            {
                SetState(ETCCharacterState::Run);
            }
        }

        MoveRightScale = UKismetMathLibrary::FInterpTo(MoveRightScale, Value, DeltaSec, 4);

        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, MoveRightScale);
    }
}

void ATCCharacter::OnTimelineGravityJumpRise(const float Value)
{
    GetCharacterMovement()->GravityScale = Value;
}

void ATCCharacter::OnTimelineGravityJumpFall(const float Value)
{
    GetCharacterMovement()->GravityScale = Value;
}

void ATCCharacter::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor->IsA(ATCObstacle::StaticClass())) {
        return;
    }

    if (State == ETCCharacterState::Hit) { // already in hit state
		return;
    }

    SetState(ETCCharacterState::Hit);

    DecreaseHealth();

    DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 25, FColor::Yellow, false, 1);

    const float ImpulseX = HitImpulse.X + FMath::Abs(GetVelocity().X);
    float ImpulseNormalX = Hit.ImpactNormal.X;
    // 处理近似垂直的碰撞
    const float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Hit.ImpactNormal, FVector(0, 0, 1))));
    if (Angle <= 10.0f) {
        ImpulseNormalX = GetVelocity().X > 0 ? -1 : 1;
    }

    const FVector Impulse{ImpulseX * ImpulseNormalX, 0, HitImpulse.Z};
    GetCharacterMovement()->AddImpulse(Impulse, true);
    GetCharacterMovement()->GravityScale = HitGravityScale;

    // camera shake
    if (const APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
        PlayerController->PlayerCameraManager->StartCameraShake(UTCMatineeCameraShake::StaticClass(), 1);
    }
}

void ATCCharacter::SetState(const ETCCharacterState& St)
{
    if (HasAuthority()) {
        State = St;

        ReactOnState();
    }
}

void ATCCharacter::OnRep_State()
{
    LOG_FUNC_WARN()

    ReactOnState();
}

void ATCCharacter::ReactOnState()
{
    // do some VFX

    switch (State) {
    case ETCCharacterState::Hit:
    AlertComponent->SetHiddenInGame(false); // show alert
    break;

    case ETCCharacterState::Landed:
    if (!AlertComponent->bHiddenInGame) {
        FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [&AlertComponent = AlertComponent]() {
			AlertComponent->SetHiddenInGame(true); // hide alert
		}, 1.75f, false);
    }

    break;

    case ETCCharacterState::Die:
    if (ATCPlayerController* PlayerController = Cast<ATCPlayerController>(GetController())) {
        PlayerController->OnDie();
    }

    break;

    default:
    break;
    }
}

void ATCCharacter::DecreaseHealth()
{
    if (HasAuthority()) {
        Health -= 5;
        UpdateHealthBar(Health / HealthMax);
    }
}

void ATCCharacter::UpdateHealthBar(const float Percent)
{
    HealthBarComponent->SetDrawSize(FVector2D(HealthBarMaxWidth * FMath::Max<float>(0, Percent), HealthBarHeight));
	if (Percent <= .5) {
		BP_UpdateHeathBarColor(FLinearColor(.7, 0, 0, 1));
	} else if (Percent <= .7) {
		BP_UpdateHeathBarColor(FLinearColor(.7, .7, 0, 1));
	}
}

void ATCCharacter::OnRep_Health()
{
    UpdateHealthBar(Health / HealthMax);
}

void ATCCharacter::OnRep_bShowDust()
{
    // if (bShowDust) {
    //     NiagaraComponent->Activate();
    // } else {
    //     NiagaraComponent->Deactivate();
    // }
}

void ATCCharacter::SetShowDust(const bool Show)
{
    // 为了客户端及时得到视觉反馈，本地先「擅自」改动，再等待服务端校验与更新
    bShowDust = Show;

    if (bShowDust) {
        NiagaraComponent->Activate();
    } else {
        NiagaraComponent->Deactivate();
    }

    if (HasAuthority()) {
        bShowDust = Show;

        // if (bShowDust) {
        //     NiagaraComponent->Activate();
        // } else {
        //     NiagaraComponent->Deactivate();
        // }
    }
}

static float Max_Walk_Speed = 600;

void ATCCharacter::UpdateSpeed()
{
	if (ATCRunGameMode* GameMode = GetWorld()->GetAuthGameMode<ATCRunGameMode>())
	{
		const float CurrentTime = UGameplayStatics::GetTimeSeconds(GetWorld());
		float Duration = CurrentTime - GameMode->StartAt;
		int Index = GameMode->SpeedRules.Num() - 1; // 默认为最后阶段（省去了超出所有阶段时的判断）
		for (int i = 0; i < GameMode->SpeedRules.Num(); ++i)
		{
			if (Duration < GameMode->SpeedRules[i].Duration)
			{
				Index = i;
				break;
			}

			Duration -= GameMode->SpeedRules[i].Duration;
		}

		if (Index == 0) { return; } // 第一阶段保持匀速

		const FSpeedRule Previous = GameMode->SpeedRules[Index - 1];
		const FSpeedRule Current = GameMode->SpeedRules[Index];

		const float Speed = Duration * (Current.Speed - Previous.Speed) / Current.Duration + Previous.Speed;

		GetCharacterMovement()->MaxWalkSpeed = Max_Walk_Speed * Speed;
	}
}

void ATCCharacter::WidgetsFaceToCamera() const
{
	const FVector CameraLoc = Camera->GetComponentToWorld().GetLocation();
	const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(NameComponent->GetComponentToWorld().GetLocation(),
	                                                            CameraLoc);
	NameComponent->SetWorldRotation(Rot);
	HealthBarComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
		HealthBarComponent->GetComponentToWorld().GetLocation(),
		CameraLoc));
	AlertComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(
		AlertComponent->GetComponentToWorld().GetLocation(),
		CameraLoc));
}

void ATCCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATCCharacter, State);
    DOREPLIFETIME(ATCCharacter, ActionState);
    DOREPLIFETIME(ATCCharacter, Health);
    DOREPLIFETIME(ATCCharacter, bShowDust);
}
