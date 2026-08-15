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

	bHasStartedEnding = true;

	// 다시 상호작용하지 못하도록 Collision을 끔
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 플레이어를 운전석 위치로 이동시킨 후 차량에 붙임
	PlayerPawn->SetActorLocationAndRotation(DriverSeatPoint->GetComponentLocation(), DriverSeatPoint->GetComponentRotation());

	PlayerPawn->AttachToComponent(VehicleMesh, FAttachmentTransformRules::KeepWorldTransform);

	// 외부 카메라 연출이므로 캐릭터를 숨김
	PlayerPawn->SetActorHiddenInGame(true);
	PlayerPawn->SetActorEnableCollision(false);

	// 이동과 카메라 조작을 막고 시네마틱 모드로 전환
	PlayerController->SetCinematicMode(
		true,  // 시네마틱 모드
		false, // 플레이어 숨기기: 위에서 직접 숨김
		true,  // HUD 영향
		true,  // 이동 제한
		true   // 시점 회전 제한
	);

	SequencePlayer->Play();
}

FText AKTEscapeVehicle::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("[F] 차량에 탑승하기"));
}

