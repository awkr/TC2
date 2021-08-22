// Fill out your copyright notice in the Description page of Project Settings.


#include "TCWater.h"
#include "TCCharacter.h"
#include "TC2/Log.h"

// Sets default values
ATCWater::ATCWater() : Super()
{
 	// static mesh

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	StaticMesh->SetStaticMesh(MeshAsset.Object);
	StaticMesh->SetRelativeScale3D(FVector(1.75, 10.5, 1));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> ObjectFinder(
		TEXT("/Game/Materials/M_Water_Inst.M_Water_Inst"));
	if (ObjectFinder.Succeeded())
	{
		StaticMesh->SetMaterial(0, ObjectFinder.Object);
	}

	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ATCWater::OnBeginOverlap);
}

void ATCWater::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const ATCCharacter* Character = Cast<ATCCharacter>(OtherActor))
	{
		LOG_FUNC_WARN()
	}
}
