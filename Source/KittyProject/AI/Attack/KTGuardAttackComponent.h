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
	
	//블루프린트에서 호출할 수 있도록 설정
	UFUNCTION(BlueprintCallable, Category="Combat||Attack")
	virtual void HandleHitNotify();
};
