// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "TCCharacter.generated.h"

UENUM()
enum ETCCharacterState
{
    None,
    Landed,
    Idle,
    Run,
    JumpRise,
    JumpFall,
    Hit,
    Die
};

UENUM()
enum ETCPlayerActionState
{
    PA_None,
    PA_Pressed
};

UCLASS()
class TC2_API ATCCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ATCCharacter();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnRep_PlayerState() override; // usage: client set name component

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

    virtual void Jump() override;
    virtual void StopJumping() override;
    virtual void Landed(const FHitResult& Hit) override;
    void MoveForward(const float Value);
    void MoveRight(const float Value);

protected:
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USpringArmComponent* SpringArm;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UCameraComponent* Camera;
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UTextRenderComponent* NameComponent;
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UWidgetComponent* AlertComponent;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UWidgetComponent* HealthBarComponent;
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UNiagaraComponent* NiagaraComponent;

    FTimeline TimelineGravityJumpRise;
    FTimeline TimelineGravityJumpFall;
    UPROPERTY(EditDefaultsOnly, Category="Gravity timeline")
    UCurveFloat* CurveGravityJumpRise;
    UPROPERTY(EditDefaultsOnly, Category="Gravity timeline")
    UCurveFloat* CurveGravityJumpFall;

    UFUNCTION() // must have
    void OnTimelineGravityJumpRise(const float Value);
    UFUNCTION()
    void OnTimelineGravityJumpFall(const float Value);

    UPROPERTY(EditDefaultsOnly, Category="Impulse")
	FVector HitImpulse{800.0f, 0.0f, 400.0f};
	UPROPERTY(EditDefaultsOnly, Category="Impulse")
	float HitGravityScale = 2.5;
    UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    void SetState(const ETCCharacterState& St);
    UFUNCTION()
    void OnRep_State();

    float HealthMax = 100;
    UPROPERTY(ReplicatedUsing = OnRep_Health)
    float Health = HealthMax;
    UFUNCTION()
    void OnRep_Health();

    void DecreaseHealth();
    void UpdateHealthBar(const float Percent);
    UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateHeathBarColor(const FLinearColor& Color);

    UPROPERTY(ReplicatedUsing = OnRep_bShowDust)
    bool bShowDust = false;
    UFUNCTION()
    void OnRep_bShowDust();
    void SetShowDust(const bool Show);

    void WidgetsFaceToCamera() const;

private:
    float DeltaSec = 0;
    float MoveForwardScale = 0;
    float MoveRightScale = 0;

    UPROPERTY(ReplicatedUsing = OnRep_State)
    TEnumAsByte<ETCCharacterState> State = ETCCharacterState::None;
    UPROPERTY(Replicated)
    TEnumAsByte<ETCPlayerActionState> ActionState = ETCPlayerActionState::PA_None;

    void ReactOnState();
    void UpdateSpeed();
};
