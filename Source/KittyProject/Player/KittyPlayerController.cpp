// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/KittyPlayerController.h"

#include "UI/KTMissionTrackerWidget.h"
#include "UI/KTMissionNotificationWidget.h"
#include "Mission/KTMissionSubsystem.h"
#include "UI/KTObjectiveMarkerWidget.h"
#include "Mission/KTMissionDataAsset.h"
#include "TimerManager.h"
#include "Character/KittyCharacterPlayer.h"
#include "Inventory/KTInventoryComponent.h"
#include "UI/KTInventoryWidget.h"


void AKittyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Game Mode In Viewport
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
	
	// UI 추가 코드
	if (!IsLocalController())
	{
		return;
	}

	if (!MissionTrackerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MissionTrackerClass가 설정되지 않았습니다."));
		return;
	}

	MissionTrackerWidget = CreateWidget<UKTMissionTrackerWidget>(
		this,
		MissionTrackerClass
	);

	if (!MissionTrackerWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MissionTrackerWidget 생성에 실패했습니다."));
		return;
	}

	MissionTrackerWidget->AddToViewport();
	if (ObjectiveMarkerClass)
	{
		ObjectiveMarkerWidget =
			CreateWidget<UKTObjectiveMarkerWidget>(
				this,
				ObjectiveMarkerClass
			);

		if (ObjectiveMarkerWidget)
		{
			ObjectiveMarkerWidget->AddToViewport(1);
		}
	}
	
	if (MissionNotificationClass)
	{
		MissionNotificationWidget =
			CreateWidget<UKTMissionNotificationWidget>(
				this,
				MissionNotificationClass
			);

		if (MissionNotificationWidget)
		{
			MissionNotificationWidget->AddToViewport(10);
		}
	}
	
	MissionSubsystem = GetWorld()->GetSubsystem<UKTMissionSubsystem>();

	if (MissionSubsystem)
	{
		MissionSubsystem->OnMissionStepChanged.AddDynamic(
			this,
			&AKittyPlayerController::HandleMissionStepChanged
		);

		MissionSubsystem->OnMissionCompleted.AddDynamic(
			this,
			&AKittyPlayerController::HandleMissionCompleted
		);
		
		MissionSubsystem->OnMissionNextStepQueued.AddDynamic(
			this,
			&AKittyPlayerController::HandleMissionNextStepQueued
		);
		
		if (InitialMission)
		{
			GetWorldTimerManager().SetTimer(
				InitialMissionStartTimer,
				this,
				&AKittyPlayerController::StartInitialMission,
				0.2f,
				false
			);
		}
	}

}

void AKittyPlayerController::HandleMissionNextStepQueued(
	FText MissionTitle,
	FMissionStep NextStep)
{
	if (MissionNotificationWidget)
	{
		MissionNotificationWidget->ShowNewMission(
			MissionTitle,
			NextStep.Description
		);
	}

	// 기존 목표가 완료되었음을 HUD에 잠깐 표시합니다.
	if (MissionTrackerWidget)
	{
		MissionTrackerWidget->SetMissionText(
			MissionTitle,
			FText::FromString(TEXT("목표 완료"))
		);
	}

	// 이전 목표였던 권총의 마커를 즉시 숨깁니다.
	if (ObjectiveMarkerWidget)
	{
		ObjectiveMarkerWidget->SetTargetActor(nullptr);
	}
}



void AKittyPlayerController::HandleMissionStepChanged(
	FText MissionTitle,
	FMissionStep CurrentStep)
{
	if (MissionTrackerWidget)
	{
		MissionTrackerWidget->SetMissionText(
			MissionTitle,
			CurrentStep.Description
		);
	}

	if (ObjectiveMarkerWidget && MissionSubsystem)
	{
		ObjectiveMarkerWidget->SetTargetActor(
			MissionSubsystem->GetCurrentTargetActor()
		);
	}
}

void AKittyPlayerController::HandleMissionCompleted(
	FText MissionTitle)
{
	if (MissionTrackerWidget)
	{
		MissionTrackerWidget->SetMissionText(
			MissionTitle,
			FText::FromString(TEXT("미션 완료"))
		);
	}

	if (ObjectiveMarkerWidget)
	{
		ObjectiveMarkerWidget->SetTargetActor(nullptr);
	}
}

void AKittyPlayerController::StartInitialMission()
{
	if (!MissionSubsystem || !InitialMission)
	{
		return;
	}

	MissionSubsystem->StartMission(InitialMission);
}


void AKittyPlayerController::ToggleInventory()
{
	if (bIsInventoryOpen)
	{
		CloseInventory();
	}
	else
	{
		OpenInventory();
	}
}

void AKittyPlayerController::OpenInventory()
{
	if (!IsLocalController() || bIsInventoryOpen)
	{
		return;
	}

	AKittyCharacterPlayer* PlayerCharacter =
		Cast<AKittyCharacterPlayer>(GetPawn());

	if (!IsValid(PlayerCharacter) || !InventoryWidgetClass)
	{
		return;
	}

	if (!InventoryWidget)
	{
		InventoryWidget = CreateWidget<UKTInventoryWidget>(
			this,
			InventoryWidgetClass
		);

		if (!InventoryWidget)
		{
			return;
		}

		InventoryWidget->AddToViewport(20);
	}

	InventoryWidget->SetInventoryComponent(
		PlayerCharacter->GetInventoryComponent()
	);

	InventoryWidget->SetVisibility(
		ESlateVisibility::Visible
	);
	
	InventoryWidget->PlayOpenAnimation();
	
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(
		InventoryWidget->TakeWidget()
	);

	SetInputMode(InputMode);
	bShowMouseCursor = true;

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	// 인벤토리가 열린 동안 미션 HUD를 숨깁니다.
	if (MissionTrackerWidget)
	{
		MissionTrackerWidget->SetVisibility(
			ESlateVisibility::Collapsed
		);
	}

	// 목적지 마커도 함께 숨깁니다.
	if (ObjectiveMarkerWidget)
	{
		ObjectiveMarkerWidget->SetVisibility(
			ESlateVisibility::Collapsed
		);
	}
	
	bIsInventoryOpen = true;
}

void AKittyPlayerController::CloseInventory()
{
	if (!bIsInventoryOpen)
	{
		return;
	}

	if (InventoryWidget)
	{
		InventoryWidget->SetVisibility(
			ESlateVisibility::Collapsed
		);
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	// 게임 화면으로 돌아오면 미션 HUD를 다시 표시합니다.
	if (MissionTrackerWidget)
	{
		MissionTrackerWidget->SetVisibility(
			ESlateVisibility::Visible
		);
	}

	if (ObjectiveMarkerWidget)
	{
		ObjectiveMarkerWidget->SetVisibility(
			ESlateVisibility::Visible
		);
	}
	
	bIsInventoryOpen = false;
}
void AKittyPlayerController::ShowObjectiveInteractionPrompt(
	AActor* InteractableActor,
	const FText& PromptText)
{
	if (!ObjectiveMarkerWidget)
	{
		return;
	}

	const bool bIsCurrentMissionTarget =
		ObjectiveMarkerWidget->IsTrackingActor(
			InteractableActor
		);

	ObjectiveMarkerWidget->SetInteractionPrompt(
		bIsCurrentMissionTarget,
		PromptText
	);
}

void AKittyPlayerController::HideObjectiveInteractionPrompt()
{
	if (ObjectiveMarkerWidget)
	{
		ObjectiveMarkerWidget->SetInteractionPrompt(
			false,
			FText::GetEmpty()
		);
	}
}