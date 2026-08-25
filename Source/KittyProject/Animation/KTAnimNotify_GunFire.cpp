// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/KTAnimNotify_GunFire.h"

#include "AI/Attack/KTGunAttackComponent.h"
#include "Character/KittyCharacterNonplayer.h"
#include "Components/SkeletalMeshComponent.h"

void UKTAnimNotify_GunFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (IsValid(MeshComp))
	{
		return;
	}
	
	AKittyCharacterNonplayer* Guard = Cast<AKittyCharacterNonplayer>(MeshComp->GetOwner());
	
	if (IsValid(Guard))
	{
		return;
	}
	
	if (Guard->IsDead())
	{
		return;
	}
	
	UKTGunAttackComponent* GunAttackComponent =Cast<UKTGunAttackComponent>(Guard->GetAttackComponent());
	
	if (!IsValid(GunAttackComponent))
	{
		return;
	}
	
	GunAttackComponent->HandleHitNotify();
}
