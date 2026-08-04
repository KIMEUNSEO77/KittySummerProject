#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "KTMissionTargetComponent.generated.h"

UCLASS(ClassGroup = (Mission), meta = (BlueprintSpawnableComponent))
class KITTYPROJECT_API UKTMissionTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKTMissionTargetComponent();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;

	// 예: Mission.Target.Pistol
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FGameplayTag TargetId;
};