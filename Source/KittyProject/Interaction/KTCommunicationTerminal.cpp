// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTCommunicationTerminal.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Components/WidgetComponent.h"
#include "Mission/KTMissionSubsystem.h"
#include "GameplayTagContainer.h"
#include "Character/KittyCharacterPlayer.h"

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
	
	HologramWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HologramWidget"));
	HologramWidget->SetupAttachment(SceneRoot);

	// 월드 공간에 홀로그램처럼 표시
	HologramWidget->SetWidgetSpace(EWidgetSpace::World);

	// 기본 위젯 해상도
	HologramWidget->SetDrawSize(FVector2D(800.0f, 450.0f));

	// 양쪽에서 보이게 설정
	HologramWidget->SetTwoSided(true);

	// 처음에는 숨겨둠
	HologramWidget->SetVisibility(false);
}

void AKTCommunicationTerminal::CompleteTerminalInteraction(AActor* Interactor)
{
	// 중복 완료 방지
	if (bIsInvestigated)
	{
		return;
	}

	bIsInvestigated = true;

	if (IsValid(HologramWidget))
	{
		HologramWidget->SetVisibility(true);
	}

	// 통신 단말기 조사 완료 이벤트 전송
	if (UKTMissionSubsystem* MissionSubsystem = UKTMissionSubsystem::Get(this))
	{
		const FGameplayTag TerminalExaminedTag =
			FGameplayTag::RequestGameplayTag(
				FName(
					"Mission.Event.Examine."
					"CommunicationTerminal.Completed"
				)
			);

		MissionSubsystem->BroadcastMissionEvent(TerminalExaminedTag, Interactor);
	}

	// 다시 상호작용되지 않도록 충돌 비활성화
	if (IsValid(InteractionCollision))
	{
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AKTCommunicationTerminal::Interact_Implementation(AActor* Interactor)
{
	AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(Interactor);

	if (!IsValid(Player) || bIsInvestigated)
	{
		return;
	}

	// 홀로그램을 바로 띄우지 않고 조작 애니메이션부터 시작
	Player->StartTerminalInteraction(this);
}

FText AKTCommunicationTerminal::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("통신 단말기 조사하기"));
}

