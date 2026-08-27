// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTSecurityCamera.h"

#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AKTSecurityCamera::AKTSecurityCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	SetRootComponent(SceneRoot);

	CameraBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBaseMesh"));
	CameraBaseMesh->SetupAttachment(SceneRoot);
	CameraPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPivot"));
	CameraPivot->SetupAttachment(SceneRoot);
	CameraHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraHeadMesh"));
	CameraHeadMesh->SetupAttachment(CameraPivot);

	WarningLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("WarningLight"));
	WarningLight->SetupAttachment(CameraPivot);

	// 붉은 조명
	WarningLight->SetLightColor(FLinearColor::Red);
	WarningLight->SetIntensity(5000.0f);
	WarningLight->SetAttenuationRadius(1500.0f);
	WarningLight->SetInnerConeAngle(10.0f);
	WarningLight->SetOuterConeAngle(25.0f);
}

void AKTSecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsRotating || !IsValid(CameraPivot))
	{
		return;
	}

	const float TargetYaw = bTargetingRight ? RightYaw : LeftYaw;

	FRotator CurrentRotation = CameraPivot->GetRelativeRotation();

	const float NewYaw =
		FMath::FInterpConstantTo(
			CurrentRotation.Yaw,
			TargetYaw,
			DeltaTime,
			RotationSpeed
		);

	CurrentRotation.Yaw = NewYaw;
	CameraPivot->SetRelativeRotation(CurrentRotation);

	if (FMath::IsNearlyEqual(NewYaw, TargetYaw, 0.1f))
	{
		CurrentRotation.Yaw = TargetYaw;
		CameraPivot->SetRelativeRotation(CurrentRotation);

		bIsRotating = false;
		bTargetingRight = !bTargetingRight;

		// 현재 방향에서 3초 대기
		GetWorldTimerManager().SetTimer(
			RotationWaitTimer,
			this,
			&AKTSecurityCamera::StartRotation,
			WaitDuration,
			false
		);
	}
}

void AKTSecurityCamera::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(CameraPivot))
	{
		return;
	}

	// 게임 시작 시 왼쪽을 바라봄
	FRotator InitialRotation = CameraPivot->GetRelativeRotation();

	InitialRotation.Yaw = LeftYaw;

	CameraPivot->SetRelativeRotation(InitialRotation);

	// 왼쪽을 3초 동안 바라본 후 회전 시작
	GetWorldTimerManager().SetTimer(
		RotationWaitTimer,
		this,
		&AKTSecurityCamera::StartRotation,
		WaitDuration,
		false
	);
}

void AKTSecurityCamera::StartRotation()
{
	bIsRotating = true;
}

