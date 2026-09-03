#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Vision/KTSpectralVisionTypes.h"
#include "KTSpectralTargetComponent.generated.h"

class UPrimitiveComponent;

/**
 * Spectral Vision으로 표시될 Actor에 부착하는 컴포넌트입니다.
 *
 * BeginPlay에서 SpectralVisionSubsystem에 자동 등록하고,
 * Owner가 가진 Skeletal Mesh 및 Static Mesh의
 * Custom Depth/Stencil 상태를 관리합니다.
 */
UCLASS(
	ClassGroup = (SpectralVision),
	meta = (BlueprintSpawnableComponent)
)
class KITTYPROJECT_API UKTSpectralTargetComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UKTSpectralTargetComponent();

	/**
	 * Spectral Highlight 표시를 요청합니다.
	 *
	 * bEnabled가 true여도 TargetType이 None이거나
	 * bCanBeHighlighted가 false이면 실제 표시되지 않습니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Spectral Vision")
	void SetHighlightEnabled(bool bEnabled);

	/**
	 * 대상 종류를 변경합니다.
	 *
	 * Highlight가 활성화된 상태라면 변경된 종류의
	 * Stencil 값을 즉시 Mesh에 다시 적용합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Spectral Vision")
	void SetTargetType(EKTSpectralTargetType NewType);

	/**
	 * TargetType에 대응하는 Custom Stencil 값을 반환합니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Spectral Vision")
	uint8 GetStencilValue() const;

	UFUNCTION(BlueprintPure, Category = "Spectral Vision")
	EKTSpectralTargetType GetTargetType() const
	{
		return TargetType;
	}

	UFUNCTION(BlueprintPure, Category = "Spectral Vision")
	bool CanBeHighlighted() const
	{
		return bCanBeHighlighted;
	}

	UFUNCTION(BlueprintPure, Category = "Spectral Vision")
	bool ShouldHideWhenDead() const
	{
		return bHideWhenDead;
	}

	UFUNCTION(BlueprintPure, Category = "Spectral Vision")
	bool IsHighlightEnabled() const
	{
		return bHighlightEnabled;
	}

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:
	/**
	 * 에디터에서 Mesh를 직접 지정했다면 해당 Mesh를 사용하고,
	 * 배열이 비어 있으면 Owner의 Skeletal/Static Mesh를
	 * 자동으로 수집합니다.
	 */
	void CacheHighlightMeshes();

	/**
	 * 현재 설정에 맞춰 실제 Mesh 상태를 적용합니다.
	 */
	void ApplyHighlightState();

	/**
	 * Highlight 대상으로 사용할 수 있는 Mesh인지 검사합니다.
	 */
	bool IsSupportedHighlightMesh(
		const UPrimitiveComponent* Component
	) const;

private:
	/**
	 * 대상 표시 종류입니다.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Spectral Vision",
		meta = (AllowPrivateAccess = "true")
	)
	EKTSpectralTargetType TargetType =
		EKTSpectralTargetType::Guard;

	/**
	 * false이면 Vision이 활성화되어도 표시되지 않습니다.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Spectral Vision",
		meta = (AllowPrivateAccess = "true")
	)
	bool bCanBeHighlighted = true;

	/**
	 * 사망한 NPC의 Highlight를 숨길 때 사용할 옵션입니다.
	 *
	 * 실제 사망 이벤트 연결은 다음 단계에서 처리합니다.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Spectral Vision",
		meta = (AllowPrivateAccess = "true")
	)
	bool bHideWhenDead = true;

	/**
	 * 직접 지정할 Mesh 목록입니다.
	 *
	 * 비워두면 Owner의 Skeletal Mesh와 Static Mesh를
	 * 자동으로 찾아 사용합니다.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Spectral Vision",
		meta = (AllowPrivateAccess = "true")
	)
	TArray<TObjectPtr<UPrimitiveComponent>> HighlightMeshes;

	/**
	 * 실행 중 사용하는 Mesh 캐시입니다.
	 *
	 * Weak Pointer를 사용하므로 Mesh가 파괴되어도
	 * 해당 Mesh의 수명을 강제로 연장하지 않습니다.
	 */
	TArray<TWeakObjectPtr<UPrimitiveComponent>>
		CachedHighlightMeshes;

	/**
	 * Subsystem이 요청한 활성화 상태입니다.
	 */
	bool bHighlightRequested = false;

	/**
	 * 현재 실제로 Highlight가 적용됐는지 나타냅니다.
	 */
	bool bHighlightEnabled = false;
};