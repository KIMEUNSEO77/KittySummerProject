#include "Vision/KTSpectralVisionSubsystem.h"

#include "Vision/KTSpectralTargetComponent.h"

void UKTSpectralVisionSubsystem::Deinitialize()
{
	// World가 제거되기 전에 남아 있는 대상의
	// Custom Depth를 안전하게 비활성화합니다.
	for (const TWeakObjectPtr<UKTSpectralTargetComponent>&
		TargetPtr : RegisteredTargets)
	{
		if (UKTSpectralTargetComponent* Target =
			TargetPtr.Get())
		{
			Target->SetHighlightEnabled(false);
		}
	}

	RegisteredTargets.Reset();
	bVisionEnabled = false;

	Super::Deinitialize();
}

void UKTSpectralVisionSubsystem::RegisterTarget(
	UKTSpectralTargetComponent* Target)
{
	if (!IsValid(Target))
	{
		return;
	}

	RemoveInvalidTargets();

	RegisteredTargets.AddUnique(
		TWeakObjectPtr<UKTSpectralTargetComponent>(
			Target
		)
	);

	// Vision이 이미 켜진 상태에서 생성된 대상도
	// 즉시 현재 상태를 적용받습니다.
	Target->SetHighlightEnabled(bVisionEnabled);
}

void UKTSpectralVisionSubsystem::UnregisterTarget(
	UKTSpectralTargetComponent* Target)
{
	RegisteredTargets.RemoveAllSwap(
		[Target](
			const TWeakObjectPtr<
				UKTSpectralTargetComponent>& TargetPtr)
		{
			return !TargetPtr.IsValid() ||
				TargetPtr.Get() == Target;
		}
	);
}

void UKTSpectralVisionSubsystem::SetVisionEnabled(
	const bool bEnabled)
{
	if (bVisionEnabled == bEnabled)
	{
		// 같은 상태를 다시 요청하더라도 런타임에
		// 변경된 Mesh나 Target 설정을 반영합니다.
		RefreshAllTargets();
		return;
	}

	bVisionEnabled = bEnabled;
	RefreshAllTargets();
}

void UKTSpectralVisionSubsystem::RefreshAllTargets()
{
	RemoveInvalidTargets();

	for (const TWeakObjectPtr<UKTSpectralTargetComponent>&
		TargetPtr : RegisteredTargets)
	{
		if (UKTSpectralTargetComponent* Target =
			TargetPtr.Get())
		{
			Target->SetHighlightEnabled(
				bVisionEnabled
			);
		}
	}
}

void UKTSpectralVisionSubsystem::RemoveInvalidTargets()
{
	RegisteredTargets.RemoveAllSwap(
		[](
			const TWeakObjectPtr<
				UKTSpectralTargetComponent>& TargetPtr)
		{
			return !TargetPtr.IsValid();
		}
	);
}