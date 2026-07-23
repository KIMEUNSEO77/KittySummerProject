// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/KittyCharacterBase.h"
#include "KittyCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API AKittyCharacterPlayer : public AKittyCharacterBase
{
	GENERATED_BODY()
	
public:
	AKittyCharacterPlayer();
	
	// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
};
