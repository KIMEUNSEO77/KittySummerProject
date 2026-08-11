// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/KittyCharacterBase.h"
#include "Interface/KittyCharacterAIInterface.h"
#include "KittyCharacterNonplayer.generated.h"

/**
 * 
 */

class UAnimMontage;
class ATargetPoint;
 
UENUM(BlueprintType)
enum class EKittyDeathDirection : uint8
{
	None  UMETA(DisplayName = "None"),
	Front UMETA(DisplayName = "Front"),
	Back  UMETA(DisplayName = "Back"),
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UCLASS()
class KITTYPROJECT_API AKittyCharacterNonplayer : public AKittyCharacterBase, public IKittyCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AKittyCharacterNonplayer();
	
	bool IsPatrolEnabled() const
	{
		return bPatrolEnabled;
	}
	
	const TArray<TObjectPtr<ATargetPoint>>& GetPatrolPoints() const
	{
		return PatrolPoints;
	}
	
	bool IsDead() const
	{
		return bIsDead;
	}
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnDamaged(AActor* DamageActor,
		float Damage, 
		const UDamageType* DamageType, 
		AController* InstigatedBy, 
		AActor* DamageCauser);

protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;	
	
private:
	
	//순찰가능 여부를 판단
	UPROPERTY(EditAnywhere, Category = AI)
	bool bPatrolEnabled;
	//순찰 위치를 저장하는 배열
	UPROPERTY(EditAnywhere, Category = AI)
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;
	//죽었는지 판단
	UPROPERTY(EditAnywhere, Category = "Debug|Death",meta = (AllowPrivateAccess = "true"))
	bool bIsDead;
	//죽은 상태를 디버그하기위한 변수
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category = "Debug|Death", meta = (AllowPrivateAccess = "true"))
	EKittyDeathDirection DeathDirection = EKittyDeathDirection::None;
	//각 방향별 죽는 몽타주
	UPROPERTY(VisibleDefaultsOnly, Category = "Animation|Death")
	TObjectPtr<UAnimMontage> BackDeathMontage;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Animation|Death")
	TObjectPtr<UAnimMontage> FrontDeathMontage;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Animation|Death")
	TObjectPtr<UAnimMontage> LeftDeathMontage;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Animation|Death")
	TObjectPtr<UAnimMontage> RightDeathMontage;


private:
	//죽는 방향 선택
	EKittyDeathDirection CalculateDeathDirection(AController* InstigatedBy,AActor* DamageCauser) const;
	//죽을때 재생할 몽타주 선택
	UAnimMontage* GetDeathMontage(EKittyDeathDirection Direction) const;
	//선택된 몽타주로 죽는 모션 실행.
	void Die(UAnimMontage* DeathMontage);
	
private:
	UPROPERTY(EditAnywhere, Category = "Debug|Death")
	EKittyDeathDirection DebugDirection = EKittyDeathDirection::Front;
	
	UFUNCTION(CallInEditor, Category = "Debug|Death")
	void DebugKill();
	
	// Item Section
private:
	// 경비원이 소지한 아이템 종류
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AKTItemPickupBase> CarriedItemClass;

	// 아이템을 붙일 메시 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (AllowPrivateAccess = "true"))
	FName CarriedItemSocketName = TEXT("KeycardSocket");

	// 허리에 붙어 있는 아이템
	UPROPERTY()
	TObjectPtr<class AKTItemPickupBase> CarriedItem;
};
