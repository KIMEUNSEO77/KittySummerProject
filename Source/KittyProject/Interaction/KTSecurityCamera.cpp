// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTSecurityCamera.h"

#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

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
	WarningLight->SetMobility(EComponentMobility::Movable);
	WarningLight->SetVisibility(true);
	WarningLight->SetLightColor(FLinearColor::Red);
	WarningLight->SetIntensity(100000.0f);
	WarningLight->SetAttenuationRadius(2000.0f);
	WarningLight->SetInnerConeAngle(10.0f);
	WarningLight->SetOuterConeAngle(25.0f);
	WarningLight->SetVolumetricScatteringIntensity(5.0f);
	WarningLight->SetCastShadows(false);
	
	AlarmAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("AlarmAudio"));
	AlarmAudio->SetupAttachment(CameraPivot);

	// 게임 시작과 동시에 재생되지 않도록 설정
	AlarmAudio->SetAutoActivate(false);
}

void AKTSecurityCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 카메라 회전 처리
	if (bIsRotating && IsValid(CameraPivot))
	{
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

			GetWorldTimerManager().SetTimer(
				RotationWaitTimer,
				this,
				&AKTSecurityCamera::StartRotation,
				WaitDuration,
				false
			);
		}
	}

	// 회전 여부와 상관없이 항상 감지
	UpdatePlayerDetection(DeltaTime);
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

void AKTSecurityCamera::UpdatePlayerDetection(float DeltaTime)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	FVector TraceStart;
	FVector TraceEnd;

	const bool bCanSeePlayer =
		CanSeePlayer(
			PlayerCharacter,
			TraceStart,
			TraceEnd
		);

	if (bCanSeePlayer)
	{
		// DetectionDuration초 동안 노출되면 1.0 도달
		DetectionProgress += DeltaTime / FMath::Max(DetectionDuration, 0.01f);
	}
	else
	{
		DetectionProgress -= DeltaTime * DetectionDecreaseSpeed;
	}

	DetectionProgress =
		FMath::Clamp(
			DetectionProgress,
			0.0f,
			1.0f
		);

	if (bShowDetectionDebug)
	{
		// 감지되면 빨강, 엄폐되면 초록
		DrawDebugLine(
			GetWorld(),
			TraceStart,
			TraceEnd,
			bCanSeePlayer
				? FColor::Red
				: FColor::Green,
			false,
			0.0f,
			0,
			2.0f
		);

		if (GEngine)
		{
			const FString DebugText =
				FString::Printf(
					TEXT("CCTV 감지 게이지: %.0f%%"),
					DetectionProgress * 100.0f
				);

			// 같은 메시지를 갱신하여 화면 도배 방지
			GEngine->AddOnScreenDebugMessage(
				8721,
				0.0f,
				bCanSeePlayer
					? FColor::Red
					: FColor::Green,
				DebugText
			);
		}
	}

	if (DetectionProgress >= 1.0f && !bAlarmTriggered)
	{
		TriggerAlarm();
	}
}

bool AKTSecurityCamera::CanSeePlayer(class ACharacter* PlayerCharacter, FVector& OutTraceStart,
                                     FVector& OutTraceEnd) const
{
	if (!IsValid(PlayerCharacter) ||
		!IsValid(WarningLight))
	{
		return false;
	}

	OutTraceStart =
		WarningLight->GetComponentLocation();

	// 플레이어 몸통을 향하도록 높이 보정
	OutTraceEnd =
		PlayerCharacter->GetActorLocation() +
		FVector(0.0f, 0.0f, 50.0f);

	const FVector ToPlayer = OutTraceEnd - OutTraceStart;

	const float DistanceToPlayer = ToPlayer.Size();

	// 빨간 조명의 길이를 실제 감지 거리로 사용
	const float DetectionDistance = WarningLight->AttenuationRadius;

	if (DistanceToPlayer > DetectionDistance)
	{
		return false;
	}

	const FVector DirectionToPlayer = ToPlayer.GetSafeNormal();

	const FVector CameraForward = WarningLight->GetForwardVector();

	// Spotlight 바깥 각도를 실제 시야각으로 사용
	const float MinimumViewDot =
		FMath::Cos(
			FMath::DegreesToRadians(
				WarningLight->OuterConeAngle
			)
		);

	const float ViewDot =
		FVector::DotProduct(
			CameraForward,
			DirectionToPlayer
		);

	if (ViewDot < MinimumViewDot)
	{
		return false;
	}

	// 카메라 자신은 Line Trace에서 무시
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;

	const bool bHit =
		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			OutTraceStart,
			OutTraceEnd,
			ECC_Visibility,
			QueryParams
		);

	return bHit && HitResult.GetActor() == PlayerCharacter;
}

void AKTSecurityCamera::TriggerAlarm()
{
	bAlarmTriggered = true;
	
	// 카메라 회전 정지
	bIsRotating = false;
	GetWorldTimerManager().ClearTimer(RotationWaitTimer);

	// 경보음 재생
	if (IsValid(AlarmAudio) && !AlarmAudio->IsPlaying())
	{
		AlarmAudio->Play();
	}
	
	// 블루프린트에서 경보 UI 실행
	OnAlarmTriggered();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Red,
			TEXT("경보 발생! CCTV에 발각되었습니다.")
		);
	}
}

