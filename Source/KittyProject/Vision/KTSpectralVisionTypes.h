#pragma once

#include "CoreMinimal.h"
#include "KTSpectralVisionTypes.generated.h"

/**
 * Spectral Vision에서 대상을 어떤 색상으로 표시할지 구분합니다.
 */
UENUM(BlueprintType)
enum class EKTSpectralTargetType : uint8
{
	None UMETA(DisplayName = "None"),

	// 일반 경비원: 흰색 또는 회색
	Guard UMETA(DisplayName = "Guard"),

	// 중요 타깃: 빨간색
	PrimaryTarget UMETA(DisplayName = "Primary Target"),

	// 중요 상호작용 오브젝트: 노란색
	Interactable UMETA(DisplayName = "Interactable")
};

/**
 * Post Process Material에서 판별할 Custom Stencil 값입니다.
 *
 * 이 숫자를 여러 클래스에 직접 반복해서 쓰지 않고
 * 이곳에서 한 번만 관리합니다.
 */
namespace KTSpectralStencil
{
	constexpr uint8 None = 0;
	constexpr uint8 Guard = 1;
	constexpr uint8 PrimaryTarget = 2;
	constexpr uint8 Interactable = 3;
}