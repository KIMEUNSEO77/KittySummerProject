#include "Mission/KTMissionTargetComponent.h"

#include "Mission/KTMissionSubsystem.h"

UKTMissionTargetComponent::UKTMissionTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKTMissionTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UKTMissionSubsystem* MissionSubsystem =
		UKTMissionSubsystem::Get(this))
	{
		MissionSubsystem->RegisterMissionTarget(
			TargetId,
			GetOwner()
		);
	}
}

void UKTMissionTargetComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	if (UKTMissionSubsystem* MissionSubsystem =
		UKTMissionSubsystem::Get(this))
	{
		MissionSubsystem->UnregisterMissionTarget(
			TargetId,
			GetOwner()
		);
	}

	Super::EndPlay(EndPlayReason);
}