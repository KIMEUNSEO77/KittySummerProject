// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UBTService_UpdateAttackRange : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateAttackRange();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
