// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/KTInteractableInterface.h"
#include "KTCommunicationTerminal.generated.h"

UCLASS()
class KITTYPROJECT_API AKTCommunicationTerminal : public AActor, public IKTInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKTCommunicationTerminal();
	
protected:
	// 액터의 기준이 되는 루트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terminal|Components")
	TObjectPtr<class USceneComponent> SceneRoot;

	// 월드에 표시되는 통신 단말기 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terminal|Components")
	TObjectPtr<class UStaticMeshComponent> TerminalMesh;

	// 플레이어에게 감지될 상호작용 영역
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terminal|Components")
	TObjectPtr<class UBoxComponent> InteractionCollision;

	// F키를 눌렀을 때 실행
	virtual void Interact_Implementation(AActor* Interactor) override;

	// 플레이어가 단말기를 바라볼 때 반환할 안내 문구
	virtual FText GetInteractionText_Implementation() const override;
};
