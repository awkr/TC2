// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCCuboid.h"
#include "GameFramework/Actor.h"
#include "TCWater.generated.h"

UCLASS()
class TC2_API ATCWater : public ATCCuboid
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATCWater();

protected:
	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
