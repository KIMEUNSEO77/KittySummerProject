#include "UI/KTExamineWidget.h"

#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
#include "Inventory/KTItemDataAsset.h"

UKTExamineWidget::UKTExamineWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = true;
}

void UKTExamineWidget::SetItemData(
	const UKTItemDataAsset* NewItemData)
{
	if (!IsValid(NewItemData))
	{
		return;
	}

	if (Txt_ItemName)
	{
		Txt_ItemName->SetText(
			NewItemData->DisplayName
		);
	}

	if (Txt_ItemDescription)
	{
		Txt_ItemDescription->SetText(
			NewItemData->Description
		);
	}
}

void UKTExamineWidget::PlayOpenAnimation()
{
	if (Anim_Open)
	{
		PlayAnimation(Anim_Open);
	}
}

FReply UKTExamineWidget::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnCloseRequested.Broadcast();

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(
		InGeometry,
		InKeyEvent
	);
}