// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/KTItemPickupBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/KittyCharacterPlayer.h"
#include "Inventory/KTInventoryComponent.h"
#include "Inventory/KTItemDataAsset.h"
#include "Mission/KTMissionSubsystem.h"


// Sets default values
AKTItemPickupBase::AKTItemPickupBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Line Trace가 감지할 구 형태의 충돌체를 만듦
	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));

	SetRootComponent(InteractionCollision);

	InteractionCollision->InitSphereRadius(50.0f);

	// 물리 충돌은 하지 않고, Line Trace 
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionObjectType(ECC_WorldDynamic);

	// 일단 모든 채널을 무시
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 플레이어의 Visibility Line Trace만 막음
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 월드에 표시할 Static Mesh를 만듦
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));

	PickupMesh->SetupAttachment(InteractionCollision);

	// 메시 자체의 충돌은 끄고, Line Trace 감지는 위의 Sphere Collision이 담당
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetGenerateOverlapEvents(false);
	
	// 기본 안내 문구
	InteractionText = FText::FromString(TEXT("아이템 획득하기"));
}

// Called when the game starts or when spawned
void AKTItemPickupBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKTItemPickupBase::Interact_Implementation(AActor* Interactor)
{
	AKittyCharacterPlayer* Player =
		Cast<AKittyCharacterPlayer>(Interactor);

	if (!IsValid(Player) || !IsValid(ItemData))
	{
		return;
	}

	UKTInventoryComponent* Inventory =
		Player->GetInventoryComponent();

	if (!IsValid(Inventory))
	{
		return;
	}

	// 인벤토리에 정상적으로 들어간 경우에만
	// 맵의 아이템을 삭제합니다.
	if (!Inventory->AddItem(ItemData))
	{
		return;
	}

	// 이 아이템이 미션과 관련 있다면 완료 태그를 전송합니다.
	if (ItemData->AcquiredEventTag.IsValid())
	{
		if (UKTMissionSubsystem* MissionSubsystem =
			UKTMissionSubsystem::Get(this))
		{
			MissionSubsystem->BroadcastMissionEvent(
				ItemData->AcquiredEventTag,
				Interactor
			);
		}
	}

	Destroy();
}

FText AKTItemPickupBase::GetInteractionText_Implementation() const
{
	return InteractionText;
}

void AKTItemPickupBase::SetPickupInteractionEnabled(bool bEnabled)
{
	if (!IsValid(InteractionCollision))
	{
		return;
	}

	InteractionCollision->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

