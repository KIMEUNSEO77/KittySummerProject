#include "UI/KTInventoryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Inventory/KTInventoryComponent.h"
#include "Inventory/KTItemDataAsset.h"
#include "UI/KTInventorySlotWidget.h"

void UKTInventoryWidget::SetInventoryComponent(
	UKTInventoryComponent* NewInventoryComponent)
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UKTInventoryWidget::HandleInventoryChanged
		);
	}

	InventoryComponent = NewInventoryComponent;

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(
			this,
			&UKTInventoryWidget::HandleInventoryChanged
		);
	}

	RefreshInventory();
}

void UKTInventoryWidget::NativeDestruct()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UKTInventoryWidget::HandleInventoryChanged
		);
	}

	Super::NativeDestruct();
}

void UKTInventoryWidget::HandleInventoryChanged()
{
	RefreshInventory();
}

void UKTInventoryWidget::RefreshInventory()
{
	if (!Wrap_ItemList)
	{
		return;
	}

	Wrap_ItemList->ClearChildren();

	if (!InventoryComponent || !InventorySlotClass)
	{
		return;
	}

	for (const FKTInventoryEntry& Entry :
		InventoryComponent->GetItems())
	{
		if (!IsValid(Entry.ItemData) || Entry.Quantity <= 0)
		{
			continue;
		}

		UKTInventorySlotWidget* NewSlot =
			CreateWidget<UKTInventorySlotWidget>(
				GetOwningPlayer(),
				InventorySlotClass
			);

		if (!NewSlot)
		{
			continue;
		}

		NewSlot->SetItemEntry(
			Entry.ItemData,
			Entry.Quantity
		);

		NewSlot->OnSlotClicked.AddDynamic(
			this,
			&UKTInventoryWidget::HandleSlotClicked
		);

		Wrap_ItemList->AddChildToWrapBox(NewSlot);
	}
}

void UKTInventoryWidget::HandleSlotClicked(
	UKTItemDataAsset* ItemData)
{
	UpdateInspectPanel(ItemData);
}

void UKTInventoryWidget::UpdateInspectPanel(
	UKTItemDataAsset* ItemData)
{
	if (!IsValid(ItemData))
	{
		return;
	}

	if (Txt_ItemName)
	{
		Txt_ItemName->SetText(ItemData->DisplayName);
	}

	if (Txt_ItemDescription)
	{
		Txt_ItemDescription->SetText(ItemData->Description);
	}

	if (Img_InspectImage)
	{
		if (ItemData->bCanInspect &&
			!ItemData->InspectImage.IsNull())
		{
			Img_InspectImage->SetBrushFromSoftTexture(
				ItemData->InspectImage,
				false
			);

			Img_InspectImage->SetVisibility(
				ESlateVisibility::Visible
			);
		}
		else
		{
			Img_InspectImage->SetVisibility(
				ESlateVisibility::Collapsed
			);
		}
	}
}

void UKTInventoryWidget::PlayOpenAnimation()
{
	if (Anim_Open)
	{
		PlayAnimation(Anim_Open);
	}
}