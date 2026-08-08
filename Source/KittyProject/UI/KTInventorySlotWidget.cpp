#include "UI/KTInventorySlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/KTItemDataAsset.h"

void UKTInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Item)
	{
		Btn_Item->OnClicked.AddDynamic(
			this,
			&UKTInventorySlotWidget::HandleItemButtonClicked
		);
	}
}

void UKTInventorySlotWidget::SetItemEntry(
	UKTItemDataAsset* NewItemData,
	int32 NewQuantity)
{
	ItemData = NewItemData;
	Quantity = NewQuantity;

	if (!IsValid(ItemData))
	{
		return;
	}

	if (Img_Icon && !ItemData->Icon.IsNull())
	{
		Img_Icon->SetBrushFromSoftTexture(
			ItemData->Icon,
			false
		);
	}

	if (Txt_Quantity)
	{
		Txt_Quantity->SetText(
			FText::AsNumber(Quantity)
		);
	}
}

void UKTInventorySlotWidget::HandleItemButtonClicked()
{
	if (IsValid(ItemData))
	{
		OnSlotClicked.Broadcast(ItemData);
	}
}