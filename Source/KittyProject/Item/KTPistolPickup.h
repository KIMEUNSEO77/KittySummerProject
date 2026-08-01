// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/KTItemPickupBase.h"
#include "KTPistolPickup.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API AKTPistolPickup : public AKTItemPickupBase
{
	GENERATED_BODY()
	
public:
	AKTPistolPickup();

	// 권총 전용 상호작용 동작
	virtual void Interact_Implementation(AActor* Interactor) override;
};
