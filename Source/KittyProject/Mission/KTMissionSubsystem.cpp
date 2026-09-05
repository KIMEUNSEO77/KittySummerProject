#include "Mission/KTMissionSubsystem.h"

#include "Mission/KTMissionDataAsset.h"

UKTMissionSubsystem* UKTMissionSubsystem::Get(
    const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();

    if (!World)
    {
        return nullptr;
    }

    return World->GetSubsystem<UKTMissionSubsystem>();
}

void UKTMissionSubsystem::StartMission(
    UKTMissionDataAsset* NewMission)
{
    if (!NewMission || NewMission->Steps.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("시작할 수 있는 미션 Step이 없습니다."));
        return;
    }

    CurrentMission = NewMission;
    CurrentStepIndex = 0;
    CurrentState = EMissionState::Active;

    BroadcastCurrentStep();
}

void UKTMissionSubsystem::BroadcastMissionEvent(
    FGameplayTag EventTag,
    AActor* InstigatorActor)
{
    if (!EventTag.IsValid())
    {
        return;
    }
    OnMissionEventReceived.Broadcast(EventTag, InstigatorActor);
    
    if (bIsTransitioning)
    {
        return;
    }
    
    if (CurrentState != EMissionState::Active ||
        !CurrentMission ||
        !CurrentMission->Steps.IsValidIndex(CurrentStepIndex))
    {
        return;
    }

    const FMissionStep& CurrentStep =
        CurrentMission->Steps[CurrentStepIndex];

    if (CurrentStep.FailureEventTag.IsValid() &&
        EventTag.MatchesTagExact(CurrentStep.FailureEventTag))
    {
        CurrentState = EMissionState::Failed;

        UE_LOG(LogTemp, Warning, TEXT("미션 실패"));
        return;
    }

    if (!EventTag.MatchesTagExact(CurrentStep.CompletionEventTag))
    {
        return;
    }

    const int32 NextStepIndex = CurrentStepIndex + 1;

    if (CurrentMission->Steps.IsValidIndex(NextStepIndex))
    {
        bIsTransitioning = true;

        OnMissionNextStepQueued.Broadcast(
            CurrentMission->MissionTitle,
            CurrentMission->Steps[NextStepIndex]
        );

        GetWorld()->GetTimerManager().SetTimer(
            StepTransitionTimer,
            this,
            &UKTMissionSubsystem::StartNextStep,
            StepTransitionDelay,
            false
        );

        return;
    }

    CurrentState = EMissionState::Completed;
    OnMissionCompleted.Broadcast(CurrentMission->MissionTitle);
}

void UKTMissionSubsystem::RegisterMissionTarget(
    FGameplayTag TargetId,
    AActor* TargetActor)
{
    if (!TargetId.IsValid() || !IsValid(TargetActor))
    {
        return;
    }

    RegisteredTargets.Add(TargetId, TargetActor);
    
    if (CurrentState == EMissionState::Active &&
    CurrentMission &&
    CurrentMission->Steps.IsValidIndex(CurrentStepIndex))
    {
        const FMissionStep& CurrentStep =
            CurrentMission->Steps[CurrentStepIndex];

        if (CurrentStep.TargetId.MatchesTagExact(TargetId))
        {
            BroadcastCurrentStep();
        }
    }
}

void UKTMissionSubsystem::UnregisterMissionTarget(
    FGameplayTag TargetId,
    AActor* TargetActor)
{
    if (!TargetId.IsValid() || !IsValid(TargetActor))
    {
        return;
    }

    if (TObjectPtr<AActor>* FoundTarget =
        RegisteredTargets.Find(TargetId))
    {
        if (*FoundTarget == TargetActor)
        {
            RegisteredTargets.Remove(TargetId);
        }
    }
}

AActor* UKTMissionSubsystem::GetCurrentTargetActor() const
{
    if (CurrentState != EMissionState::Active ||
        !CurrentMission ||
        !CurrentMission->Steps.IsValidIndex(CurrentStepIndex))
    {
        return nullptr;
    }

    const FMissionStep& CurrentStep =
        CurrentMission->Steps[CurrentStepIndex];

    if (!CurrentStep.bShowMarker || !CurrentStep.TargetId.IsValid())
    {
        return nullptr;
    }

    const TObjectPtr<AActor>* FoundTarget =
        RegisteredTargets.Find(CurrentStep.TargetId);

    return FoundTarget ? FoundTarget->Get() : nullptr;
}

void UKTMissionSubsystem::BroadcastCurrentStep()
{
    if (!CurrentMission ||
        !CurrentMission->Steps.IsValidIndex(CurrentStepIndex))
    {
        return;
    }

    OnMissionStepChanged.Broadcast(
        CurrentMission->MissionTitle,
        CurrentMission->Steps[CurrentStepIndex]
    );
}

void UKTMissionSubsystem::StartNextStep()
{
    CurrentStepIndex++;
    bIsTransitioning = false;

    BroadcastCurrentStep();
}

void UKTMissionSubsystem::RestoreMissionProgress(UKTMissionDataAsset* MissionToRestore, int32 SavedStepIndex)
{
    if (!IsValid(MissionToRestore) || MissionToRestore->Steps.IsEmpty())
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(StepTransitionTimer);

    CurrentMission = MissionToRestore;
    CurrentStepIndex = FMath::Clamp(SavedStepIndex, 0, CurrentMission->Steps.Num() - 1);

    CurrentState = EMissionState::Active;
    bIsTransitioning = false;

    BroadcastCurrentStep();
}
