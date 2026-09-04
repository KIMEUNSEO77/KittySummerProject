#include "Vision/KTSpectralVisionComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Vision/KTSpectralVisionSubsystem.h"
#include "Misc/App.h"
#include "Kismet/GameplayStatics.h"
#include "Character/KittyCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/KittyPlayerController.h"

UKTSpectralVisionComponent::UKTSpectralVisionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UKTSpectralVisionComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(
        DeltaTime,
        TickType,
        ThisTickFunction
    );

    UpdateVisionTransition();

    if (bVisionEnabled)
    {
        UpdateDepthOfField();
    }
}

void UKTSpectralVisionComponent::BeginPlay()
{
    Super::BeginPlay();

    bVisionEnabled = false;

    if (
        UKTSpectralVisionSubsystem* VisionSubsystem =
            GetVisionSubsystem()
    )
    {
        VisionSubsystem->SetVisionEnabled(false);
    }
}

void UKTSpectralVisionComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    DeactivateVision();
    // 맵 종료 중에는 다음 Tick이 없을 수 있으므로
    // 페이드를 기다리지 않고 즉시 원래 설정을 복구합니다.
    VisionTransitionAlpha = 0.0f;
    VisionTransitionDirection = 0.0f;

    RestoreVisionVisuals();
    
    Super::EndPlay(EndPlayReason);
}

void UKTSpectralVisionComponent::ToggleVision()
{
    if (bVisionEnabled)
    {
        DeactivateVision();
    }
    else
    {
        ActivateVision();
    }
}

void UKTSpectralVisionComponent::ActivateVision()
{
    if (bVisionEnabled ||
       !CanActivateVision())
    {
        return;
    }

    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    USpringArmComponent* CameraBoom =
        Player->GetCameraBoom();

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(CameraBoom) ||
        !IsValid(FollowCamera))
    {
        return;
    }

    UKTSpectralVisionSubsystem* VisionSubsystem =
        GetVisionSubsystem();

    if (!IsValid(VisionSubsystem))
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!IsValid(World))
    {
        return;
    }

    // Spectral Vision 진입 전 현재 카메라 상태 저장
    SavedCameraArmLength =
        CameraBoom->TargetArmLength;

    SavedCameraSocketOffset =
        CameraBoom->SocketOffset;

    SavedCameraFOV =
        FollowCamera->FieldOfView;

    // 진입 애니메이션 시작

    bVisionEnabled = true;
    
    // 흑백 Post Process 적용
    ApplyVisionVisuals();
    VisionTransitionDirection = 1.0f;
    
    // 현재 시간 배율을 저장한 후 슬로모션 적용
    PreviousTimeDilation =
        UGameplayStatics::GetGlobalTimeDilation(this);

    UGameplayStatics::SetGlobalTimeDilation(
        this,
        VisionTimeDilation
    );

    // 현재 플레이어 위치를 기준으로 범위 검사
    VisionSubsystem->UpdateTargetsInRange(
        Player->GetActorLocation(),
        HighlightRange
    );

    VisionSubsystem->SetVisionEnabled(true);

    World->GetTimerManager().SetTimer(
        TargetRefreshTimerHandle,
        this,
        &UKTSpectralVisionComponent::RefreshTargets,
        TargetRefreshInterval,
        true
    );
}

void UKTSpectralVisionComponent::DeactivateVision()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(
            TargetRefreshTimerHandle
        );
    }

    if (!bVisionEnabled)
    {
        return;
    }

    bVisionEnabled = false;
    
    // 흑백에서 컬러 방향으로 페이드를 시작합니다.
    VisionTransitionDirection = -1.0f;
    
    // DOF 설정 복구
    RestoreDepthOfField();
    
    // Spectral Vision을 켜기 전 시간 배율로 복구
    UGameplayStatics::SetGlobalTimeDilation(
        this,
        PreviousTimeDilation
    );

    if (UKTSpectralVisionSubsystem* VisionSubsystem =
        GetVisionSubsystem())
    {
        VisionSubsystem->SetVisionEnabled(false);
    }
}

