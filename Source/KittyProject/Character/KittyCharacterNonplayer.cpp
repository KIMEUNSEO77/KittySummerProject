// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KittyCharacterNonplayer.h"
#include "AI/KittyAIController.h"

AKittyCharacterNonplayer::AKittyCharacterNonplayer()
{
	GetMesh()->SetHiddenInGame(false);
	
	AIControllerClass = AKittyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
}

void AKittyCharacterNonplayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

float AKittyCharacterNonplayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AKittyCharacterNonplayer::GetAIDetectRange()
{
	return 400.0f;
}

float AKittyCharacterNonplayer::GetAIAttackRange()
{
	return 0.0f;
}

float AKittyCharacterNonplayer::GetAITurnSpeed()
{
	return 500.0f;
}
