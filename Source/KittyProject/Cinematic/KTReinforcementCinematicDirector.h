#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "KTReinforcementCinematicDirector.generated.h"

class ACineCameraActor;
class APlayerController;
class UAnimSequence;
class UKTMissionSubsystem;

UCLASS(Blueprintable)
class KITTYPROJECT_API AKTReinforcementCinematicDirector : public AActor
{
	GENERATED_BODY()

public:
	AKTReinforcementCinematicDirector();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleMissionEvent(FGameplayTag EventTag, AActor* InstigatorActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Trigger")
	FGameplayTag TriggerTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Trigger")
	FGameplayTag CompletionTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Guards", meta = (ClampMin = "10", ClampMax = "15"))
	int32 GuardCount = 12;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Guards")
	TSubclassOf<AActor> CinematicGuardClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Guards")
	TObjectPtr<UAnimSequence> RunningAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Timing", meta = (ClampMin = "3.0"))
	float CinematicDuration = 6.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Route")
	FVector RunStart = FVector(-11600.0f, 5050.0f, 380.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Cinematic|Route")
	FVector RunEnd = FVector(-9970.0f, 4700.0f, 380.0f);

private:
	struct FCinematicGuardState
	{
		TWeakObjectPtr<AActor> Actor;
		FVector Start;
		FVector End;
		float StartDelay = 0.0f;
	};

	void StartCinematic();
	void UpdateGuards();
	void UpdateCamera();
	void BeginFinishTransition();
	void FinishCinematic();
	void CleanupCinematicActors();
	void SetPlayerCinematicMode(bool bEnabled);
	FRotator MakeLookAtRotation(const FVector& From, const FVector& To) const;

	UPROPERTY(Transient)
	TObjectPtr<ACineCameraActor> CinematicCamera;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(Transient)
	TObjectPtr<AActor> PreviousViewTarget;

	UPROPERTY(Transient)
	TObjectPtr<AActor> TriggerInstigator;

	TArray<FCinematicGuardState> GuardStates;
	FTimerHandle FinishTransitionTimer;
	float ElapsedTime = 0.0f;
	bool bHasPlayed = false;
	bool bIsPlaying = false;
	bool bIsFinishing = false;
};
