// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetNextPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UBTTask_SetNextPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SetNextPatrolPoint();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
