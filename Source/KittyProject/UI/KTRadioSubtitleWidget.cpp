// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KTRadioSubtitleWidget.h"
#include "Components/TextBlock.h"


void UKTRadioSubtitleWidget::ShowRadioSubtitle(const FText& SpeakerName, const FText& SubtitleText)
{
	if (IsValid(Txt_SpeakerName))
	{
		Txt_SpeakerName->SetText(SpeakerName);
	}

	if (IsValid(Txt_Subtitle))
	{
		Txt_Subtitle->SetText(SubtitleText);
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UKTRadioSubtitleWidget::HideRadioSubtitle()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UKTRadioSubtitleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
}
