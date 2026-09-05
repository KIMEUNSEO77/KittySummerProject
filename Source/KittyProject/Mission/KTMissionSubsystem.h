#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Mission/KTMissionTypes.h"
#include "KTMissionSubsystem.generated.h"

class UKTMissionDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMissionStepChanged,
	FText,
	MissionTitle,
	FMissionStep,
	CurrentStep
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMissionCompleted,
	FText,
	MissionTitle
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMissionNextStepQueued,
	FText,
	MissionTitle,
	FMissionStep,
	NextStep
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMissionEventReceived,
	FGameplayTag,
	EventTag,
	AActor*,
	InstigatorActor
);

UCLASS(BlueprintType)
class KITTYPROJECT_API UKTMissionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Mission", meta = (WorldContext = "WorldContextObject", DisPlayName = "Get KT Mission Subsystem"))
	static UKTMissionSubsystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StartMission(UKTMissionDataAsset* NewMission);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void BroadcastMissionEvent(
		FGameplayTag EventTag,
		AActor* InstigatorActor
	);

	void RegisterMissionTarget(
		FGameplayTag TargetId,
		AActor* TargetActor
	);

	void UnregisterMissionTarget(
		FGameplayTag TargetId,
		AActor* TargetActor
	);

	UFUNCTION(BlueprintPure, Category = "Mission")
	AActor* GetCurrentTargetActor() const;

	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionStepChanged OnMissionStepChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionCompleted OnMissionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionNextStepQueued OnMissionNextStepQueued;
	
	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionEventReceived OnMissionEventReceived;
	
private:
	void BroadcastCurrentStep();

	UPROPERTY()
	TObjectPtr<UKTMissionDataAsset> CurrentMission;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<AActor>> RegisteredTargets;

	int32 CurrentStepIndex = INDEX_NONE;

	EMissionState CurrentState = EMissionState::Inactive;
	
	void StartNextStep();

	FTimerHandle StepTransitionTimer;

	bool bIsTransitioning = false;

	float StepTransitionDelay = 1.5f;
	
	// Save Game Section
public:
	UFUNCTION(BlueprintPure, Category = "Mission")
	int32 GetCurrentStepIndex() const
	{
		return CurrentStepIndex;
	}

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void RestoreMissionProgress(UKTMissionDataAsset* MissionToRestore, int32 SavedStepIndex);
};