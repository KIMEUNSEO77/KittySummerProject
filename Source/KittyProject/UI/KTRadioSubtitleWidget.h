// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTRadioSubtitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UKTRadioSubtitleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Radio")
	void ShowRadioSubtitle(const FText& SpeakerName, const FText& SubtitleText);

	UFUNCTION(BlueprintCallable, Category = "Radio")
	void HideRadioSubtitle();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_SpeakerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Subtitle;
};
