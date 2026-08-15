// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTEscapeVehicle.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

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

	bHasStartedEnding = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Green,
			TEXT("탈출 차량 탑승 성공")
		);
	}
}

FText AKTEscapeVehicle::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("[F] 차량에 탑승하기"));
}

