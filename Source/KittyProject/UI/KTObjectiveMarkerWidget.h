#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTObjectiveMarkerWidget.generated.h"

class AActor;
class UImage;
class UTextBlock;

UCLASS()
class KITTYPROJECT_API UKTObjectiveMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetTargetActor(AActor* NewTargetActor);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Arrow;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Distance;

private:
	TWeakObjectPtr<AActor> TargetActor;

	void UpdateMarker();
};