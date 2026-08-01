// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KTInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UKTInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 * 총, 아이템, 문 등 상호작용 가능한 클래스가 구현할 인터페이스
 */
class KITTYPROJECT_API IKTInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 플레이어가 상호작용 키를 눌렀을 때 호출, 상호작용을 시도한 Actor(플레이어)가 전달됨
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);
	
	// 화면에 표시할 상호작용 안내 문구를 반환 ex) "[F] 총 획득하기" 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText() const;
};
