// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/KittyPlayerController.h"

void AKittyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Game Mode In Viewport
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
