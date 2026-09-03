#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "KTSpectralVisionSubsystem.generated.h"

class UKTSpectralTargetComponent;

/**
 * 현재 World의 Spectral Target 목록과
 * Vision 활성화 상태를 관리합니다.
 */
UCLASS()
class KITTYPROJECT_API UKTSpectralVisionSubsystem
	: public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	void RegisterTarget(
		UKTSpectralTargetComponent* Target
	);

	void UnregisterTarget(
		UKTSpectralTargetComponent* Target
	);

	UFUNCTION(
		BlueprintCallable,
		Category = "Spectral Vision"
	)
	void SetVisionEnabled(bool bEnabled);

	UFUNCTION(
		BlueprintPure,
		Category = "Spectral Vision"
	)
	bool IsVisionEnabled() const
	{
		return bVisionEnabled;
	}

	/**
	 * 플레이어 위치를 기준으로 범위 안의 Target만 표시합니다.
	 */
	void UpdateTargetsInRange(
		const FVector& ViewerLocation,
		float HighlightRange
	);

	/**
	 * 현재 저장된 위치와 거리 조건을 다시 적용합니다.
	 */
	void RefreshAllTargets();

private:
	void RemoveInvalidTargets();

	bool IsTargetInRange(
		const UKTSpectralTargetComponent* Target
	) const;

private:
	TArray<TWeakObjectPtr<UKTSpectralTargetComponent>>
		RegisteredTargets;

	bool bVisionEnabled = false;

	bool bHasViewerLocation = false;

	FVector LastViewerLocation = FVector::ZeroVector;

	float CurrentHighlightRange = 3000.0f;
};