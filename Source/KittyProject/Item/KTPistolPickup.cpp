// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/KTPistolPickup.h"
#include "Character/KittyCharacterPlayer.h"
#include "Engine/Engine.h"

#include "Mission/KTMissionSubsystem.h"
#include "GameplayTagContainer.h"

AKTPistolPickup::AKTPistolPickup()
{
	InteractionText = FText::FromString(TEXT("[F] 총 획득하기"));
}

void AKTPistolPickup::Interact_Implementation(AActor* Interactor)
{
	AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(Interactor);

	if (!IsValid(Player))
	{
		return;
	}

	const bool bAcquired = Player->AcquirePistol(this);

	if (!bAcquired)
	{
		return;
	}
	
	if (UKTMissionSubsystem* MissionSubsystem =
	UKTMissionSubsystem::Get(this))
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

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(4, 2.0f, FColor::Green, TEXT("권총 획득 성공"));
	}
}