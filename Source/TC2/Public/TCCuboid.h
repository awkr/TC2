// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TCCuboid.generated.h"

UCLASS()
class TC2_API ATCCuboid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATCCuboid();

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USceneComponent* Scene = nullptr;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh = nullptr;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetScaledBoundingBox() const;
};
