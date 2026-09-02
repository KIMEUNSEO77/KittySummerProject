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

protected:
	virtual void BeginPlay() override;

	// 반복 재생할 주민 애니메이션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Ambient")
	TObjectPtr<class UAnimSequence> AmbientAnimation;

	// 애니메이션을 반복할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Civilian|Ambient")
	bool bLoopAmbientAnimation = true;
};
