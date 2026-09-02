// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KTCharacterCivilian.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AKTCharacterCivilian::AKTCharacterCivilian()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKTCharacterCivilian::TriggerStartledReaction()
{
	if (bHasReacted || !IsValid(GetMesh()) || !IsValid(StartledAnimation))
	{
		return;
	}

	bHasReacted = true;

	// 놀람 애니메이션 재생
	GetMesh()->PlayAnimation(StartledAnimation, false);

	// 설정된 음성 중 하나를 무작위로 재생
	if (!AlertSounds.IsEmpty())
	{
		const int32 RandomIndex = FMath::RandRange(0, AlertSounds.Num() - 1);
		USoundBase* SelectedSound = AlertSounds[RandomIndex];

		if (IsValid(SelectedSound))
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				SelectedSound,
				GetActorLocation()
			);
		}
	}

	// 기존 생활 애니메이션으로 복귀
	GetWorldTimerManager().SetTimer(
		ReactionAnimationTimer,
		this,
		&AKTCharacterCivilian::ResumeAmbientAnimation,
		StartledAnimation->GetPlayLength(),
		false
	);
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

void AKTCharacterCivilian::ResumeAmbientAnimation()
{
	if (IsValid(GetMesh()) && IsValid(AmbientAnimation))
	{
		GetMesh()->PlayAnimation(AmbientAnimation, bLoopAmbientAnimation);
	}
}
