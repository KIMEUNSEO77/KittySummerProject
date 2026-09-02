// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/KTCharacterCivilian.h"
#include "Animation/AnimSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Character/KittyCharacterPlayer.h"
#include "Sound/AmbientSound.h"
#include "Components/AudioComponent.h"

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
	
	if (IsValid(AmbientConversationSound))
	{
		if (UAudioComponent* AudioComponent = AmbientConversationSound->GetAudioComponent())
		{
			AudioComponent->FadeOut(0.2f, 0.0f);
		}
	}

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
	
	GetWorldTimerManager().SetTimer(DetectionTimer, this, &AKTCharacterCivilian::CheckPlayerDetection, DetectionInterval, true);
}

void AKTCharacterCivilian::CheckPlayerDetection()
{
	if (bHasReacted)
	{
		GetWorldTimerManager().ClearTimer(DetectionTimer);
		return;
	}

	AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (!IsValid(Player))
	{
		return;
	}

	const float DistanceSquared = FVector::DistSquared(GetActorLocation(), Player->GetActorLocation());

	if (DistanceSquared > FMath::Square(DetectionRange))
	{
		return;
	}
	
	// 높이 차이는 제외하고 수평 방향만 비교
	FVector DirectionToPlayer = Player->GetActorLocation() - GetActorLocation();

	DirectionToPlayer.Z = 0.0f;
	DirectionToPlayer.Normalize();

	// NPC의 정면 방향
	FVector DetectionForward = GetActorForwardVector();
	DetectionForward.Z = 0.0f;
	DetectionForward.Normalize();

	// 보이는 메시와 Actor 정면이 다를 때 방향 보정
	DetectionForward = DetectionForward.RotateAngleAxis(DetectionYawOffset, FVector::UpVector);

	const float ViewDot = FVector::DotProduct(DetectionForward, DirectionToPlayer);
	const float MinimumViewDot = FMath::Cos(FMath::DegreesToRadians(DetectionHalfAngle));

	if (ViewDot < MinimumViewDot)
	{
		return;
	}

	TriggerStartledReaction();
}
