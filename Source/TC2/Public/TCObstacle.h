// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCCuboid.h"
#include "TCObstacle.generated.h"

/**
 *
 */
UCLASS()
class TC2_API ATCObstacle : public ATCCuboid
{
	GENERATED_BODY()

public:
	ATCObstacle();

protected:
	UPROPERTY()
	UMaterialInstanceDynamic* Material;
	UPROPERTY(ReplicatedUsing = OnReq_Color)
	FVector Color;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnReq_Color();
};
