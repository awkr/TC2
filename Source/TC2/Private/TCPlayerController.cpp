// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPlayerController.h"
#include "TC2/Log.h"

void ATCPlayerController::OnDie()
{
	OnDieDelegate.Broadcast();
}
