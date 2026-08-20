// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface//KTInteractableInterface.h"
#include "KTItemPickupBase.generated.h"

class UKTItemDataAsset;
class UWidgetComponent;

UCLASS()
class KITTYPROJECT_API AKTItemPickupBase : public AActor, public IKTInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKTItemPickupBase();
	
	// IKTInteractableInterface
	// BlueprintNativeEvent의 C++ 기본 동작을 구현하기 위해 _Implementation을 사용
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionText_Implementation() const override;
	
	void SetPickupInteractionEnabled(bool bEnabled); // 픽업 활성화 함수
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 아이템의 루트이자 Line Trace 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Components")
	TObjectPtr<class USphereComponent> InteractionCollision;

	// 월드에 표시되는 아이템 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Components")
	TObjectPtr<class UStaticMeshComponent> PickupMesh;

	// 화면에 표시할 상호작용 문구
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Interaction")
	FText InteractionText;
	
	// 이 월드 액터가 획득 시 인벤토리에 넣을 아이템 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Item")
	TObjectPtr<UKTItemDataAsset> ItemData;
	
	// Interaction Animation Section
public:
	// Notify 시점에 아이템 획득 처리
	bool CompletePickup(class AKittyCharacterPlayer* Player);
	
	// 아이템마다 사용할 획득 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Animation")
	TObjectPtr<class UAnimMontage> PickupMontage;
	
	// 이 아이템과 상호작용할 수 있는 거리 (애니메이션 때문에 따로 설정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Interaction")
	float PickupInteractionDistance = 150.0f;

	float GetPickupInteractionDistance() const
	{
		return PickupInteractionDistance;
	}
};
