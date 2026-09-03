#include "Vision/KTSpectralVisionSubsystem.h"

#include "GameFramework/Actor.h"
#include "Vision/KTSpectralTargetComponent.h"

void UKTSpectralVisionSubsystem::Deinitialize()
{
    for (
        const TWeakObjectPtr<UKTSpectralTargetComponent>& TargetPtr
        : RegisteredTargets
    )
    {
        if (UKTSpectralTargetComponent* Target = TargetPtr.Get())
        {
            Target->SetHighlightEnabled(false);
        }
    }

    RegisteredTargets.Reset();

    bVisionEnabled = false;
    bHasViewerLocation = false;

    Super::Deinitialize();
}

void UKTSpectralVisionSubsystem::RegisterTarget(
    UKTSpectralTargetComponent* Target
)
{
    if (!IsValid(Target))
    {
        return;
    }

    RemoveInvalidTargets();

    RegisteredTargets.AddUnique(
        TWeakObjectPtr<UKTSpectralTargetComponent>(Target)
    );

    // Vision이 꺼져 있거나 플레이어 위치를 아직 모르면
    // 새로 등록된 Target을 표시하지 않습니다.
    const bool bShouldHighlight =
        bVisionEnabled &&
        bHasViewerLocation &&
        IsTargetInRange(Target);

    Target->SetHighlightEnabled(bShouldHighlight);
}

void UKTSpectralVisionSubsystem::UnregisterTarget(
    UKTSpectralTargetComponent* Target
)
{
    RegisteredTargets.RemoveAllSwap(
        [Target](
            const TWeakObjectPtr<
                UKTSpectralTargetComponent
            >& TargetPtr
        )
        {
            return !TargetPtr.IsValid() ||
                TargetPtr.Get() == Target;
        }
    );
}

void UKTSpectralVisionSubsystem::SetVisionEnabled(
    const bool bEnabled
)
{
    bVisionEnabled = bEnabled;

    if (!bVisionEnabled)
    {
        // Vision을 끌 때는 거리와 무관하게 전부 해제합니다.
        for (
            const TWeakObjectPtr<UKTSpectralTargetComponent>& TargetPtr
            : RegisteredTargets
        )
        {
            if (UKTSpectralTargetComponent* Target = TargetPtr.Get())
            {
                Target->SetHighlightEnabled(false);
            }
        }

        return;
    }

    RefreshAllTargets();
}

void UKTSpectralVisionSubsystem::UpdateTargetsInRange(
    const FVector& ViewerLocation,
    const float HighlightRange
)
{
    LastViewerLocation = ViewerLocation;
    CurrentHighlightRange = FMath::Max(
        0.0f,
        HighlightRange
    );

    bHasViewerLocation = true;

    if (!bVisionEnabled)
    {
        return;
    }

    RefreshAllTargets();
}

void UKTSpectralVisionSubsystem::RefreshAllTargets()
{
    RemoveInvalidTargets();

    for (
        const TWeakObjectPtr<UKTSpectralTargetComponent>& TargetPtr
        : RegisteredTargets
    )
    {
        UKTSpectralTargetComponent* Target =
            TargetPtr.Get();

        if (!IsValid(Target))
        {
            continue;
        }

        const bool bShouldHighlight =
            bVisionEnabled &&
            bHasViewerLocation &&
            IsTargetInRange(Target);

        Target->SetHighlightEnabled(
            bShouldHighlight
        );
    }
}

bool UKTSpectralVisionSubsystem::IsTargetInRange(
    const UKTSpectralTargetComponent* Target
) const
{
    if (!IsValid(Target))
    {
        return false;
    }

    const AActor* TargetOwner = Target->GetOwner();

    if (!IsValid(TargetOwner))
    {
        return false;
    }

    const float DistanceSquared =
        FVector::DistSquared(
            LastViewerLocation,
            TargetOwner->GetActorLocation()
        );

    const float RangeSquared =
        FMath::Square(CurrentHighlightRange);

    return DistanceSquared <= RangeSquared;
}

void UKTSpectralVisionSubsystem::RemoveInvalidTargets()
{
    RegisteredTargets.RemoveAllSwap(
        [](
            const TWeakObjectPtr<
                UKTSpectralTargetComponent
            >& TargetPtr
        )
        {
            return !TargetPtr.IsValid();
        }
    );
}