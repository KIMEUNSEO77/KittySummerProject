// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTCommunicationTerminal.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AKTCommunicationTerminal::AKTCommunicationTerminal()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	SetRootComponent(SceneRoot);

	TerminalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TerminalMesh"));

	TerminalMesh->SetupAttachment(SceneRoot);
	TerminalMesh->SetCollisionProfileName(TEXT("BlockAll"));

	InteractionCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));

	InteractionCollision->SetupAttachment(SceneRoot);

	// 단말기 앞쪽 상호작용 영역의 기본 크기
	InteractionCollision->SetBoxExtent(FVector(80.0f, 80.0f, 120.0f));

	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 플레이어의 상호작용 탐색 코드가 WorldDynamic을 검사함
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);

	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	InteractionCollision->SetGenerateOverlapEvents(true);
}

void AKTCommunicationTerminal::Interact_Implementation(AActor* Interactor)
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
			TEXT("통신 단말기 조사 완료")
		);
	}
}

FText AKTCommunicationTerminal::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("[F] 통신 단말기 조사하기"));
}

