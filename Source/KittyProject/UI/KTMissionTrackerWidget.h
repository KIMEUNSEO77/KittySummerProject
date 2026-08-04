// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTMissionTrackerWidget.generated.h"

class UTextBlock;

UCLASS()
class KITTYPROJECT_API UKTMissionTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// PlayerController가 미션 제목과 목표 문구를 전달할 때 사용합니다.
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void SetMissionText(const FText& InMissionTitle, const FText& InObjectiveText);

protected:
	// Widget Blueprint 안의 TextBlock 이름과 정확히 일치해야 합니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_MissionTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_ObjectiveText;
};