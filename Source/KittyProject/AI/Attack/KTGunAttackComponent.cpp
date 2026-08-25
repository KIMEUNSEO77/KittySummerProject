// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Attack/KTGunAttackComponent.h"

#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Character/KittyCharacterNonplayer.h"
#include "Components/SkeletalMeshComponent.h"

UKTGunAttackComponent::UKTGunAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UKTGunAttackComponent::GetAttackRange() const
{
	return AttackRange;
}

bool UKTGunAttackComponent::StartAttack(AActor* Target)
{
	AKittyCharacterNonplayer* Guard = Cast<AKittyCharacterNonplayer>(GetOwner());
	
	// 공격에 필요한 대상과 몽타주가 모두 있는지 검사한다.
	if (!IsValid(Guard) || !IsValid(Target) || !IsValid(AttackMontage))
	{
		return false;
	}

	// 현재 비활성화된 공격 컴포넌트라면 공격하지 않는다.
	if (!IsActive())
	{
		return false;
	}

	// 이미 사격 몽타주를 실행하고 있다면 중복 실행하지 않는다.
	if (IsAttacking())
	{
		return false;
	}

	AttackTarget = Target;

	// 새로운 공격이므로 총알 발사 여부를 초기화한다.
	bBulletSpawned = false;

	const float MontageDuration = Guard->PlayAnimMontage(AttackMontage);

	// AnimBP의 Slot이 맞지 않거나 몽타주가 재생되지 않은 경우
	if (MontageDuration <= 0.0f)
	{
		AttackTarget.Reset();
		return false;
	}

	return true;
}

bool UKTGunAttackComponent::IsAttacking() const
{
	const AKittyCharacterNonplayer* Guard = Cast<AKittyCharacterNonplayer>(GetOwner());
	
	if (!IsValid(Guard)||!IsValid(AttackMontage))
	{
		return false;
	}
	
	const USkeletalMeshComponent* GuardMesh = Guard->GetMesh();
	
	if (!IsValid(GuardMesh))
	{
		return false;
	}
	
	const UAnimInstance* AnimInstance = GuardMesh->GetAnimInstance();
	
	if (!IsValid(AnimInstance))
	{
		return false;
	}
	
	return AnimInstance->Montage_IsPlaying(AttackMontage);
}

void UKTGunAttackComponent::HandleHitNotify()
{
	if (bBulletSpawned)
	{
		return;
	}
	
	if (!IsActive())
	{
		return;
	}
	
	AKittyCharacterNonplayer* Guard = Cast<AKittyCharacterNonplayer>(GetOwner());
	
	AActor* Target = AttackTarget.Get();
	
	if (!IsValid(Target) || !IsValid(Guard))
	{
		return;
	}
	
	if (Guard->GetAttackComponent()!=this)
	{
		return;
	}
	
	if (!IsAttacking())
	{
		return;
	}
	
	const float DistanceSquard = FVector::DistSquared2D(Guard->GetActorLocation(), Target->GetActorLocation());
	
	if (DistanceSquard > FMath::Square(AttackRange))
	{
		return;
	}
	
	AAIController* AIController = Cast<AAIController>(Guard->GetController());
	
	if (!IsValid(AIController))
	{
		return;
	}
	
	if (!AIController->LineOfSightTo(Target))
	{
		return;
	}
	
	bBulletSpawned = true;
	
	Guard->FireGuardProjectile(Target, Damage);
}
