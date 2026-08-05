// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/KittyPlayerController.h"

#include "UI/KTMissionTrackerWidget.h"
#include "UI/KTMissionNotificationWidget.h"
#include "Mission/KTMissionSubsystem.h"
#include "UI/KTObjectiveMarkerWidget.h"
#include "Mission/KTMissionDataAsset.h"
#include "TimerManager.h"

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
