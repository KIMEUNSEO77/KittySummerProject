// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GuardAttack.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UBTTask_GuardAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_GuardAttack();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
