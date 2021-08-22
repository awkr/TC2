// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TCCuboid.h"
#include "TCBuilding.h"
#include "TCObstacle.h"
#include "TCRunGameMode.generated.h"

USTRUCT()
struct FSpeedRule
{
    GENERATED_BODY()

public:
    FSpeedRule()
    {
    }

    FSpeedRule(const int8 Duration, const float Speed) : Duration(Duration), Speed(Speed)
    {
    }

    int8 Duration; // seconds
    float Speed;
};

/**
 *
 */
UCLASS()
class TC2_API ATCRunGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    ATCRunGameMode();
    virtual void BeginPlay() override;

    // wrapper function of spawning floor, building(s), water
    UFUNCTION(BlueprintCallable)
    void SpawnFloor();

    UPROPERTY()
	TArray<FSpeedRule> SpeedRules;

    float StartAt;

protected:
    UPROPERTY()
    TArray<ATCCuboid*> FloorAndWater;
    UPROPERTY()
	TArray<ATCBuilding*> Buildings;
    UPROPERTY()
	TArray<ATCObstacle*> Obstacles;

    void SpawnWater();
    void SpawnBuilding();
    void SpawnObstacle(const int Num);

private:
    FVector FloorSize;
    FVector WaterSize;
};
