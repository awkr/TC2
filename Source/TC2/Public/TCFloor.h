// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCCuboid.h"
#include "Components/BoxComponent.h"
#include "TCFloor.generated.h"

UCLASS()
class TC2_API ATCFloor : public ATCCuboid
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATCFloor();

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UBoxComponent* Box = nullptr;

	UFUNCTION(BlueprintCallable)
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
