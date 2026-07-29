// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/KittyCharacterBase.h"
#include "Interface/KittyCharacterAIInterface.h"
#include "KittyCharacterNonplayer.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API AKittyCharacterNonplayer : public AKittyCharacterBase, public IKittyCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AKittyCharacterNonplayer();
	
protected:
	virtual void PostInitializeComponents() override;
	
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;	
};
