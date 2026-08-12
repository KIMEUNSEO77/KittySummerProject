// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KTKeycardDoor.generated.h"

UCLASS()
class KITTYPROJECT_API AKTKeycardDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKTKeycardDoor();

protected:
	// 문틀처럼 움직이지 않는 부분
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	// 실제로 열릴 문 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
};
