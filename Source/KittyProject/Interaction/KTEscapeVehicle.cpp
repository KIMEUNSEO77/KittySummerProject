// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTEscapeVehicle.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Mission/KTMissionSubsystem.h"
#include "Player/KittyPlayerController.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

// Sets default values
AKTEscapeVehicle::AKTEscapeVehicle()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	VehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMesh"));
	VehicleMesh->SetupAttachment(SceneRoot);

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(SceneRoot);

	InteractionCollision->SetBoxExtent(FVector(100.0f, 150.0f, 100.0f));

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);

	DriverSeatPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DriverSeatPoint"));
	DriverSeatPoint->SetupAttachment(VehicleMesh);
	
	VehicleEntryPoint = CreateDefaultSubobject<USceneComponent>(TEXT("VehicleEntryPoint"));
	VehicleEntryPoint->SetupAttachment(VehicleMesh);
}

void AKTEscapeVehicle::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor) || bHasStartedEnding)
	{
		return;
	}

	if (!IsValid(EscapeSequenceActor))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.0f,
				FColor::Red,
				TEXT("EscapeSequenceActor가 설정되지 않았습니다.")
			);
		}

		return;
	}

	ULevelSequencePlayer* SequencePlayer = EscapeSequenceActor->GetSequencePlayer();

	if (!IsValid(SequencePlayer))
	{
		return;
	}

	APawn* PlayerPawn = Cast<APawn>(Interactor);

	if (!IsValid(PlayerPawn))
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}
	
	// 시네마틱 종료 후 페이드아웃에 사용할 컨트롤러 저장
	EndingPlayerController = PlayerController;

	bHasStartedEnding = true;

	// 다시 상호작용하지 못하도록 Collision을 끔
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ACharacter* PlayerCharacter = Cast<ACharacter>(PlayerPawn);

	if (!IsValid(PlayerCharacter) ||
		!IsValid(EnterVehicleMontage) ||
		!IsValid(VehicleEntryPoint))
	{
		return;
	}

	USkeletalMeshComponent* PlayerMesh = PlayerCharacter->GetMesh();

	if (!IsValid(PlayerMesh))
	{
		return;
	}

	UAnimInstance* AnimInstance = PlayerMesh->GetAnimInstance();

	if (!IsValid(AnimInstance))
	{
		return;
	}

	// 엔딩 처리를 시작한 플레이어 컨트롤러 저장
	EndingPlayerController = PlayerController;

	bHasStartedEnding = true;

	// 다시 상호작용하지 못하도록 충돌 끄기
	InteractionCollision->SetCollisionEnabled(
		ECollisionEnabled::NoCollision);

	// 차량과 캐릭터가 충돌하지 않도록 설정
	PlayerPawn->SetActorEnableCollision(false);

	// 탑승 애니메이션 시작 위치로 이동
	PlayerPawn->SetActorLocationAndRotation(
		VehicleEntryPoint->GetComponentLocation(),
		VehicleEntryPoint->GetComponentRotation()
	);

	// UI 숨기기
	if (AKittyPlayerController* KittyPlayerController =
		Cast<AKittyPlayerController>(PlayerController))
	{
		KittyPlayerController->SetGameplayUIVisible(false);
	}

	// 이동 및 카메라 조작 잠금
	PlayerController->SetCinematicMode(
		true,
		false,
		true,
		true,
		true
	);

	AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AKTEscapeVehicle::HandleEnterVehicleNotify);
	
	// 우선 탑승 몽타주만 재생
	const float PlayedDuration = AnimInstance->Montage_Play(EnterVehicleMontage);

	if (PlayedDuration <= 0.0f)
	{
		// 몽타주 재생 실패
		bHasStartedEnding = false;

		InteractionCollision->SetCollisionEnabled(
			ECollisionEnabled::QueryOnly);

		PlayerPawn->SetActorEnableCollision(true);

		PlayerController->SetCinematicMode(
			false,
			false,
			true,
			true,
			true);

		if (AKittyPlayerController* KittyPlayerController =
			Cast<AKittyPlayerController>(PlayerController))
		{
			KittyPlayerController->SetGameplayUIVisible(true);
		}
	}
}

FText AKTEscapeVehicle::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("차량에 탑승하기"));
}

void AKTEscapeVehicle::HandleSequenceFinished()
{
	if (EscapeCompletedEventTag.IsValid())
	{
		if (UKTMissionSubsystem* MissionSubsystem =
			UKTMissionSubsystem::Get(this))
		{
			MissionSubsystem->BroadcastMissionEvent(
				EscapeCompletedEventTag,
				this
			);
		}
	}

	if (!IsValid(EndingPlayerController) ||
		!IsValid(EndingPlayerController->PlayerCameraManager))
	{
		return;
	}

	// Sequencer에서 완성된 검은 화면을 그대로 유지
	EndingPlayerController->PlayerCameraManager
		->SetManualCameraFade(
			1.0f,
			FLinearColor::Black,
			false
		);
	
	// 1초 뒤 게임 종료
	FTimerHandle QuitTimerHandle;

	GetWorldTimerManager().SetTimer(QuitTimerHandle, this, &AKTEscapeVehicle::QuitGameAfterEnding, 1.0f, false);
}

void AKTEscapeVehicle::HandleEnterVehicleNotify(FName NotifyName,
	const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName != TEXT("EnterVehicle"))
	{
		return;
	}

	if (!IsValid(EndingPlayerController) ||
		!IsValid(EscapeSequenceActor))
	{
		return;
	}

	ACharacter* PlayerCharacter = Cast<ACharacter>(EndingPlayerController->GetPawn());

	if (!IsValid(PlayerCharacter) ||
		!IsValid(DriverSeatPoint))
	{
		return;
	}

	USkeletalMeshComponent* PlayerMesh = PlayerCharacter->GetMesh();

	if (!IsValid(PlayerMesh))
	{
		return;
	}

	UAnimInstance* AnimInstance = PlayerMesh->GetAnimInstance();

	if (IsValid(AnimInstance))
	{
		// Notify가 두 번 처리되지 않도록 연결 해제
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AKTEscapeVehicle::HandleEnterVehicleNotify);
	}

	// 플레이어를 정확한 운전석 위치에 부착
	PlayerCharacter->AttachToComponent(DriverSeatPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// 몽타주 대신 운전 애니메이션 반복 재생
	if (IsValid(DrivingAnimation))
	{
		PlayerMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		PlayerMesh->PlayAnimation(DrivingAnimation, true);
	}

	ULevelSequencePlayer* SequencePlayer = EscapeSequenceActor->GetSequencePlayer();

	if (!IsValid(SequencePlayer))
	{
		return;
	}

	SequencePlayer->OnFinished.RemoveDynamic(this, &AKTEscapeVehicle::HandleSequenceFinished);
	SequencePlayer->OnFinished.AddDynamic(this, &AKTEscapeVehicle::HandleSequenceFinished);

	// 기존 엔딩 시네마틱 시작
	SequencePlayer->Play();
}

void AKTEscapeVehicle::QuitGameAfterEnding()
{
	UKismetSystemLibrary::QuitGame(this, EndingPlayerController, EQuitPreference::Quit, false);
}

