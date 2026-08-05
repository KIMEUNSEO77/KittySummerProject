// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTMissionNotificationWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class KITTYPROJECT_API UKTMissionNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void ShowNewMission(
		FText InMissionTitle,
		FText InObjectiveText
	);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Announcement;
/*
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_MissionTitle;
*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_ObjectiveText;

	// Widget Blueprint에서 만들 Animation 이름과 같아야 합니다.
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_NewMission;

private:
	void HideNotification();

	FTimerHandle HideTimerHandle;
};
