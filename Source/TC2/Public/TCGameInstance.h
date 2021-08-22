// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TCGameInstance.generated.h"

/**
 *
 */
UCLASS()

class TC2_API UTCGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UTCGameInstance();

    UPROPERTY()
    FText Username; // todo refactor to 'PlayerName'

protected:
    UFUNCTION(BlueprintCallable)
    FText GeneratePlayerName();
};
