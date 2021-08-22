// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TCPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDieDelegate);

/**
 *
 */
UCLASS()

class TC2_API ATCPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UFUNCTION()
	void OnDie();
	UPROPERTY(BlueprintAssignable)
	FOnDieDelegate OnDieDelegate;
};
