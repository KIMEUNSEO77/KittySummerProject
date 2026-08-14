#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTExamineWidget.generated.h"

class UTextBlock;
class UKTItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExamineCloseRequested);

UCLASS()
class KITTYPROJECT_API UKTExamineWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UKTExamineWidget(
		const FObjectInitializer& ObjectInitializer
	);

	void SetItemData(
		const UKTItemDataAsset* NewItemData
	);
	
	UFUNCTION(BlueprintCallable, Category = "Examine|Animation")
	void PlayOpenAnimation();
	
	UPROPERTY(BlueprintAssignable, Category = "Examine")
	FOnExamineCloseRequested OnCloseRequested;

protected:
	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_ItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_ItemDescription;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Open;
};