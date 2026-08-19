// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Attack/KTBatonAttackComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UKTBatonAttackComponent::UKTBatonAttackComponent()
{
}

float UKTBatonAttackComponent::GetAttackRange() const
{
	return AttackRange;
}

bool UKTBatonAttackComponent::StartAttack(AActor* Target)
{
	ACharacter* Guard = Cast<ACharacter>(GetOwner());
	
	if (!IsValid(Guard) || !IsValid(Target) || !AttackMontage || IsAttacking())
	{
		return false;
	}
	
	AttackTarget = Target;
	
	const float MontageDuration = Guard->PlayAnimMontage(AttackMontage);
	
	if (MontageDuration <= 0.0f)
	{
		AttackTarget.Reset();
		return false;
	}
	
	return true;
}

bool UKTBatonAttackComponent::IsAttacking() const
{
	const ACharacter* Guard = Cast<ACharacter>(GetOwner());
	
	if (!IsValid(Guard)||!AttackMontage)
	{
		return false;
	}
	
	const UAnimInstance* AnimInstance = Guard->GetMesh()->GetAnimInstance();
	
	return AnimInstance && AnimInstance->Montage_IsPlaying(AttackMontage);
}

void UKTBatonAttackComponent::HandleHitNotify()
{
	ACharacter* Guard = Cast<ACharacter>(GetOwner());
	AActor* IntendedTarget = AttackTarget.Get();

	// 이 공격의 타격 기회는 Notify 한 번뿐이다.
	AttackTarget.Reset();

	if (!IsValid(Guard) || !IsValid(IntendedTarget))
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FVector HitCenter;

	if (Guard->GetMesh()->DoesSocketExist(HitSocketName))
	{
		HitCenter =
			Guard->GetMesh()->GetSocketLocation(HitSocketName);
	}
	else
	{
		HitCenter =
			Guard->GetActorTransform().TransformPosition(HitOffset);
	}

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Guard);

	TArray<FOverlapResult> OverlapResults;

	const bool bOverlapped =
		World->OverlapMultiByObjectType(
			OverlapResults,
			HitCenter,
			FQuat::Identity,
			ObjectParams,
			FCollisionShape::MakeSphere(HitRadius),
			QueryParams
		);

	// 맞았는지와 관계없이 구체를 표시한다.
	DrawDebugSphere(
		World,
		HitCenter,
		HitRadius,
		16,
		bOverlapped ? FColor::Green : FColor::Red,
		false,
		1.0f
	);

	if (!bOverlapped)
	{
		return;
	}

	for (const FOverlapResult& Result : OverlapResults)
	{
		APawn* HitPawn = Cast<APawn>(Result.GetActor());

		if (!HitPawn || !HitPawn->IsPlayerControlled())
		{
			continue;
		}

		if (HitPawn != IntendedTarget)
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(
			HitPawn,
			Damage,
			Guard->GetController(),
			Guard,
			UDamageType::StaticClass()
		);

		break;
	}
}