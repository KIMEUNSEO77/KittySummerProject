// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTKeycardDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AKTKeycardDoor::AKTKeycardDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(SceneRoot);

	// 플레이어와 총의 Line Trace가 문을 통과하지 않도록 설정
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

