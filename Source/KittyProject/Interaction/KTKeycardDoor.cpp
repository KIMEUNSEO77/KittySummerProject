// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTKeycardDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"

// Sets default values
AKTKeycardDoor::AKTKeycardDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	InteractionCollision =
	CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));

	InteractionCollision->SetupAttachment(SceneRoot);
	InteractionCollision->SetSphereRadius(100.0f);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	InteractionCollision->SetGenerateOverlapEvents(true);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(SceneRoot);

	// 플레이어와 총의 Line Trace가 문을 통과하지 않도록 설정
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AKTKeycardDoor::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor))
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			TEXT("키카드 문 상호작용 성공")
		);
	}
}

FText AKTKeycardDoor::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("[F] 출입증 사용하기"));
}

