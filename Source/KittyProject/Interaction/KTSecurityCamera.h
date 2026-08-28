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

// Rotation Section
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
	
	// Detection Section
protected:
	// 플레이어가 완전히 감지되는 데 필요한 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Detection")
	float DetectionDuration = 2.0f;

	// 플레이어를 놓쳤을 때 감지 게이지가 감소하는 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Detection")
	float DetectionDecreaseSpeed = 0.75f;

	// 현재 감지 게이지: 0.0 ~ 1.0
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Detection")
	float DetectionProgress = 0.0f;

	// 디버그 선과 메시지를 표시할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera|Debug")
	bool bShowDetectionDebug = true;

	// 이미 완전히 발각됐는지
	bool bAlarmTriggered = false;

	// 거리·시야각·엄폐물을 확인하여 플레이어 감지
	void UpdatePlayerDetection(float DeltaTime);

	// 플레이어가 실제로 보이는지 확인
	bool CanSeePlayer(
		class ACharacter* PlayerCharacter,
		FVector& OutTraceStart,
		FVector& OutTraceEnd
	) const;

	// 감지 게이지가 최대가 됐을 때 호출
	void TriggerAlarm();
	
	// 발각 시 재생하는 경보음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
	TObjectPtr<class UAudioComponent> AlarmAudio;
	
	// CCTV 경보 발생 시 블루프린트에서 UI 연출을 실행
	UFUNCTION(BlueprintImplementableEvent, Category = "Camera|Alarm")
	void OnAlarmTriggered();
};
