// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdateAttackRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/KittyCharacterNonplayer.h"
#include "AIController.h"

UBTService_UpdateAttackRange::UBTService_UpdateAttackRange()
{
	NodeName = TEXT("Update Attack Range");
	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_UpdateAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (!AIController||!Blackboard)
	{
		return;
	}
	
	AKittyCharacterNonplayer* Guard = Cast<AKittyCharacterNonplayer>(AIController->GetPawn());
	
	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target")));
	
	bool bIsInAttackRange = false;
	
	if (IsValid(Target)&&IsValid(Guard))
	{
		const float AttackRange = Guard->GetAIAttackRange();
		
		const float DistanceSquared = FVector::DistSquared2D(Guard->GetActorLocation(), Target->GetActorLocation());
		
		bIsInAttackRange = AttackRange > 0.0f && DistanceSquared <= FMath::Square(AttackRange);
	}
	
	Blackboard->SetValueAsBool(TEXT("IsInAttackRange"), bIsInAttackRange);
}
