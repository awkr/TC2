// Fill out your copyright notice in the Description page of Project Settings.

#include "TCCuboid.h"

// Sets default values
ATCCuboid::ATCCuboid() : Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// root

	Scene = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Scene;

	// static mesh

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	// SetupAttachment is a preferred way to attach components in constructors
	StaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATCCuboid::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATCCuboid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ATCCuboid::GetScaledBoundingBox() const {
	return StaticMesh->GetStaticMesh()->GetBoundingBox().GetSize() * StaticMesh->GetRelativeScale3D();
}
