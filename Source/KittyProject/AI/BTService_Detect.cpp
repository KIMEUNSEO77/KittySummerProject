// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Detect.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Interface/KittyCharacterAIInterface.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!BlackboardComponent || !AIController)
	{
		return;
	}

	APawn* ControllingPawn = AIController->GetPawn();
	if (!ControllingPawn)
	{
		BlackboardComponent->ClearValue(TEXT("Target"));
		return;
	}

	IKittyCharacterAIInterface* AIPawn = Cast<IKittyCharacterAIInterface>(ControllingPawn);
	if (!AIPawn)
	{
		BlackboardComponent->ClearValue(TEXT("Target"));
		return;
	}

	UWorld* World = ControllingPawn->GetWorld();
	if (!World)
	{
		BlackboardComponent->ClearValue(TEXT("Target"));
		return;
	}

	const FVector Center = ControllingPawn->GetActorLocation();
	const float DetectRadius = AIPawn->GetAIDetectRange();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);
	World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
		if (Pawn && Pawn != ControllingPawn && Pawn->IsPlayerControlled())
		{
			BlackboardComponent->SetValueAsObject(TEXT("Target"), Pawn);
			DrawDebugCircle(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
			DrawDebugPoint(World, Pawn->GetActorLocation(), 10.0f, FColor::Green, false, 0.2f);
			DrawDebugLine(World, Center, Pawn->GetActorLocation(), FColor::Green, false, 0.27f);
			return;
		}
	}

	BlackboardComponent->ClearValue(TEXT("Target"));
	DrawDebugCircle(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}
