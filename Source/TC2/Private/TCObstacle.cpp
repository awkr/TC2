// Fill out your copyright notice in the Description page of Project Settings.


#include "TCObstacle.h"
#include "Net/UnrealNetwork.h"
#include "TC2/Log.h"

ATCObstacle::ATCObstacle() : Super()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	StaticMesh->SetStaticMesh(MeshAsset.Object);
	StaticMesh->SetRelativeScale3D(FVector(1, 1, 1));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> Finder(
		TEXT("/Game/Materials/M_Obstacle_Inst.M_Obstacle_Inst"));
	if (Finder.Succeeded())
	{
		Material = UMaterialInstanceDynamic::Create(Finder.Object, StaticMesh);
		StaticMesh->SetMaterial(0, Material);
	}
}

void ATCObstacle::BeginPlay()
{
	LOG_FUNC_WARN()
	Super::BeginPlay();

	if (HasAuthority() && Material)
    {
        Color = FVector(FMath::FRandRange(0, 1), FMath::FRandRange(0, 1),
                        FMath::FRandRange(0, 1));
        Material->SetVectorParameterValue(FName("BaseColor"), Color);
    }
}

void ATCObstacle::OnReq_Color()
{
	LOG_FUNC_WARN()
	Material->SetVectorParameterValue(FName("BaseColor"), Color);
}

void ATCObstacle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ATCObstacle, Color, COND_InitialOnly);
}
