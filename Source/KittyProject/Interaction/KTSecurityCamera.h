// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KTSecurityCamera.generated.h"

UCLASS()
class KITTYPROJECT_API AKTSecurityCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKTSecurityCamera();
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// 전체 CCTV의 루트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<class USceneComponent> SceneRoot;

	// 벽에 고정되는 CCTV 받침대
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<class UStaticMeshComponent> CameraBaseMesh;

	// 카메라 머리의 회전 중심
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<class USceneComponent> CameraPivot;

	// 실제로 회전하는 카메라 머리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<class UStaticMeshComponent> CameraHeadMesh;

	// 카메라에서 나오는 붉은 조명
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<class USpotLightComponent> WarningLight;

	// 왼쪽을 바라보는 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Rotation")
	float LeftYaw = -60.0f;

	// 오른쪽을 바라보는 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Rotation")
	float RightYaw = 60.0f;

	// 초당 회전 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Rotation")
	float RotationSpeed = 35.0f;

	// 한 방향을 바라보며 기다리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Rotation")
	float WaitDuration = 3.0f;

	// 대기가 끝난 뒤 회전을 시작
	void StartRotation();

	// 현재 회전하고 있는지
	bool bIsRotating = false;

	// 다음 목표가 오른쪽인지
	bool bTargetingRight = true;

	FTimerHandle RotationWaitTimer;
};
