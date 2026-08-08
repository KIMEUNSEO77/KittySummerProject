#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KTInventoryComponent.generated.h"

class UKTItemDataAsset;

USTRUCT(BlueprintType)
struct FKTInventoryEntry
{
	GENERATED_BODY()

	// 어떤 아이템인지 정의한 데이터 에셋입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UKTItemDataAsset> ItemData;

	// 현재 소지 수량입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

/**
 * 플레이어가 보유한 일반 아이템 목록을 관리합니다.
 *
 * 카드키, 문서, 메모, 증거물 등에 사용합니다.
 * 권총처럼 캐릭터에 장착되는 무기는 현재 별도 구조를 유지합니다.
 */

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KITTYPROJECT_API UKTInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKTInventoryComponent();

	// 아이템을 인벤토리에 추가합니다.
	// 성공하면 true, 잘못된 아이템이면 false를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UKTItemDataAsset* ItemData, int32 Amount = 1);

	// 아이템을 인벤토리에서 제거합니다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UKTItemDataAsset* ItemData, int32 Amount = 1);

	// 해당 데이터 에셋 아이템을 갖고 있는지 확인합니다.
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(UKTItemDataAsset* ItemData) const;

	// ItemID 문자열로 아이템 소지 여부를 확인합니다.
	// 문이나 카드키 리더기가 검사할 때 주로 사용합니다.
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItemByID(FName ItemID) const;

	// C++ UI가 현재 아이템 목록을 읽을 때 사용합니다.
	const TArray<FKTInventoryEntry>& GetItems() const
	{
		return Items;
	}

	// 아이템 목록이 바뀔 때 UI에 알립니다.
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

private:
	// 실제 플레이어 소지 목록입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FKTInventoryEntry> Items;
};