void UKTSpectralVisionComponent::RefreshTargets()
{
    if (!bVisionEnabled)
    {
        return;
    }

    const AActor* Owner = GetOwner();

    if (!IsValid(Owner))
    {
        DeactivateVision();
        return;
    }

    if (
        UKTSpectralVisionSubsystem* VisionSubsystem =
            GetVisionSubsystem()
    )
    {
        VisionSubsystem->UpdateTargetsInRange(
            Owner->GetActorLocation(),
            HighlightRange
        );
    }
}

UKTSpectralVisionSubsystem*
UKTSpectralVisionComponent::GetVisionSubsystem() const
{
    const UWorld* World = GetWorld();

    if (!IsValid(World))
    {
        return nullptr;
    }

    return World->GetSubsystem<
        UKTSpectralVisionSubsystem
    >();
}

void UKTSpectralVisionComponent::UpdateVisionTransition()
{
    // 전환 중이 아니면 아무것도 하지 않습니다.
    if (FMath::IsNearlyZero(
        VisionTransitionDirection
    ))
    {
        return;
    }

    if (!bHasSavedVisualSettings)
    {
        return;
    }

    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    // 슬로모션의 영향을 받지 않는 실제 프레임 시간을 사용합니다.
    const float RealDeltaTime =
        FApp::GetDeltaTime();

    const float SafeFadeDuration =
        FMath::Max(
            VisionFadeDuration,
            0.01f
        );

    VisionTransitionAlpha +=
        VisionTransitionDirection *
        RealDeltaTime /
        SafeFadeDuration;

    VisionTransitionAlpha =
        FMath::Clamp(
            VisionTransitionAlpha,
            0.0f,
            1.0f
        );

    // 시작과 끝이 부드러운 Ease In/Out 값입니다.
    const float SmoothAlpha =
        FMath::InterpEaseInOut(
            0.0f,
            1.0f,
            VisionTransitionAlpha,
            2.0f
        );

    const FVector4 GrayscaleSaturation(
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    Settings.bOverride_ColorSaturation = true;

    Settings.ColorSaturation =
        FMath::Lerp(
            SavedColorSaturation,
            GrayscaleSaturation,
            SmoothAlpha
        );

    FollowCamera->PostProcessBlendWeight = 1.0f;

    // 흑백 전환이 끝났습니다.
    if (
        VisionTransitionDirection > 0.0f &&
        VisionTransitionAlpha >= 1.0f
    )
    {
        VisionTransitionAlpha = 1.0f;
        VisionTransitionDirection = 0.0f;

        Settings.ColorSaturation =
            GrayscaleSaturation;

        return;
    }

    // 컬러 복귀가 끝났습니다.
    if (
        VisionTransitionDirection < 0.0f &&
        VisionTransitionAlpha <= 0.0f
    )
    {
        VisionTransitionAlpha = 0.0f;
        VisionTransitionDirection = 0.0f;

        // 전환이 완전히 끝난 후 원래 Override 값과
        // Post Process Blend Weight까지 복구합니다.
        RestoreVisionVisuals();
    }
}

AKittyCharacterPlayer*
UKTSpectralVisionComponent::GetPlayerCharacter() const
{
    return Cast<AKittyCharacterPlayer>(
        GetOwner()
    );
}

bool UKTSpectralVisionComponent::CanActivateVision() const
{
    const AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return false;
    }

    const AKittyPlayerController* Controller =
        Cast<AKittyPlayerController>(
            Player->GetController()
        );

    if (!IsValid(Controller))
    {
        return false;
    }

    return
        !Controller->IsInventoryOpen() &&
        !Controller->IsExamineOpen() &&
        !Controller->IsGameplayUIHidden() &&
        !Controller->IsRadioInputLocked();
}


