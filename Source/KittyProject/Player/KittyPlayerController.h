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
class UKTExamineWidget;
class UKTItemDataAsset;
class UKTRadioSubtitleWidget;

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
	
	UPROPERTY(
	EditDefaultsOnly,
	BlueprintReadOnly,
	Category = "Sound|Mission"
)
	TObjectPtr<class USoundBase> NewMissionSound;
public:
	// 플레이어 입력으로 인벤토리를 열고 닫습니다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventory();

	UFUNCTION(BlueprintCallable, Category = "Examine")
	void OpenExamine(UKTItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable, Category = "Examine")
	void CloseExamine();
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryOpen() const
	{
		return bIsInventoryOpen;
	}
	

	UFUNCTION(BlueprintPure, Category = "Examine")
	bool IsExamineOpen() const
	{
		return bIsExamineOpen;
	}

	UFUNCTION(BlueprintPure, Category = "Cinematic")
	bool IsGameplayUIHidden() const
	{
		return bIsGameplayUIHidden;
	}

	UFUNCTION(BlueprintPure, Category = "Radio")
	bool IsRadioInputLocked() const
	{
		return bRadioInputLocked;
	}
	
	// 현재 상호작용 대상이 목적지 마커의 대상일 때,
	// 마커를 F 상호작용 UI로 전환합니다.
	void ShowObjectiveInteractionPrompt(
		AActor* InteractableActor,
		const FText& PromptText
	);

	// 일반 목적지 마커 상태로 되돌립니다.
	void HideObjectiveInteractionPrompt();
	
	// 시네마틱 실행 중 게임 UI를 숨기거나 다시 표시
	void SetGameplayUIVisible(bool bVisible);

	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Inventory")
	TSubclassOf<UKTInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UKTInventoryWidget> InventoryWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Examine")
	TSubclassOf<UKTExamineWidget> ExamineWidgetClass;

	UPROPERTY()
	TObjectPtr<UKTExamineWidget> ExamineWidget;

	bool bIsExamineOpen = false;

	UFUNCTION()
	void HandleExamineCloseRequested();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound|Inventory")
    TObjectPtr<class USoundBase> InventoryOpenSound;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound|Inventory")
    TObjectPtr<class USoundBase> InventoryCloseSound;

	bool bIsInventoryOpen = false;
	
	// 시네마틱으로 게임 UI를 숨긴 상태인지 확인
	bool bIsGameplayUIHidden = false;
	
	// Radio Section
public:
	UFUNCTION(BlueprintCallable, Category = "Radio")
	void PlayRadioMessage(USoundBase* VoiceSound, const FText& SpeakerName, const FText& SubtitleText, bool bLockInput = true);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Radio")
	TSubclassOf<UKTRadioSubtitleWidget> RadioSubtitleClass;

	UPROPERTY()
	TObjectPtr<UKTRadioSubtitleWidget> RadioSubtitleWidget;
	
	UFUNCTION()
	void HandleRadioMessageFinished();

	UPROPERTY()
	TObjectPtr<class UAudioComponent> ActiveRadioAudio;

	bool bIsRadioPlaying = false;
	bool bRadioInputLocked = false;
};
