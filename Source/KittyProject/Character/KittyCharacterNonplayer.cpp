// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KittyCharacterNonplayer.h"
#include "AI/KittyAIController.h"

AKittyCharacterNonplayer::AKittyCharacterNonplayer()
{
	GetMesh()->SetHiddenInGame(true);
	
	AIControllerClass = AKittyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
}

void AKittyCharacterNonplayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
