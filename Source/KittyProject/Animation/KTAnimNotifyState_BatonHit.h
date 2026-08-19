// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "KTAnimNotifyState_BatonHit.generated.h"

class UKTBatonAttackComponent;

/**
 * 
 */
UCLASS(meta = (DisplayName="Baton Hit Window"))
class KITTYPROJECT_API UKTAnimNotifyState_BatonHit : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
private:
	UKTBatonAttackComponent* GetAttackComponent(USkeletalMeshComponent* MeshComp) const;
};
