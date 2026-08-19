// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GuardAttack.h"

#include "AIController.h"
#include "AI/Attack/KTGuardAttackComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KittyCharacterNonplayer.h"

UBTTask_GuardAttack::UBTTask_GuardAttack()
{
	NodeName = TEXT("Guard Attack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GuardAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	
	if (!Blackboard||!AIController)
	{
		return EBTNodeResult::Failed;
	}
	
	AKittyCharacterNonplayer* Guard = Cast<AKittyCharacterNonplayer>(AIController->GetPawn());
	
	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target")));
	
	if (!IsValid(Target) || !IsValid(Guard))
	{
		return EBTNodeResult::Failed;
	}
	
	UKTGuardAttackComponent* AttackComponent = Guard->GetAttackComponent();
	
	if (!IsValid(AttackComponent))
	{
		return EBTNodeResult::Failed;
	}
	
	AIController->StopMovement();
	
	if (!AttackComponent->StartAttack(Target))
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::InProgress;
}

void UBTTask_GuardAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	AKittyCharacterNonplayer* Guard = AIController ? Cast<AKittyCharacterNonplayer>(AIController->GetPawn()) : nullptr;
	
	if (!IsValid(Guard))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	UKTGuardAttackComponent* AttackComponent = Guard->GetAttackComponent();
	
	if (!IsValid(AttackComponent))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!AttackComponent->IsAttacking())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}
