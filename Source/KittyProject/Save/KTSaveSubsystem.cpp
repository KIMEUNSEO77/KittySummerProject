// Fill out your copyright notice in the Description page of Project Settings.


#include "Save/KTSaveSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Save/KTSaveGame.h"
#include "Character/KittyCharacterPlayer.h"
#include "Inventory/KTInventoryComponent.h"

const FString UKTSaveSubsystem::SaveSlotName = TEXT("AutoSave");

UKTSaveSubsystem* UKTSaveSubsystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;

	return GameInstance ? GameInstance->GetSubsystem<UKTSaveSubsystem>() : nullptr;
}

bool UKTSaveSubsystem::SaveProgress(int32 MissionStepIndex, FName CheckpointId)
{
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return false;
	}

	UKTSaveGame* SaveData = Cast<UKTSaveGame>(UGameplayStatics::CreateSaveGameObject(UKTSaveGame::StaticClass()));

	if (!IsValid(SaveData))
	{
		return false;
	}

	SaveData->SavedLevelName = FName(*UGameplayStatics::GetCurrentLevelName(this, true));
	SaveData->MissionStepIndex = MissionStepIndex;
	SaveData->CheckpointId = CheckpointId;

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		SaveData->PlayerLocation = PlayerPawn->GetActorLocation();
		SaveData->PlayerRotation = PlayerPawn->GetActorRotation();
		
		if (AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(PlayerPawn))
		{
			SaveData->bHasPistol = Player->HasPistol();

			if (UKTInventoryComponent* Inventory = Player->GetInventoryComponent())
			{
				SaveData->InventoryItems.Reset();

				for (const FKTInventoryEntry& Entry : Inventory->GetItems())
				{
					if (!IsValid(Entry.ItemData.Get()) || Entry.Quantity <= 0)
					{
						continue;
					}

					FKTInventorySaveEntry SaveEntry;
					SaveEntry.ItemData = Entry.ItemData.Get();
					SaveEntry.Quantity = Entry.Quantity;

					SaveData->InventoryItems.Add(SaveEntry);
				}
			}
		}
	}

	SaveData->CompletedWorldStateIds = RuntimeCompletedWorldStateIds;
	
	return UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, UserIndex);
}

UKTSaveGame* UKTSaveSubsystem::LoadProgress()
{
	if (!DoesSaveExist())
	{
		return nullptr;
	}

	return Cast<UKTSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
}

bool UKTSaveSubsystem::DoesSaveExist() const
{
	return UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex);
}

bool UKTSaveSubsystem::DeleteSave()
{
	PendingSaveData = nullptr;
	RuntimeCompletedWorldStateIds.Reset();
	
	if (!DoesSaveExist())
	{
		return true;
	}

	return UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);
}

bool UKTSaveSubsystem::ContinueGame()
{
	PendingSaveData = LoadProgress();

	if (!IsValid(PendingSaveData) || PendingSaveData->SavedLevelName.IsNone())
	{
		return false;
	}

	RuntimeCompletedWorldStateIds = PendingSaveData->CompletedWorldStateIds;
	
	UGameplayStatics::OpenLevel(this, PendingSaveData->SavedLevelName);

	return true;
}

void UKTSaveSubsystem::ClearPendingSaveData()
{
	PendingSaveData = nullptr;
}

void UKTSaveSubsystem::MarkWorldStateCompleted(FName StateId)
{
	if (StateId.IsNone())
	{
		return;
	}

	RuntimeCompletedWorldStateIds.AddUnique(StateId);
}

bool UKTSaveSubsystem::IsWorldStateCompleted(FName StateId) const
{
	return !StateId.IsNone() && RuntimeCompletedWorldStateIds.Contains(StateId);
}


