// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KTCharacterCivilian.h"

#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AKTCharacterCivilian::AKTCharacterCivilian()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKTCharacterCivilian::BeginPlay()
{
	Super::BeginPlay();
	
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->DisableMovement();
	}

	if (IsValid(GetMesh()) && IsValid(AmbientAnimation))
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		GetMesh()->PlayAnimation(AmbientAnimation, bLoopAmbientAnimation);
	}
}
