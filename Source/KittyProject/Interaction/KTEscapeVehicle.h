// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/KTInteractableInterface.h"
#include "KTEscapeVehicle.generated.h"

UCLASS()
class KITTYPROJECT_API AKTEscapeVehicle : public AActor,  public IKTInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKTEscapeVehicle();
	
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionText_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Components")
	TObjectPtr<class USceneComponent> SceneRoot;

	// 자동차 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Components")
	TObjectPtr<class UStaticMeshComponent> VehicleMesh;

	// 자동차 탑승 상호작용 영역
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Components")
	TObjectPtr<class UBoxComponent> InteractionCollision;

	// 시네마틱에서 플레이어를 배치할 운전석 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Components")
	TObjectPtr<class USceneComponent> DriverSeatPoint;

	// 엔딩이 중복 실행되지 않도록 확인
	bool bHasStartedEnding = false;
};
