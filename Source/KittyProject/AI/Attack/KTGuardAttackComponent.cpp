// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Attack/KTGuardAttackComponent.h"

// Sets default values for this component's properties
UKTGuardAttackComponent::UKTGuardAttackComponent()
{
}

float UKTGuardAttackComponent::GetAttackRange() const
{
	return 0.0f;
}

bool UKTGuardAttackComponent::StartAttack(AActor* Target)
{
	return false;
}

bool UKTGuardAttackComponent::IsAttacking() const
{
	return false;
}

void UKTGuardAttackComponent::HandleHitNotify()
{
}
