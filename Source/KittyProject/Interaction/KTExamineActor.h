#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interface/KTInteractableInterface.h"
#include "KTExamineActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UKTItemDataAsset;

UCLASS()
class KITTYPROJECT_API AKTExamineActor
	: public AActor,
	  public IKTInteractableInterface
{
	GENERATED_BODY()

public:
	AKTExamineActor();

	virtual void Interact_Implementation(
		AActor* Interactor
	) override;

	virtual FText GetInteractionText_Implementation()
		const override;

protected:
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Examine|Components"
	)
	TObjectPtr<USphereComponent> InteractionCollision;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Examine|Components"
	)
	TObjectPtr<UStaticMeshComponent> ExamineMesh;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Examine|Interaction"
	)
	FText InteractionText;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Examine|Data"
	)
	TObjectPtr<UKTItemDataAsset> ItemData;
	
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Examine|Mission"
	)
	FGameplayTag ExaminedEventTag;
};