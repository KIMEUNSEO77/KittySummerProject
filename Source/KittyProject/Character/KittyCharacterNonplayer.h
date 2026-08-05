// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/KittyCharacterBase.h"
#include "Interface/KittyCharacterAIInterface.h"
#include "KittyCharacterNonplayer.generated.h"

/**
 * 
 */
 
 class ATargetPoint;
 
UCLASS()
class KITTYPROJECT_API AKittyCharacterNonplayer : public AKittyCharacterBase, public IKittyCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AKittyCharacterNonplayer();
	
	bool IsPatrolEnabled() const
	{
		return bPatrolEnabled;
	}
	
	const TArray<TObjectPtr<ATargetPoint>>& GetPatrolPoints() const
	{
		return PatrolPoints;
	}
	
protected:
	virtual void PostInitializeComponents() override;
	
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;	
	
private:
	
	//순찰 경로 지정을 위한 값
	UPROPERTY(EditAnywhere, Category = AI)
	bool bPatrolEnabled;
	
	UPROPERTY(EditAnywhere, Category = AI)
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;
};