void UKTSpectralVisionComponent::UpdateDepthOfField()
{
    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    const FVector CameraLocation =
        FollowCamera->GetComponentLocation();

    const FVector ChestLocation =
        Player->GetActorLocation() +
        FVector(0.0f, 0.0f, 60.0f);

    const float FocusDistance =
        FVector::Distance(
            CameraLocation,
            ChestLocation
        );

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    Settings.bOverride_DepthOfFieldFstop = true;
    Settings.DepthOfFieldFstop = VisionFStop;

    Settings.bOverride_DepthOfFieldFocalDistance = true;
    Settings.DepthOfFieldFocalDistance = FocusDistance;

    Settings.bOverride_DepthOfFieldFocalRegion = true;
    Settings.DepthOfFieldFocalRegion =
        VisionFocalRegion;

    Settings.bOverride_DepthOfFieldNearTransitionRegion =
        true;

    Settings.DepthOfFieldNearTransitionRegion =
        150.0f;

    Settings.bOverride_DepthOfFieldFarTransitionRegion =
        true;

    Settings.DepthOfFieldFarTransitionRegion =
        400.0f;

    // 카메라 Post Process 설정이 화면에 적용되도록 설정
    FollowCamera->PostProcessBlendWeight = 1.0f;
}

void UKTSpectralVisionComponent::RestoreDepthOfField()
{
    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    Settings.bOverride_DepthOfFieldFstop = false;

    Settings.bOverride_DepthOfFieldFocalDistance =
        false;

    Settings.bOverride_DepthOfFieldFocalRegion =
        false;

    Settings.bOverride_DepthOfFieldNearTransitionRegion =
        false;

    Settings.bOverride_DepthOfFieldFarTransitionRegion =
        false;
}


void UKTSpectralVisionComponent::ApplyVisionVisuals()
{
    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    // 최초 활성화 시에만 기존 설정을 저장합니다.
    // 같은 설정을 여러 번 덮어쓰지 않도록 보호합니다.
    if (!bHasSavedVisualSettings)
    {
        bSavedOverrideColorSaturation =
            Settings.bOverride_ColorSaturation;

        SavedColorSaturation =
            Settings.ColorSaturation;

        SavedPostProcessBlendWeight =
            FollowCamera->PostProcessBlendWeight;

        bHasSavedVisualSettings = true;
    }

    // 카메라가 Color Saturation 값을 사용하도록 설정합니다.
    Settings.bOverride_ColorSaturation = true;

    // 즉시 흑백으로 만들지 않습니다.
    // UpdateVisionTransition()이 현재 Alpha에 따라
    // 컬러와 흑백 사이를 매 프레임 계산합니다.
    Settings.ColorSaturation =
        FMath::Lerp(
            SavedColorSaturation,
            FVector4(
                0.0f,
                0.0f,
                0.0f,
                1.0f
            ),
            VisionTransitionAlpha
        );

    FollowCamera->PostProcessBlendWeight = 1.0f;
}

void UKTSpectralVisionComponent::RestoreVisionVisuals()
{
    if (!bHasSavedVisualSettings)
    {
        return;
    }

    AKittyCharacterPlayer* Player =
        GetPlayerCharacter();

    if (!IsValid(Player))
    {
        return;
    }

    UCameraComponent* FollowCamera =
        Player->GetFollowCamera();

    if (!IsValid(FollowCamera))
    {
        return;
    }

    FPostProcessSettings& Settings =
        FollowCamera->PostProcessSettings;

    // Spectral Vision 진입 전에 사용하던 설정으로 복원합니다.
    Settings.bOverride_ColorSaturation =
        bSavedOverrideColorSaturation;

    Settings.ColorSaturation =
        SavedColorSaturation;

    FollowCamera->PostProcessBlendWeight =
        SavedPostProcessBlendWeight;

    bHasSavedVisualSettings = false;
}