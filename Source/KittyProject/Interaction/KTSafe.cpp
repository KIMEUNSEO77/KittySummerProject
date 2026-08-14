// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTSafe.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AKTSafe::AKTSafe()
{
	PrimaryActorTick.bCanEverTick = false;

	// 기본 루트
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// 금고 본체
	SafeBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SafeBodyMesh"));
	SafeBodyMesh->SetupAttachment(SceneRoot);

	// 문의 경첩 역할을 할 회전축
	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(SceneRoot);

	// 금고 문
	SafeDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SafeDoorMesh"));
	SafeDoorMesh->SetupAttachment(DoorPivot);

	// 상호작용 감지용 충돌체
	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(SceneRoot);
	InteractionCollision->InitSphereRadius(100.0f);

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
}

void AKTSafe::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor))
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Green,TEXT("금고 상호작용 성공"));
	}
}

FText AKTSafe::GetInteractionText_Implementation() const
{
	if (bIsOpen)
	{
		return FText::FromString(TEXT("금고가 열려 있습니다"));
	}

	return FText::FromString(TEXT("[F] 금고 비밀번호 입력하기"));
}

