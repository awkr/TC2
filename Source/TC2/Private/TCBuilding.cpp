// Fill out your copyright notice in the Description page of Project Settings.


#include "TCBuilding.h"
#include "Net/UnrealNetwork.h"
#include "TC2/Log.h"

ATCBuilding::ATCBuilding() : Super()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	StaticMesh->SetStaticMesh(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> Finder(
		TEXT("/Game/Materials/Building/M_Building_Inst.M_Building_Inst"));
	if (Finder.Succeeded()) {
		Material = UMaterialInstanceDynamic::Create(Finder.Object, StaticMesh);
		StaticMesh->SetMaterial(0, Material);
	}
}

void ATCBuilding::BeginPlay()
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

void ATCBuilding::OnReq_Color()
{
	LOG_FUNC_WARN()
	Material->SetVectorParameterValue(FName("BaseColor"), Color);
}

void ATCBuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ATCBuilding, Color, COND_InitialOnly);
}
