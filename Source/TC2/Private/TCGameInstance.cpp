// Fill out your copyright notice in the Description page of Project Settings.

#include "TCGameInstance.h"
#include "TC2/Log.h"

UTCGameInstance::UTCGameInstance() : Super()
{
    LOG_FUNC_WARN()

    Username = GeneratePlayerName();
}

FText UTCGameInstance::GeneratePlayerName()
{
    return FText::FromString(
            FString::Printf(TEXT("%c%d"), FMath::RandRange(65, 90), FMath::RandRange(1, 100)));
}
