#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "KTSpectralVisionSubsystem.generated.h"

class UKTSpectralTargetComponent;

/**
 * 현재 World에 존재하는 Spectral Target 목록과
 * Vision 활성화 상태를 관리합니다.
 */
UCLASS()
class KITTYPROJECT_API UKTSpectralVisionSubsystem
	: public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	/**
	 * 새 Spectral Target을 등록합니다.
	 *
	 * Vision이 이미 활성화돼 있다면 새 대상에도
	 * 즉시 Highlight를 적용합니다.
	 */
	void RegisterTarget(
		UKTSpectralTargetComponent* Target
	);

	/**
	 * Spectral Target을 등록 목록에서 제거합니다.
	 */
	void UnregisterTarget(
		UKTSpectralTargetComponent* Target
	);

	/**
	 * 현재 World의 Spectral Vision 상태를 변경합니다.
	 */
	UFUNCTION(
		BlueprintCallable,
		Category = "Spectral Vision"
	)
	void SetVisionEnabled(bool bEnabled);

	/**
	 * 현재 Spectral Vision 상태를 반환합니다.
	 */
	UFUNCTION(
		BlueprintPure,
		Category = "Spectral Vision"
	)
	bool IsVisionEnabled() const
	{
		return bVisionEnabled;
	}

	/**
	 * 현재 상태를 등록된 모든 대상에 다시 적용합니다.
	 */
	UFUNCTION(
		BlueprintCallable,
		Category = "Spectral Vision"
	)
	void RefreshAllTargets();

private:
	/**
	 * 파괴된 Target을 등록 목록에서 제거합니다.
	 */
	void RemoveInvalidTargets();

private:
	/**
	 * 현재 World에 등록된 Spectral Target 목록입니다.
	 *
	 * Weak Pointer이므로 Target의 수명을 강제로
	 * 연장하지 않습니다.
	 */
	TArray<TWeakObjectPtr<UKTSpectralTargetComponent>>
		RegisteredTargets;

	bool bVisionEnabled = false;
};