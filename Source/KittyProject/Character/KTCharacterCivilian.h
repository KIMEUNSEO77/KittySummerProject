// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/KittyCharacterBase.h"
#include "KTCharacterCivilian.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API AKTCharacterCivilian : public AKittyCharacterBase
{
	GENERATED_BODY()
	
public:
	AKTCharacterCivilian();
	
	UFUNCTION(BlueprintCallable, Category = "Civilian|Reaction")
	void TriggerStartledReaction();

protected:
	virtual void BeginPlay() override;

	// 반복 재생할 주민 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Ambient")
	TObjectPtr<class UAnimSequence> AmbientAnimation;

	// 애니메이션을 반복할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Ambient")
	bool bLoopAmbientAnimation = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Reaction")
	TObjectPtr<class UAnimSequence> StartledAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Reaction")
	TArray<TObjectPtr<class USoundBase>> AlertSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Reaction")
	float DetectionRange = 1200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Civilian|Reaction")
	bool bHasReacted = false;
	
	// 일정 간격으로 플레이어 감지
	void CheckPlayerDetection();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Detection")
	float DetectionInterval = 0.2f;

	FTimerHandle DetectionTimer;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Civilian|Ambient")
	TObjectPtr<class AAmbientSound> AmbientConversationSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Detection")
	float DetectionHalfAngle = 65.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Detection")
	float DetectionYawOffset = 0.0f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Civilian|Guard")
	TObjectPtr<class ATargetPoint> GuardSpawnPoint;
	
	// 발각됐을 때 생성할 경비원
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Guard")
	TSubclassOf<class AKittyCharacterNonplayer> GuardClass;

	// 경비원이 등장하기까지의 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Guard")
	float GuardSpawnDelay = 1.0f;

	void SpawnAlertGuard();

	FTimerHandle GuardSpawnTimer;
};
