// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KTSaveSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UKTSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Save", meta = (WorldContext = "WorldContextObject"))
	static UKTSaveSubsystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool SaveProgress(int32 MissionStepIndex, FName CheckpointId);

	UFUNCTION(BlueprintCallable, Category = "Save")
	UKTSaveGame* LoadProgress();

	UFUNCTION(BlueprintPure, Category = "Save")
	bool DoesSaveExist() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool DeleteSave();
	
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool ContinueGame();

	UFUNCTION(BlueprintPure, Category = "Save")
	UKTSaveGame* GetPendingSaveData() const
	{
		return PendingSaveData;
	}

	UFUNCTION(BlueprintCallable, Category = "Save")
	void ClearPendingSaveData();

private:
	static const FString SaveSlotName;
	static constexpr int32 UserIndex = 0;
	
	UPROPERTY()
	TObjectPtr<UKTSaveGame> PendingSaveData;
	
	// Status Store Section
public:
	UFUNCTION(BlueprintCallable, Category = "Save|World State")
	void MarkWorldStateCompleted(FName StateId);

	UFUNCTION(BlueprintPure, Category = "Save|World State")
	bool IsWorldStateCompleted(FName StateId) const;
	
private:
	UPROPERTY()
	TArray<FName> RuntimeCompletedWorldStateIds;
};
