// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/KTInteractableInterface.h"
#include "KTKeycardDoor.generated.h"

UCLASS()
class KITTYPROJECT_API AKTKeycardDoor : public AActor, public IKTInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKTKeycardDoor();

protected:
	// 문틀처럼 움직이지 않는 부분
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<USceneComponent> SceneRoot;
	
	// 플레이어의 상호작용 탐색에 감지될 전용 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<class USphereComponent> InteractionCollision;

	// 실제로 열릴 문 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	
	// F키를 눌렀을 때 실행되는 상호작용
	virtual void Interact_Implementation(AActor* Interactor) override;

	// 플레이어가 문을 바라볼 때 표시할 안내 문구
	virtual FText GetInteractionText_Implementation() const override;
};
