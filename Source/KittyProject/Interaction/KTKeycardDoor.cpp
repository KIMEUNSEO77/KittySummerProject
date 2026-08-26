// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/KTKeycardDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Character/KittyCharacterPlayer.h"
#include "Inventory/KTInventoryComponent.h"
#include "Mission/KTMissionSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/Engine.h"

// Sets default values
AKTKeycardDoor::AKTKeycardDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

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
	
	CardReaderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReaderMesh"));
	CardReaderMesh->SetupAttachment(RootComponent);
	CardReaderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKTKeycardDoor::CompleteKeycardInteraction()
{
	// 이미 열렸거나 열리는 중이면 실행하지 않음
	if (bIsOpen || bIsOpening)
	{
		return;
	}

	// 문 열기 시작
	bIsOpening = true;
	SetActorTickEnabled(true);

	// 다시 상호작용되지 않도록 충돌 비활성화
	if (IsValid(InteractionCollision))
	{
		InteractionCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			TEXT("출입증 인증 완료")
		);
	}
}

void AKTKeycardDoor::BeginPlay()
{
	Super::BeginPlay();
	
	// 블루프린트에서 설정한 문 메시의 현재 상대 위치 저장
	ClosedLocation = DoorMesh->GetRelativeLocation();

	// 닫힌 위치에서 위쪽으로 OpenHeight만큼 이동한 위치
	OpenLocation = ClosedLocation + FVector(0.0f, 0.0f, OpenHeight);
}

void AKTKeycardDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsOpening || !IsValid(DoorMesh))
	{
		return;
	}

	const FVector CurrentLocation = DoorMesh->GetRelativeLocation();
	const FVector NewLocation = FMath::VInterpTo(CurrentLocation, OpenLocation, DeltaTime, OpenSpeed);

	DoorMesh->SetRelativeLocation(NewLocation);

	// 목표 위치에 거의 도착
	if (NewLocation.Equals(OpenLocation, 1.0f))
	{
		DoorMesh->SetRelativeLocation(OpenLocation);

		bIsOpening = false;
		bIsOpen = true;
		
		if (UKTMissionSubsystem* MissionSubsystem =UKTMissionSubsystem::Get(this))
		{
			const FGameplayTag DoorOpenedTag =
				FGameplayTag::RequestGameplayTag(
					FName("Mission.Event.Door.KeycardAccepted")
				);

			MissionSubsystem->BroadcastMissionEvent(
				DoorOpenedTag,
				nullptr
			);
		}
		
		// Tick 중지
		SetActorTickEnabled(false);
	}
}

void AKTKeycardDoor::Interact_Implementation(AActor* Interactor)
{
	// 상호작용한 Actor가 플레이어인지 확인
	AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(Interactor);

	if (!IsValid(Player))
	{
		return;
	}
	
	// 플레이어가 카드 단말기의 상호작용 범위 안에 있는지 확인
	if (!InteractionCollision->IsOverlappingActor(Player))
	{
		return;
	}

	// 플레이어의 인벤토리 가져오기
	UKTInventoryComponent* Inventory = Player->GetInventoryComponent();

	if (!IsValid(Inventory))
	{
		return;
	}

	// 출입증 보유 여부 확인
	const bool bHasKeycard = Inventory->HasItemByID(FName(TEXT("Keycard")));

	if (!bHasKeycard)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Red,
				TEXT("출입증이 필요합니다.")
			);
		}

		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			TEXT("출입증 확인 완료!")
		);
	}
	
	// 이미 열렸거나 열리는 중이라면 다시 실행하지 않음
	if (bIsOpen || bIsOpening)
	{
		return;
	}

	// 출입증 사용 애니메이션 시작
	Player->StartKeycardDoorInteraction(this);
}

FText AKTKeycardDoor::GetInteractionText_Implementation() const
{
	return FText::FromString(TEXT("출입증 사용하기"));
}

