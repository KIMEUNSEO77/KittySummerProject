#include "Interaction/KTExamineActor.h"
#include "Inventory/KTItemDataAsset.h"
#include "Character/KittyCharacterPlayer.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/KittyPlayerController.h"
#include "Mission/KTMissionSubsystem.h"

AKTExamineActor::AKTExamineActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionCollision =
		CreateDefaultSubobject<USphereComponent>(
			TEXT("InteractionCollision")
		);

	SetRootComponent(InteractionCollision);

	InteractionCollision->InitSphereRadius(70.0f);
	InteractionCollision->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);
	InteractionCollision->SetCollisionObjectType(
		ECC_WorldDynamic
	);
	InteractionCollision->SetCollisionResponseToAllChannels(
		ECR_Ignore
	);
	InteractionCollision->SetCollisionResponseToChannel(
		ECC_Visibility,
		ECR_Block
	);

	ExamineMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("ExamineMesh")
		);

	ExamineMesh->SetupAttachment(InteractionCollision);
	ExamineMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	InteractionText = FText::FromString(
		TEXT("살펴보기")
	);
}

void AKTExamineActor::Interact_Implementation(
	AActor* Interactor)
{
	AKittyCharacterPlayer* Player =
		Cast<AKittyCharacterPlayer>(Interactor);

	if (!IsValid(Player) || !IsValid(ItemData.Get()))
	{
		return;
	}

	AKittyPlayerController* PlayerController =
		Cast<AKittyPlayerController>(
			Player->GetController()
		);

	if (!IsValid(PlayerController))
	{
		return;
	}

	PlayerController->OpenExamine(ItemData);

	if (ExaminedEventTag.IsValid())
	{
		if (UKTMissionSubsystem* MissionSubsystem =
			UKTMissionSubsystem::Get(this))
		{
			MissionSubsystem->BroadcastMissionEvent(
				ExaminedEventTag,
				Interactor
			);
		}
	}
}

FText AKTExamineActor::GetInteractionText_Implementation()
	const
{
	return InteractionText;
}