// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/KittyAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "GameFramework/Pawn.h"

#include "Character/KittyCharacterNonplayer.h"

AKittyAIController::AKittyAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Game/AI/BB_KittyCharacter.BB_KittyCharacter"));
	if (BBAssetRef.Succeeded())
	{
		BBAsset = BBAssetRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Game/AI/BT_KittyCharacter.BT_KittyCharacter"));
	if (BTAssetRef.Succeeded())
	{
		BTAsset = BTAssetRef.Object;
	}
	
	// AI Perception 컴포넌트 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	// 시각 설정 객체 생성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	// 초기값 설정
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionHalfAngle;

	// 현재는 팀 시스템이 없으므로 모두 감지하도록 설정
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// 컴포넌트에 시각 설정 등록
	AIPerceptionComponent->ConfigureSense(*SightConfig);

	// 대표 감각을 Sight로 지정
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// 이 컴포넌트를 AIController의 Perception으로 등록
	SetPerceptionComponent(*AIPerceptionComponent);
}

void AKittyAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = nullptr;

	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		APawn* ControlledPawn = GetPawn();

		if (!ControlledPawn)
		{
			return;
		}

		BlackboardPtr->SetValueAsVector(TEXT("HomePos"), ControlledPawn->GetActorLocation());

		BlackboardPtr->SetValueAsInt(TEXT("PatrolIndex"), 0);

		if (const AKittyCharacterNonplayer* NPC = Cast<AKittyCharacterNonplayer>(ControlledPawn))
		{
			BlackboardPtr->SetValueAsBool(TEXT("IsPatrolEnabled"),NPC->IsPatrolEnabled());
		}

		const bool bRunResult = RunBehaviorTree(BTAsset);
		ensure(bRunResult);
	}
}

void AKittyAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AKittyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	RunAI();
	
	ApplySightConfig();
	
	if (AIPerceptionComponent)
	{
		// 중복 등록 방지
		AIPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(
			this,
			&AKittyAIController::OnTargetPerceptionUpdated
		);

		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
			this,
			&AKittyAIController::OnTargetPerceptionUpdated
		);

		AIPerceptionComponent->RequestStimuliListenerUpdate();
	}
}

void AKittyAIController::ApplySightConfig()
{
	if (!AIPerceptionComponent || !SightConfig)
	{
		return;
	}

	SightConfig->SightRadius = FMath::Max(0.0f, SightRadius);

	// LoseSightRadius가 SightRadius보다 작지 않게 방어
	SightConfig->LoseSightRadius = FMath::Max(SightRadius, LoseSightRadius);

	SightConfig->PeripheralVisionAngleDegrees = FMath::Clamp(PeripheralVisionHalfAngle, 0.0f, 180.0f);

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->RequestStimuliListenerUpdate();
}

void AKittyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Actor))
	{
		return;
	}
	
	APawn* SensedPawn = Cast<APawn>(Actor);

	if (!SensedPawn || !SensedPawn->IsPlayerControlled())
	{
		return;
	}
	
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return;
	}

	static const FName TargetKey(TEXT("Target"));

	if (Stimulus.WasSuccessfullySensed())
	{
		// 플레이어를 봤다.
		BlackboardComponent->SetValueAsObject(TargetKey, Actor);
	}
	else
	{
		// 현재 추적 중인 플레이어를 놓친 경우에만 제거
		UObject* CurrentTarget = BlackboardComponent->GetValueAsObject(TargetKey);

		if (CurrentTarget == Actor)
		{
			BlackboardComponent->ClearValue(TargetKey);
		}
	}
}
