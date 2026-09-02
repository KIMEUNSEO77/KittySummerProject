// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "KittyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBlackboardData;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API AKittyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AKittyAIController();
	void RunAI();
	void StopAI();
	
	UFUNCTION(BlueprintCallable, Category = "AI|Detection")
	void SetForcedTarget(AActor* NewTarget);
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	void ApplySightConfig();
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
private:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;
	
	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;
	
	UPROPERTY(	VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Sight", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float SightRadius = 1200.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "AI|Sight",meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float LoseSightRadius = 1500.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "AI|Sight",meta = (AllowPrivateAccess = "true",ClampMin = "0.0",ClampMax = "180.0"))
	float PeripheralVisionHalfAngle = 65.0f;
};
