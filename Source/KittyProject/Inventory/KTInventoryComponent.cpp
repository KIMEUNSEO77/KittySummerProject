#include "Inventory/KTInventoryComponent.h"

#include "Inventory/KTItemDataAsset.h"

UKTInventoryComponent::UKTInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UKTInventoryComponent::AddItem(
	UKTItemDataAsset* ItemData,
	int32 Amount)
{
	if (!IsValid(ItemData) || Amount <= 0)
	{
		return false;
	}

	// 이미 가진 아이템이면 우선 기존 슬롯에 수량을 더합니다.
	for (FKTInventoryEntry& Entry : Items)
	{
		if (Entry.ItemData == ItemData &&
			Entry.Quantity < ItemData->MaxStackSize)
		{
			const int32 AddableAmount =
				ItemData->MaxStackSize - Entry.Quantity;

			const int32 AddedAmount =
				FMath::Min(Amount, AddableAmount);

			Entry.Quantity += AddedAmount;
			Amount -= AddedAmount;

			if (Amount <= 0)
			{
				OnInventoryChanged.Broadcast();
				return true;
			}
		}
	}

	// 기존 슬롯이 없거나 최대 수량에 도달했다면 새 슬롯을 만듭니다.
	while (Amount > 0)
	{
		FKTInventoryEntry NewEntry;
		NewEntry.ItemData = ItemData;
		NewEntry.Quantity = FMath::Min(
			Amount,
			FMath::Max(1, ItemData->MaxStackSize)
		);

		Items.Add(NewEntry);
		Amount -= NewEntry.Quantity;
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UKTInventoryComponent::RemoveItem(
	UKTItemDataAsset* ItemData,
	int32 Amount)
{
	if (!IsValid(ItemData) || Amount <= 0)
	{
		return false;
	}

	for (int32 Index = Items.Num() - 1; Index >= 0; --Index)
	{
		FKTInventoryEntry& Entry = Items[Index];

		if (Entry.ItemData != ItemData)
		{
			continue;
		}

		const int32 RemovedAmount =
			FMath::Min(Amount, Entry.Quantity);

		Entry.Quantity -= RemovedAmount;
		Amount -= RemovedAmount;

		if (Entry.Quantity <= 0)
		{
			Items.RemoveAt(Index);
		}

		if (Amount <= 0)
		{
			OnInventoryChanged.Broadcast();
			return true;
		}
	}

	// 필요한 수량을 전부 제거하지 못한 경우입니다.
	return false;
}

bool UKTInventoryComponent::HasItem(
	UKTItemDataAsset* ItemData) const
{
	if (!IsValid(ItemData))
	{
		return false;
	}

	for (const FKTInventoryEntry& Entry : Items)
	{
		if (Entry.ItemData == ItemData && Entry.Quantity > 0)
		{
			return true;
		}
	}

	return false;
}

bool UKTInventoryComponent::HasItemByID(
	FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return false;
	}

	for (const FKTInventoryEntry& Entry : Items)
	{
		if (IsValid(Entry.ItemData) &&
			Entry.ItemData->ItemID == ItemID &&
			Entry.Quantity > 0)
		{
			return true;
		}
	}

	return false;
}