// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SearchLastSeenLocation.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UBTTask_SearchLastSeenLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SearchLastSeenLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	bool ReturnToHome(UBehaviorTreeComponent& OwnerComp);
	void FinishSearch(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result);
	void RestoreBlackboardState(UBehaviorTreeComponent& OwnerComp);

private:
	UPROPERTY(EditAnywhere, Category = AI, meta = (ClampMin = "0.0"))
	float AcceptanceRadius = 80.0f;

	UPROPERTY(EditAnywhere, Category = AI, meta = (ClampMin = "0.0"))
	float WaitTime = 2.0f;

	float ElapsedWaitTime = 0.0f;
	FVector MoveTargetLocation = FAISystem::InvalidLocation;
	bool bIsMoving = false;
	bool bIsReturningHome = false;
};
