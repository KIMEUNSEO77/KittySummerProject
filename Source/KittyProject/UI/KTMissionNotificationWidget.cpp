// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/KTMissionNotificationWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UKTMissionNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
}

void UKTMissionNotificationWidget::ShowNewMission(
	FText InMissionTitle,
	FText InObjectiveText)
{
	if (Txt_Announcement)
	{
		Txt_Announcement->SetText(
			FText::FromString(TEXT("새로운 미션"))
		);
	}
/*
	if (Txt_MissionTitle)
	{
		Txt_MissionTitle->SetText(InMissionTitle);
	}
*/
	if (Txt_ObjectiveText)
	{
		Txt_ObjectiveText->SetText(InObjectiveText);
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (Anim_NewMission)
	{
		PlayAnimation(Anim_NewMission);
	}

	GetWorld()->GetTimerManager().ClearTimer(HideTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		HideTimerHandle,
		this,
		&UKTMissionNotificationWidget::HideNotification,
		1.5f,
		false
	);
}

void UKTMissionNotificationWidget::HideNotification()
{
	SetVisibility(ESlateVisibility::Collapsed);
}