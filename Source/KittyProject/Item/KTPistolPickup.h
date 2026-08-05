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
	
	// 총구의 월드 위치
	FVector GetMuzzleLocation() const;

	// 총구가 바라보는 월드 방향
	FVector GetMuzzleForwardVector() const;
	
protected:
	// 총구 위치와 방향을 나타내는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol|Components")
	TObjectPtr<class USceneComponent> MuzzlePoint;
};
