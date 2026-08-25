// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Attack/KTGuardAttackComponent.h"
#include "Animation/AnimMontage.h"
#include "KTGunAttackComponent.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UKTGunAttackComponent : public UKTGuardAttackComponent
{
	GENERATED_BODY()
	
public:
	UKTGunAttackComponent();
	
public:
	virtual float GetAttackRange() const override;
	virtual bool StartAttack(AActor* Target) override;
	virtual bool IsAttacking() const override;
	virtual void HandleHitNotify() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Gun", meta=(ClampMin="0.0"))
	float AttackRange = 500.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Gun", meta=(ClampMin="0.0"))
	float Damage = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Gun")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	TWeakObjectPtr<AActor> AttackTarget;
	
	bool bBulletSpawned = false;
};
