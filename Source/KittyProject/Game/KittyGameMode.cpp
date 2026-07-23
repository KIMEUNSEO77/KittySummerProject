// Fill out your copyright notice in the Description page of Project Settings.


#include "KittyGameMode.h"
#include "Player/KittyPlayerController.h"

AKittyGameMode::AKittyGameMode()
{
	// DefaultPawnClass = 
	PlayerControllerClass = AKittyPlayerController::StaticClass();
}

