// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/KTItemPickupBase.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/KittyCharacterPlayer.h"
#include "Engine/Engine.h"

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
	InteractionText = FText::FromString(TEXT("[F] 아이템 획득하기"));
}

// Called when the game starts or when spawned
void AKTItemPickupBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKTItemPickupBase::Interact_Implementation(AActor* Interactor)
{
	// 권총, 출입증 등의 자식 클래스에서 실제 획득 처리를 구현
	AKittyCharacterPlayer* Player = Cast<AKittyCharacterPlayer>(Interactor);

	if (!IsValid(Player))
	{
		return;
	}

	const bool bAcquired =
		Player->AcquirePistol(this);

	if (!bAcquired)
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			4,
			2.0f,
			FColor::Green,
			TEXT("권총 획득 성공")
		);
	}
}

FText AKTItemPickupBase::GetInteractionText_Implementation() const
{
	return InteractionText;
}

