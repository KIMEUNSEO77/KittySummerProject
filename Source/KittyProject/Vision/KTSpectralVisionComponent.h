#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KTSpectralVisionComponent.generated.h"

class UKTSpectralVisionSubsystem;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class USoundBase;
class AKittyCharacterPlayer;


UCLASS(
	ClassGroup = (SpectralVision),
	meta = (BlueprintSpawnableComponent)
)
class KITTYPROJECT_API UKTSpectralVisionComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UKTSpectralVisionComponent();

	UFUNCTION(
		BlueprintCallable,
		Category = "Spectral Vision"
	)
	void ToggleVision();

	UFUNCTION(
		BlueprintCallable,
		Category = "Spectral Vision"
	)
	void ActivateVision();

	UFUNCTION(
		BlueprintCallable,
		Category = "Spectral Vision"
	)
	void DeactivateVision();

	UFUNCTION(
		BlueprintPure,
		Category = "Spectral Vision"
	)
	bool IsVisionEnabled() const
	{
		return bVisionEnabled;
	}

protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;
	
	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:
	void RefreshTargets();

	UKTSpectralVisionSubsystem*
		GetVisionSubsystem() const;
	
	void UpdateVisionTransition();
	void UpdateDepthOfField();
	
	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Visual")
	TObjectPtr<UMaterialInterface> SpectralPostProcessMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SpectralMaterialInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Time",
		meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float VisionTimeDilation = 0.65f;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Camera")
	float VisionCameraArmLength = 290.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Camera")
	FVector VisionCameraSocketOffset =
		FVector(0.0f, 110.0f, 55.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Camera")
	float VisionCameraFOV = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Camera")
	float CameraTransitionDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|DOF")
	float VisionFStop = 4.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|DOF")
	float VisionFocalRegion = 120.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Sound")
	TObjectPtr<USoundBase> VisionActivateSound;

	UPROPERTY(EditDefaultsOnly, Category = "Spectral Vision|Sound")
	TObjectPtr<USoundBase> VisionDeactivateSound;

	float PreviousTimeDilation = 1.0f;

	float SavedCameraArmLength = 0.0f;
	FVector SavedCameraSocketOffset = FVector::ZeroVector;
	float SavedCameraFOV = 90.0f;
	
// Spectral Vision 진입 전 카메라 색상 설정을 저장합니다.
bool bSavedOverrideColorSaturation = false;

FVector4 SavedColorSaturation =
    FVector4(1.0f, 1.0f, 1.0f, 1.0f);

float SavedPostProcessBlendWeight = 0.0f;

bool bHasSavedVisualSettings = false;
	
 //컬러와 흑백 사이를 전환하는 시간입니다.
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spectral Vision|Visual",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.01",
			Units = "s"
		)
	)
	float VisionFadeDuration = 0.3f;

	
	float VisionTransitionAlpha = 0.0f;
	float VisionTransitionDirection = 0.0f;
	
private:
	/**
	 * 일반 경비원이 표시되는 최대 거리입니다.
	 * Unreal 단위 100cm = 1m이므로 3000 = 30m입니다.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spectral Vision|Targeting",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.0",
			Units = "cm"
		)
	)
	float HighlightRange = 3000.0f;

	/**
	 * Vision 활성화 중 거리 조건을 다시 검사하는 간격입니다.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spectral Vision|Targeting",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.02",
			Units = "s"
		)
	)
	float TargetRefreshInterval = 0.1f;

	bool bVisionEnabled = false;

	FTimerHandle TargetRefreshTimerHandle;
	
private:
	bool CanActivateVision() const;

	void ApplyVisionVisuals();
	void RestoreVisionVisuals();
	void RestoreDepthOfField();

	AKittyCharacterPlayer* GetPlayerCharacter() const;

};