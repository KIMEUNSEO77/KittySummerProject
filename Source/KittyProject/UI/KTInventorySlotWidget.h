#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTInventorySlotWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UKTItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInventorySlotClicked,
	UKTItemDataAsset*,
	ItemData
);

UCLASS()
class KITTYPROJECT_API UKTInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItemEntry(
		UKTItemDataAsset* NewItemData,
		int32 NewQuantity
	);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventorySlotClicked OnSlotClicked;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleItemButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Item;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Quantity;

private:
	UPROPERTY()
	TObjectPtr<UKTItemDataAsset> ItemData;

	int32 Quantity = 0;
};