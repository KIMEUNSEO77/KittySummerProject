// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Mission/KTMissionTypes.h"
#include "KTMissionDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class KITTYPROJECT_API UKTMissionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FName MissionID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	FText MissionTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
	TArray<FMissionStep> Steps;
};
