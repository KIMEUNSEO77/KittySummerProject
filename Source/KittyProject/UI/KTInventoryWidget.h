#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KTInventoryWidget.generated.h"

class UWrapBox;
class UImage;
class UTextBlock;
class UKTInventoryComponent;
class UKTInventorySlotWidget;
class UKTItemDataAsset;

UCLASS()
class KITTYPROJECT_API UKTInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// PlayerController가 인벤토리 컴포넌트를 연결합니다.
	void SetInventoryComponent(
		UKTInventoryComponent* NewInventoryComponent
	);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleInventoryChanged();

	UFUNCTION()
	void HandleSlotClicked(UKTItemDataAsset* ItemData);

	void UpdateInspectPanel(UKTItemDataAsset* ItemData);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> Wrap_ItemList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_InspectImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_ItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_ItemDescription;

	// 에디터에서 WBP_InventorySlot을 연결합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UKTInventorySlotWidget> InventorySlotClass;

private:
	UPROPERTY()
	TObjectPtr<UKTInventoryComponent> InventoryComponent;
};