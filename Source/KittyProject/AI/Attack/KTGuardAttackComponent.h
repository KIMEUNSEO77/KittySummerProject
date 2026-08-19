// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KTGuardAttackComponent.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup="Combat", meta=(BlueprintSpawnableComponent) )
class KITTYPROJECT_API UKTGuardAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKTGuardAttackComponent();
public:
	virtual float GetAttackRange() const;
	virtual bool StartAttack(AActor* Target);
	virtual bool IsAttacking() const;
	virtual void HandleHitNotify();
};
