// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_SearchLastSeenLocation.h"

#include "AIController.h"
#include "AITypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KittyCharacterNonplayer.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_SearchLastSeenLocation::UBTTask_SearchLastSeenLocation()
{
	NodeName = TEXT("Search Last Seen Location");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_SearchLastSeenLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	if (!AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	const FVector LastSeenLocation = BlackboardComponent->GetValueAsVector(TEXT("LastSeenLocation"));

	if (!FAISystem::IsValidLocation(LastSeenLocation))
	{
		RestoreBlackboardState(OwnerComp);
		return EBTNodeResult::Failed;
	}

	const EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
		LastSeenLocation,
		AcceptanceRadius
	);

	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		RestoreBlackboardState(OwnerComp);
		return EBTNodeResult::Failed;
	}

	bIsMoving = MoveResult == EPathFollowingRequestResult::RequestSuccessful;
	bIsReturningHome = false;
	MoveTargetLocation = LastSeenLocation;
	ElapsedWaitTime = 0.0f;

	return EBTNodeResult::InProgress;
}

void UBTTask_SearchLastSeenLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		FinishSearch(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!AIController->GetPawn())
	{
		FinishSearch(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (bIsMoving)
	{
		const APawn* ControllingPawn = AIController->GetPawn();
		const float ReachRadius = AcceptanceRadius + ControllingPawn->GetSimpleCollisionRadius();
		const bool bReachedMoveTarget = FVector::DistSquared2D(
			ControllingPawn->GetActorLocation(),
			MoveTargetLocation
		) <= FMath::Square(ReachRadius);

		if (bReachedMoveTarget || AIController->GetMoveStatus() != EPathFollowingStatus::Moving)
		{
			AIController->StopMovement();
			bIsMoving = false;

			if (bIsReturningHome)
			{
				FinishSearch(OwnerComp, EBTNodeResult::Succeeded);
			}
		}

		return;
	}

	ElapsedWaitTime += DeltaSeconds;

	if (ElapsedWaitTime >= WaitTime)
	{
		const AKittyCharacterNonplayer* NPC = Cast<AKittyCharacterNonplayer>(AIController->GetPawn());

		if (NPC && !NPC->IsPatrolEnabled())
		{
			if (!ReturnToHome(OwnerComp))
			{
				FinishSearch(OwnerComp, EBTNodeResult::Failed);
			}
		}
		else
		{
			FinishSearch(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

EBTNodeResult::Type UBTTask_SearchLastSeenLocation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->StopMovement();
	}

	RestoreBlackboardState(OwnerComp);
	bIsMoving = false;
	bIsReturningHome = false;
	MoveTargetLocation = FAISystem::InvalidLocation;
	ElapsedWaitTime = 0.0f;

	return Super::AbortTask(OwnerComp, NodeMemory);
}

bool UBTTask_SearchLastSeenLocation::ReturnToHome(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComponent)
	{
		return false;
	}

	const FVector HomePos = BlackboardComponent->GetValueAsVector(TEXT("HomePos"));

	if (!FAISystem::IsValidLocation(HomePos))
	{
		return false;
	}

	const EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(
		HomePos,
		AcceptanceRadius
	);

	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		return false;
	}

	bIsReturningHome = true;
	bIsMoving = MoveResult == EPathFollowingRequestResult::RequestSuccessful;
	MoveTargetLocation = HomePos;

	if (!bIsMoving)
	{
		FinishSearch(OwnerComp, EBTNodeResult::Succeeded);
	}

	return true;
}

void UBTTask_SearchLastSeenLocation::FinishSearch(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result)
{
	bIsMoving = false;
	bIsReturningHome = false;
	MoveTargetLocation = FAISystem::InvalidLocation;
	ElapsedWaitTime = 0.0f;

	FinishLatentTask(OwnerComp, Result);
	RestoreBlackboardState(OwnerComp);
}

void UBTTask_SearchLastSeenLocation::RestoreBlackboardState(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!BlackboardComponent || !AIController)
	{
		return;
	}

	BlackboardComponent->SetValueAsBool(TEXT("IsSearching"), false);

	if (const AKittyCharacterNonplayer* NPC = Cast<AKittyCharacterNonplayer>(AIController->GetPawn()))
	{
		BlackboardComponent->SetValueAsBool(TEXT("IsPatrolEnabled"), NPC->IsPatrolEnabled());
	}
}
