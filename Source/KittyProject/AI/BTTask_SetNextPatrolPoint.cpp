// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_SetNextPatrolPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KittyCharacterNonplayer.h"
#include "Engine/TargetPoint.h"

UBTTask_SetNextPatrolPoint::UBTTask_SetNextPatrolPoint()
{
	NodeName = "Set Next Patrol Point";
}

EBTNodeResult::Type UBTTask_SetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	AKittyCharacterNonplayer* NPC = Controller ? Cast<AKittyCharacterNonplayer>(Controller->GetPawn()) : nullptr;

	if (!NPC || !Blackboard || !NPC->IsPatrolEnabled())
	{
		return EBTNodeResult::Failed;
	}

	const TArray<TObjectPtr<ATargetPoint>>& PatrolPoints = NPC->GetPatrolPoints();

	if (PatrolPoints.IsEmpty())
	{
		return EBTNodeResult::Failed;
	}

	int32 Index =
		Blackboard->GetValueAsInt(TEXT("PatrolIndex"))
		% PatrolPoints.Num();

	for (int32 Attempt = 0; Attempt < PatrolPoints.Num(); ++Attempt)
	{
		ATargetPoint* Point = PatrolPoints[Index].Get();
		Index = (Index + 1) % PatrolPoints.Num();

		if (!IsValid(Point))
		{
			continue;
		}

		Blackboard->SetValueAsVector(TEXT("PatrolPos"),Point->GetActorLocation()
		);

		Blackboard->SetValueAsInt(TEXT("PatrolIndex"), Index);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
