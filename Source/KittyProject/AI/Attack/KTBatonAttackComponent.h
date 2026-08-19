// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KTGuardAttackComponent.h"
#include "Components/ActorComponent.h"
#include "KTBatonAttackComponent.generated.h"

class UAnimMontage;

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
	virtual void HandleHitNotify() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Baton")
	float AttackRange = 170.0f;
	UPROPERTY(EditDefaultsOnly, Category="Baton")
	float Damage = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category="Baton")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Baton|Hit", meta=(ClampMin = "0.0"))
	float HitRadius = 65.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Baton|Hit")
	FVector HitOffset = FVector(110.0f, 0.0f, 60.0f);
	
	UPROPERTY(EditDefaultsOnly, Category="Baton|Hit")
	FName HitSocketName = TEXT("BatonHitSocket");
	
	TWeakObjectPtr<AActor> AttackTarget;
};
