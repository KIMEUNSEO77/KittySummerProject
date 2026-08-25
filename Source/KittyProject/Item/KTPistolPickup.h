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
	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

	// 총구가 바라보는 월드 방향
	FVector GetMuzzleForwardVector() const;
	
	// 이펙트 재생
	UFUNCTION(BlueprintCallable)
	void PlayMuzzleFlash();
	
	// 권총 sound 재생
	UFUNCTION(BlueprintCallable)
	void PlayFireSound();
	
	// 총을 주울 때 플레이어가 위치할 Transform
	FTransform GetPickupStandTransform() const;
	
	// 획득 애니메이션의 Notify 시점에 실제 총 획득 처리
	bool CompletePickup(class AKittyCharacterPlayer* Player);
	
protected:
	// 총구 위치와 방향을 나타내는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol|Components")
	TObjectPtr<class USceneComponent> MuzzlePoint;
	
	// 발사할 때 MuzzlePoint에 생성할 Niagara 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pistol|Effects")
	TObjectPtr<class UNiagaraSystem> MuzzleFlashEffect;
	
	// 권총 발사음
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pistol|Sound")
	TObjectPtr<USoundBase> FireSound;

	// 발사음 크기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pistol|Sound", meta = (ClampMin = "0.0"))
	float FireSoundVolume = 1.0f;

	// 발사음 높낮이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pistol|Sound", meta = (ClampMin = "0.1"))
	float FireSoundPitch = 1.0f;
	
	// 총을 주울 때 플레이어가 서 있을 위치와 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol|Interaction")
	TObjectPtr<class USceneComponent> PickupStandPoint;
};
