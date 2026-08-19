// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/KTAnimNotifyState_BatonHit.h"

#include "AI/Attack/KTBatonAttackComponent.h"
#include "Character/KittyCharacterNonplayer.h"
#include "Components/SkeletalMeshComponent.h"

void UKTAnimNotifyState_BatonHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (UKTBatonAttackComponent* AttackComponent =
	GetAttackComponent(MeshComp))
	{
		AttackComponent->BeginHitWindow();
	}
}

void UKTAnimNotifyState_BatonHit::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (UKTBatonAttackComponent* AttackComponent = GetAttackComponent(MeshComp))
	{
		AttackComponent->UpdateHitWindow();
	}
}

void UKTAnimNotifyState_BatonHit::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (UKTBatonAttackComponent* AttackComponent = GetAttackComponent(MeshComp))
	{
		AttackComponent->EndHitWindow();
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

UKTBatonAttackComponent* UKTAnimNotifyState_BatonHit::GetAttackComponent(USkeletalMeshComponent* MeshComp) const
{
	if (!IsValid(MeshComp))
	{
		return nullptr;
	}

	AKittyCharacterNonplayer* Guard =
		Cast<AKittyCharacterNonplayer>(MeshComp->GetOwner());

	if (!IsValid(Guard))
	{
		return nullptr;
	}

	return Cast<UKTBatonAttackComponent>(Guard->GetAttackComponent());
}
