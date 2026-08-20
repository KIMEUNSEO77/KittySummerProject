// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/KTPistolPickup.h"
#include "Character/KittyCharacterPlayer.h"
#include "Engine/Engine.h"

#include "Mission/KTMissionSubsystem.h"
#include "GameplayTagContainer.h"
#include "Components/SceneComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AKTPistolPickup::AKTPistolPickup()
{
	InteractionText = FText::FromString(TEXT("총 획득하기"));
	
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(PickupMesh);
	
	PickupStandPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PickupStandPoint"));
	PickupStandPoint->SetupAttachment(InteractionCollision);
}

void AKTPistolPickup::Interact_Implementation(AActor* Interactor)
{
	AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(Interactor);

	if (!IsValid(Player))
	{
		return;
	}
	
	Player->StartPistolPickup(this);

	/* 임시 주석 처리
	const bool bAcquired = Player->AcquirePistol(this);

	if (!bAcquired)
	{
		return;
	}
	
	if (UKTMissionSubsystem* MissionSubsystem = UKTMissionSubsystem::Get(this))
	{
		const FGameplayTag PistolAcquiredTag =
			FGameplayTag::RequestGameplayTag(
				FName("Mission.Event.Item.Pistol.Acquired")
			);

		MissionSubsystem->BroadcastMissionEvent(
			PistolAcquiredTag,
			Interactor
		);
	}
	*/
}

FVector AKTPistolPickup::GetMuzzleLocation() const
{
	if (!IsValid(MuzzlePoint))
	{
		return GetActorLocation();
	}

	return MuzzlePoint->GetComponentLocation();
}

FVector AKTPistolPickup::GetMuzzleForwardVector() const
{
	if (!IsValid(MuzzlePoint))
	{
		return GetActorForwardVector();
	}

	return MuzzlePoint->GetForwardVector();
}

void AKTPistolPickup::PlayMuzzleFlash()
{
	if (!IsValid(MuzzleFlashEffect) || !IsValid(MuzzlePoint))
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashEffect, MuzzlePoint, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
}

void AKTPistolPickup::PlayFireSound()
{
	if (!IsValid(FireSound))
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetMuzzleLocation(), FireSoundVolume, FireSoundPitch);
}

FTransform AKTPistolPickup::GetPickupStandTransform() const
{
	if (!IsValid(PickupStandPoint))
	{
		return GetActorTransform();
	}

	return PickupStandPoint->GetComponentTransform();
}
