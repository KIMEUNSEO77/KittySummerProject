// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KittyCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

UCLASS()
class KITTYPROJECT_API AKittyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKittyCharacterBase();
	
protected:
	virtual void SetCharacterControlData(const class UKittyCharacterControlData* CharacterControlData);
	
	UPROPERTY(EditAnywhere, Category= CharacterControl, meta=(AllowPrivateAccess="true"))
	TMap<ECharacterControlType, class UKittyCharacterControlData*> CharacterControlManager;
};
