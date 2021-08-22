// Fill out your copyright notice in the Description page of Project Settings.


#include "TCFloor.h"
#include "TCCharacter.h"
#include "TCRunGameMode.h"

// Sets default values
ATCFloor::ATCFloor() : Super()
{
 	// static mesh

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/MapTemplates/SM_Template_Map_Floor"));
	StaticMesh->SetStaticMesh(MeshAsset.Object);

	// box

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetRelativeLocation(FVector(0, 0, 190));
	Box->SetRelativeScale3D(FVector(1, 12, 6.25));
	Box->OnComponentBeginOverlap.AddDynamic(this, &ATCFloor::OnBoxBeginOverlap);
	Box->SetHiddenInGame(false);
	Box->SetupAttachment(RootComponent);
}

void ATCFloor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}
	if (ATCCharacter* Character = Cast<ATCCharacter>(OtherActor))
	{
		if (ATCRunGameMode* GameMode = GetWorld()->GetAuthGameMode<ATCRunGameMode>())
		{
			GameMode->SpawnFloor();

			Box->OnComponentBeginOverlap.Clear();
			Box->DestroyComponent();
		}
	}
}
