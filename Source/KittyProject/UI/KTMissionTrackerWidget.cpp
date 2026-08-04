// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KTMissionTrackerWidget.h"
#include "Components/TextBlock.h"

void UKTMissionTrackerWidget::SetMissionText(
	const FText& InMissionTitle,
	const FText& InObjectiveText)
{
	if (Txt_MissionTitle)
	{
		Txt_MissionTitle->SetText(InMissionTitle);
	}

	if (Txt_ObjectiveText)
	{
		Txt_ObjectiveText->SetText(InObjectiveText);
	}
}
