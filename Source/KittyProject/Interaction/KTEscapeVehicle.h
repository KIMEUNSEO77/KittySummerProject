// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/KTInteractableInterface.h"
#include "GameplayTagContainer.h"
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
	
	// 시네마틱 재생이 끝났을 때 호출
	UFUNCTION()
	void HandleSequenceFinished();

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
	
	// F키를 눌렀을 때 재생할 엔딩 시네마틱
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Vehicle|Cinematic")
	TObjectPtr<class ALevelSequenceActor> EscapeSequenceActor;
	
	// 마지막 미션 Step을 완료할 이벤트 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Mission")
	FGameplayTag EscapeCompletedEventTag;

	// 화면이 검게 변하는 데 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle|Ending")
	float FadeDuration = 2.0f;

	// 엔딩을 시작한 플레이어 컨트롤러
	UPROPERTY()
	TObjectPtr<class APlayerController> EndingPlayerController;
};
