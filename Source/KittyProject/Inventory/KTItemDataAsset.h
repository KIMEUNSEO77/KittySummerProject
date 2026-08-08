// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "KTItemDataAsset.generated.h"

class UTexture2D;

/**
 * 아이템 하나의 고정 정보
 *
 * 예:
 * - 카드키의 이름, 설명, 아이콘
 * - 메모의 조사 이미지
 * - 획득했을 때 보낼 미션 이벤트 태그
 *
 * 실제 플레이어 소지 수량은 InventoryComponent에 저장
 */

UCLASS(BlueprintType)
class KITTYPROJECT_API UKTItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 아이템을 구분하는 고유 이름입니다.
	// 예: Keycard_A, Receipt_01, Note_Office
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemID;

	// 인벤토리와 조사 화면에 표시할 아이템 이름입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	// 인벤토리에서 아이템을 선택했을 때 표시할 설명입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText Description;

	// 인벤토리 슬롯에 표시할 작은 아이콘입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Visual")
	TSoftObjectPtr<UTexture2D> Icon;

	// 조사 화면에 크게 표시할 이미지입니다.
	// 카드키, 메모, 영수증 등의 앞면 이미지에 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Visual")
	TSoftObjectPtr<UTexture2D> InspectImage;

	// 같은 아이템을 한 칸에 몇 개까지 쌓을 수 있는지입니다.
	// 카드키·문서·증거물은 우선 1로 설정하면 됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 MaxStackSize = 1;

	// 인벤토리에서 이 아이템을 선택해 상세 설명을 볼 수 있는지입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool bCanInspect = true;

	// 이 아이템 획득 후 미션 시스템에 보낼 이벤트 태그입니다.
	// 미션과 무관한 아이템이면 비워 둬도 됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Mission")
	FGameplayTag AcquiredEventTag;
};
