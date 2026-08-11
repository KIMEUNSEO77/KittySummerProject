#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTObjectiveMarkerWidget.generated.h"

class AActor;
class UImage;
class UTextBlock;
class UWidget;

UCLASS()
class KITTYPROJECT_API UKTObjectiveMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 미션 시스템이 현재 추적할 목적지 Actor를 설정합니다.
	void SetTargetActor(AActor* NewTargetActor);

	// 지정된 Actor가 현재 이 마커의 목적지인지 확인합니다.
	bool IsTrackingActor(const AActor* Actor) const;

	// 일반 목적지 마커와 상호작용 UI 상태를 전환합니다.
	void SetInteractionPrompt(
		bool bShouldShowInteraction,
		const FText& PromptText
	);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

	// 기존 목적지 마커 영역입니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> Panel_Marker;

	// F 키 + 상호작용 문구 영역입니다.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> Panel_Interaction;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Arrow;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Distance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_InteractionText;

private:
	TWeakObjectPtr<AActor> TargetActor;

	bool bShowingInteractionPrompt = false;

	void UpdateMarker();
	void UpdateDisplayMode();
};