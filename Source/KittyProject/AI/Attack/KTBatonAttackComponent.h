// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KTGuardAttackComponent.h"
#include "Components/ActorComponent.h"
#include "KTBatonAttackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KITTYPROJECT_API UKTBatonAttackComponent : public UKTGuardAttackComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKTBatonAttackComponent();
	
public:
	virtual float GetAttackRange() const override;
	virtual bool StartAttack(AActor* Target) override;
	virtual bool IsAttacking() const override;
	virtual bool HandleHitNotify() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Baton")
	float AttackRange = 170.0f;
	UPROPERTY(EditDefaultsOnly, Category="Baton")
	float Damage = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category="Baton")
	float TraceRadius = 35.0f;
	UPROPERTY(EditDefaultsOnly, Category="Baton")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	TWeakObjectPtr<AActor> AttackTarget;
};
