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

	// 플레이어를 운전석 위치로 이동시킨 후 차량에 붙임
	PlayerPawn->SetActorLocationAndRotation(DriverSeatPoint->GetComponentLocation(), DriverSeatPoint->GetComponentRotation());

	PlayerPawn->AttachToComponent(VehicleMesh, FAttachmentTransformRules::KeepWorldTransform);

	// 외부 카메라 연출이므로 캐릭터를 숨김
	PlayerPawn->SetActorHiddenInGame(true);
	PlayerPawn->SetActorEnableCollision(false);
	
	// 시네마틱 시작 전에 UMG UI 숨기기
	AKittyPlayerController* KittyPlayerController = Cast<AKittyPlayerController>(PlayerController);

	if (IsValid(KittyPlayerController))
	{
		KittyPlayerController->SetGameplayUIVisible(false);
	}

	// 이동과 카메라 조작을 막고 시네마틱 모드로 전환
	PlayerController->SetCinematicMode(
		true,  // 시네마틱 모드
		false, // 플레이어 숨기기: 위에서 직접 숨김
		true,  // HUD 영향
		true,  // 이동 제한
		true   // 시점 회전 제한
	);

	// 같은 이벤트가 중복으로 연결되지 않도록 기존 연결 제거
	SequencePlayer->OnFinished.RemoveDynamic(this, &AKTEscapeVehicle::HandleSequenceFinished);

	// 시네마틱이 끝나면 HandleSequenceFinished 호출
	SequencePlayer->OnFinished.AddDynamic(this, &AKTEscapeVehicle::HandleSequenceFinished);

	SequencePlayer->Play();
}

FText AKTEscapeVehicle::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("[F] 차량에 탑승하기"));
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
}

