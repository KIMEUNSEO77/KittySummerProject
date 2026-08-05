#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KTMissionTypes.generated.h"

UENUM(BlueprintType)
enum class EMissionState : uint8
{
	Inactive,
	Active,
	Completed,
	Failed
};

USTRUCT(BlueprintType)
struct FMissionStep
{
	GENERATED_BODY()

	// 프로그래머가 구분하는 Step 이름입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FName StepID;

	// HUD에 표시할 목표 문구입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FText Description;

	// 이 태그가 들어오면 Step을 완료합니다.
	// 예: Mission.Event.Item.Pistol.Acquired
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FGameplayTag CompletionEventTag;

	// 이 태그가 들어오면 Step이 실패합니다.
	// 예: Mission.Event.Stealth.Detected
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FGameplayTag FailureEventTag;

	// 맵에 배치된 목적지를 찾기 위한 태그입니다.
	// 예: Mission.Target.Pistol
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FGameplayTag TargetId;

	// 이 Step에서 목적지 마커를 표시할지 결정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	bool bShowMarker = true;
};