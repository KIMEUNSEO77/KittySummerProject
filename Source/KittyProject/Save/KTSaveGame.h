// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KTSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KITTYPROJECT_API UKTSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	// 추후 저장 형식이 변경될 때 구분하기 위한 버전
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	int32 SaveVersion = 1;

	// 저장 당시 플레이 중이던 레벨
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FName SavedLevelName = NAME_None;

	// 현재 미션 단계
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	int32 MissionStepIndex = 0;

	// 마지막으로 통과한 체크포인트
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FName CheckpointId = NAME_None;

	// 체크포인트가 없을 때 사용할 플레이어 위치
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FRotator PlayerRotation = FRotator::ZeroRotator;
};
