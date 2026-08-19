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
#include "Engine/HitResult.h"

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
	
	// 새로운 공격을 시작하므로 판정 상태를 초기화한다.
	bHitWindowActive = false;
	bDamageApplied = false;
	PreviousHitCenter = FVector::ZeroVector;
	
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
	// Notify State가 실행 중이라면 단일 Notify는 무시한다.
	if (bHitWindowActive)
	{
		return;
	}

	BeginHitWindow();
	UpdateHitWindow();
	EndHitWindow();
}

void UKTBatonAttackComponent::BeginHitWindow()
{
	AActor* Target = AttackTarget.Get();
	
	if (!IsValid(Target))
	{
		bHitWindowActive = false;
		return;
	}
	
	PreviousHitCenter = GetHitCenter();
	
	bHitWindowActive = true;
	bDamageApplied = false;
}

void UKTBatonAttackComponent::UpdateHitWindow()
{
	if (!bHitWindowActive)
	{
		return;
	}

	ACharacter* Guard =
		Cast<ACharacter>(GetOwner());

	AActor* IntendedTarget =
		AttackTarget.Get();

	if (!IsValid(Guard)
		|| !IsValid(IntendedTarget))
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	const FVector CurrentHitCenter =
		GetHitCenter();

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Guard);

	TArray<FHitResult> HitResults;

	World->SweepMultiByObjectType(
		HitResults,
		PreviousHitCenter,
		CurrentHitCenter,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(HitRadius),
		QueryParams
	);

	bool bTargetInSweep = false;

	for (const FHitResult& HitResult : HitResults)
	{
		APawn* HitPawn =
			Cast<APawn>(HitResult.GetActor());

		if (!IsValid(HitPawn))
		{
			continue;
		}

		if (!HitPawn->IsPlayerControlled())
		{
			continue;
		}

		if (HitPawn != IntendedTarget)
		{
			continue;
		}

		bTargetInSweep = true;

		// Sweep은 계속 실행하지만 데미지는 한 번만 준다.
		if (!bDamageApplied)
		{
			UGameplayStatics::ApplyDamage(
				HitPawn,
				Damage,
				Guard->GetController(),
				Guard,
				UDamageType::StaticClass()
			);

			bDamageApplied = true;
		}

		break;
	}

	DrawDebugLine(
		World,
		PreviousHitCenter,
		CurrentHitCenter,
		FColor::Yellow,
		false,
		0.1f,
		0,
		2.0f
	);

	DrawDebugSphere(
		World,
		PreviousHitCenter,
		HitRadius,
		12,
		FColor::Orange,
		false,
		1.0f
	);

	DrawDebugSphere(
		World,
		CurrentHitCenter,
		HitRadius,
		12,
		bTargetInSweep
			? FColor::Green
			: FColor::Red,
		false,
		1.0f
	);

	// 맞은 뒤에도 계속 갱신해야 한다.
	PreviousHitCenter = CurrentHitCenter;
}

void UKTBatonAttackComponent::EndHitWindow()
{
	bHitWindowActive = false;
	PreviousHitCenter = FVector::ZeroVector;
}

FVector UKTBatonAttackComponent::GetHitCenter() const
{
	const ACharacter* Guard =
	Cast<ACharacter>(GetOwner());

	if (!IsValid(Guard))
	{
		return FVector::ZeroVector;
	}

	const USkeletalMeshComponent* Mesh =
		Guard->GetMesh();

	if (IsValid(Mesh)
		&& Mesh->DoesSocketExist(HitSocketName))
	{
		return Mesh->GetSocketLocation(HitSocketName);
	}

	// 소켓이 없으면 경비원 앞쪽 고정 위치를 사용한다.
	return Guard->GetActorTransform().TransformPosition(
		HitOffset
	);
}
