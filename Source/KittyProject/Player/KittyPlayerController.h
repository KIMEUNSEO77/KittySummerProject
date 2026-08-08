// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Mission/KTMissionTypes.h"
#include "KittyPlayerController.generated.h"


class UKTMissionTrackerWidget;
class UKTMissionNotificationWidget;
class UKTInventoryWidget;
class UKTObjectiveMarkerWidget;
class UKTMissionSubsystem;
class UKTMissionDataAsset;
/**
 * 
 */
UCLASS()
class KITTYPROJECT_API AKittyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	// Editor에서 WBP_MissionTracker를 연결할 변수입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UKTMissionTrackerWidget> MissionTrackerClass;

	// 실제 게임 중 화면에 표시되는 Widget 인스턴스입니다.
	UPROPERTY()
	TObjectPtr<UKTMissionTrackerWidget> MissionTrackerWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UKTObjectiveMarkerWidget> ObjectiveMarkerClass;

	UPROPERTY()
	TObjectPtr<UKTObjectiveMarkerWidget> ObjectiveMarkerWidget;

	UPROPERTY()
	TObjectPtr<UKTMissionSubsystem> MissionSubsystem;

	// Editor의 BP_KittyPlayerController에서
	// DA_Mission_Tutorial을 연결할 변수입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mission")
	TObjectPtr<UKTMissionDataAsset> InitialMission;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UKTMissionNotificationWidget> MissionNotificationClass;

	UPROPERTY()
	TObjectPtr<UKTMissionNotificationWidget> MissionNotificationWidget;
	
	FTimerHandle InitialMissionStartTimer;

	void StartInitialMission();
	
	UFUNCTION()
	void HandleMissionStepChanged(
		FText MissionTitle,
		FMissionStep CurrentStep
	);

	UFUNCTION()
	void HandleMissionCompleted(FText MissionTitle);
	
	UFUNCTION()
	void HandleMissionNextStepQueued(
		FText MissionTitle,
		FMissionStep NextStep
	);
	
public:
	// 플레이어 입력으로 인벤토리를 열고 닫습니다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventory();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryOpen() const
	{
		return bIsInventoryOpen;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Inventory")
	TSubclassOf<UKTInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UKTInventoryWidget> InventoryWidget;

	bool bIsInventoryOpen = false;
};
