#include "Cinematic/KTReinforcementCinematicDirector.h"

#include "Animation/AnimSequence.h"
#include "Camera/PlayerCameraManager.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Mission/KTMissionSubsystem.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Player/KittyPlayerController.h"

namespace
{
	constexpr float CameraFadeDuration = 0.18f;
	// The Guard skeletal mesh faces +Y while the actor's forward axis is +X.
	// Compensate for that asset-space offset so guards face their travel direction.
	constexpr float GuardVisualYawOffset = -90.0f;
}

AKTReinforcementCinematicDirector::AKTReinforcementCinematicDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	static ConstructorHelpers::FClassFinder<AActor> GuardBlueprint(
		TEXT("/Game/AI/BP_CinematicGuard"));
	if (GuardBlueprint.Succeeded())
	{
		CinematicGuardClass = GuardBlueprint.Class;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> RunAnimation(
		TEXT("/Game/Animations/GuardRunning.GuardRunning"));
	if (RunAnimation.Succeeded())
	{
		RunningAnimation = RunAnimation.Object;
	}
}

void AKTReinforcementCinematicDirector::BeginPlay()
{
	Super::BeginPlay();

	if (!TriggerTag.IsValid())
	{
		TriggerTag = FGameplayTag::RequestGameplayTag(
			TEXT("Mission.Event.Item.DataChip.Acquired"), false);
	}

	if (!CompletionTag.IsValid())
	{
		CompletionTag = FGameplayTag::RequestGameplayTag(
			TEXT("Mission.Event.Cinematic.Reinforcement.Completed"), false);
	}

	if (UKTMissionSubsystem* MissionSubsystem =
		UKTMissionSubsystem::Get(this))
	{
		MissionSubsystem->OnMissionEventReceived.AddDynamic(
			this,
			&AKTReinforcementCinematicDirector::HandleMissionEvent);
	}
}

void AKTReinforcementCinematicDirector::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FinishTransitionTimer);

	if (UKTMissionSubsystem* MissionSubsystem =
		UKTMissionSubsystem::Get(this))
	{
		MissionSubsystem->OnMissionEventReceived.RemoveDynamic(
			this,
			&AKTReinforcementCinematicDirector::HandleMissionEvent);
	}

	if (bIsPlaying)
	{
		SetPlayerCinematicMode(false);
	}

	CleanupCinematicActors();
	Super::EndPlay(EndPlayReason);
}

void AKTReinforcementCinematicDirector::HandleMissionEvent(
	FGameplayTag EventTag,
	AActor* InstigatorActor)
{
	if (bHasPlayed || !EventTag.MatchesTagExact(TriggerTag))
	{
		return;
	}

	TriggerInstigator = InstigatorActor;
	StartCinematic();
}

void AKTReinforcementCinematicDirector::StartCinematic()
{
	bHasPlayed = true;
	ElapsedTime = 0.0f;
	bIsPlaying = true;
	bIsFinishing = false;
	GuardStates.Reset();

	PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PreviousViewTarget = PlayerController->GetViewTarget();
	}

	FActorSpawnParameters CameraSpawnParameters;
	CameraSpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CinematicCamera = GetWorld()->SpawnActor<ACineCameraActor>(
		ACineCameraActor::StaticClass(),
		FTransform::Identity,
		CameraSpawnParameters);

	if (CinematicCamera)
	{
		UCineCameraComponent* CineCamera =
		CinematicCamera->GetCineCameraComponent();

		CineCamera->SetFieldOfView(52.0f);
		CineCamera->SetConstraintAspectRatio(true);
		CineCamera->SetAspectRatio(2.35f);
	}

	const int32 SafeGuardCount = FMath::Clamp(GuardCount, 10, 15);
	const FVector RouteDirection = (RunEnd - RunStart).GetSafeNormal2D();
	const FVector RouteRight(-RouteDirection.Y, RouteDirection.X, 0.0f);
	const float LaneOffsets[3] = {-210.0f, 0.0f, 210.0f};
	FRandomStream GuardVariationRandom(9127);

	for (int32 GuardIndex = 0; GuardIndex < SafeGuardCount; ++GuardIndex)
	{
		const int32 Row = GuardIndex / 3;
		const int32 Lane = GuardIndex % 3;
		const float Stagger = FMath::Max(
			0.0f,
			Row * 0.16f + Lane * 0.045f +
				GuardVariationRandom.FRandRange(-0.08f, 0.12f));
		const float TrailingDistance = Row * 145.0f;
		const FVector LaneOffset = RouteRight * LaneOffsets[Lane];

		FCinematicGuardState State;
		State.Start = RunStart + LaneOffset - RouteDirection * TrailingDistance;
		State.End = RunEnd + LaneOffset - RouteDirection * (Row * 55.0f);
		State.StartDelay = Stagger;
		State.SpeedMultiplier = GuardVariationRandom.FRandRange(0.84f, 1.16f);

		if (CinematicGuardClass)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			const FRotator GuardRotation =
				RouteDirection.Rotation() + FRotator(0.0f, GuardVisualYawOffset, 0.0f);
			AActor* GuardActor = GetWorld()->SpawnActor<AActor>(
				CinematicGuardClass,
				State.Start,
				GuardRotation,
				SpawnParameters);

			if (GuardActor)
			{
				GuardActor->SetActorEnableCollision(false);
				State.Actor = GuardActor;

				if (USkeletalMeshComponent* Mesh =
					GuardActor->FindComponentByClass<USkeletalMeshComponent>())
				{
					Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					if (RunningAnimation)
					{
						Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
						Mesh->PlayAnimation(RunningAnimation, true);
						Mesh->SetPlayRate(State.SpeedMultiplier);
					}
				}
			}
		}

		GuardStates.Add(State);
	}

	SetPlayerCinematicMode(true);
	SetActorTickEnabled(true);
	UpdateGuards();
	UpdateCamera();

	UE_LOG(LogTemp, Log, TEXT("Reinforcement cinematic started with %d guards."),
		GuardStates.Num());
}

void AKTReinforcementCinematicDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsPlaying)
	{
		return;
	}

	ElapsedTime += DeltaSeconds;
	UpdateGuards();
	UpdateCamera();

	if (ElapsedTime >= CinematicDuration)
	{
		BeginFinishTransition();
	}
}

void AKTReinforcementCinematicDirector::UpdateGuards()
{
	// Keep every guard advancing through the final camera fade. They are removed
	// while the frame is black, before any guard reaches the end and stands still.
	const float MovementEndTime = FMath::Max(
		0.5f,
		CinematicDuration + CameraFadeDuration + 0.35f);

	for (int32 GuardIndex = 0; GuardIndex < GuardStates.Num(); ++GuardIndex)
	{
		FCinematicGuardState& State = GuardStates[GuardIndex];
		AActor* GuardActor = State.Actor.Get();
		if (!GuardActor)
		{
			continue;
		}

		const FVector Direction = (State.End - State.Start).GetSafeNormal2D();
		const float RouteLength = FVector::Dist2D(State.Start, State.End);
		const float BaseSpeed = RouteLength / MovementEndTime;
		const float ActiveRunTime = FMath::Max(
			0.0f,
			ElapsedTime - State.StartDelay);
		const FVector Position = State.Start +
			Direction * BaseSpeed * State.SpeedMultiplier * ActiveRunTime;
		const FRotator GuardRotation =
			Direction.Rotation() + FRotator(0.0f, GuardVisualYawOffset, 0.0f);
		GuardActor->SetActorLocationAndRotation(
			Position,
			GuardRotation,
			false,
			nullptr,
			ETeleportType::TeleportPhysics);
	}
}

void AKTReinforcementCinematicDirector::UpdateCamera()
{
	if (!CinematicCamera || !PlayerController)
	{
		return;
	}

	const float NormalizedTime = FMath::Clamp(
		ElapsedTime / FMath::Max(CinematicDuration, 0.1f),
		0.0f,
		1.0f);
	const FVector FormationCenter = FMath::Lerp(RunStart, RunEnd, NormalizedTime);

	FVector CameraLocation;
	FVector LookTarget;
	float FieldOfView = 50.0f;

	if (NormalizedTime < 0.34f)
	{
		const float ShotAlpha = FMath::InterpEaseInOut(
			0.0f, 1.0f, NormalizedTime / 0.34f, 2.0f);
		CameraLocation = FMath::Lerp(
			FVector(-11450.0f, 6350.0f, 1120.0f),
			FVector(-11050.0f, 5950.0f, 850.0f),
			ShotAlpha);
		LookTarget = FormationCenter + FVector(150.0f, 0.0f, 110.0f);
		FieldOfView = FMath::Lerp(55.0f, 48.0f, ShotAlpha);
	}
	else if (NormalizedTime < 0.72f)
	{
		const float ShotAlpha = (NormalizedTime - 0.34f) / 0.38f;
		CameraLocation = FMath::Lerp(
			FVector(-11050.0f, 6200.0f, 1250.0f),
			FVector(-10250.0f, 6000.0f, 1100.0f),
			ShotAlpha);
		LookTarget = FormationCenter + FVector(100.0f, 0.0f, 80.0f);
		FieldOfView = 48.0f;
	}
	else
	{
		const float ShotAlpha = (NormalizedTime - 0.72f) / 0.28f;
		CameraLocation = FMath::Lerp(
			FVector(-9820.0f, 5580.0f, 870.0f),
			FVector(-9700.0f, 5750.0f, 980.0f),
			ShotAlpha);
		LookTarget = RunEnd + FVector(-180.0f, 0.0f, 110.0f);
		FieldOfView = FMath::Lerp(47.0f, 53.0f, ShotAlpha);
	}

	CinematicCamera->SetActorLocationAndRotation(
		CameraLocation,
		MakeLookAtRotation(CameraLocation, LookTarget));
	CinematicCamera->GetCineCameraComponent()->SetFieldOfView(FieldOfView);
}

void AKTReinforcementCinematicDirector::BeginFinishTransition()
{
	if (bIsFinishing)
	{
		return;
	}

	bIsFinishing = true;

	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		PlayerController->PlayerCameraManager->StartCameraFade(
			0.0f,
			1.0f,
			CameraFadeDuration,
			FLinearColor::Black,
			false,
			true);
	}

	GetWorldTimerManager().SetTimer(
		FinishTransitionTimer,
		this,
		&AKTReinforcementCinematicDirector::FinishCinematic,
		CameraFadeDuration,
		false);
}

void AKTReinforcementCinematicDirector::FinishCinematic()
{
	CleanupCinematicActors();

	if (PlayerController)
	{
		if (PreviousViewTarget)
		{
			PlayerController->SetViewTarget(PreviousViewTarget);
		}

		SetPlayerCinematicMode(false);

		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->StartCameraFade(
				1.0f,
				0.0f,
				0.35f,
				FLinearColor::Black,
				false,
				false);
		}
	}

	bIsPlaying = false;
	bIsFinishing = false;

	if (CompletionTag.IsValid())
	{
		if (UKTMissionSubsystem* MissionSubsystem =
			UKTMissionSubsystem::Get(this))
		{
			MissionSubsystem->BroadcastMissionEvent(
				CompletionTag,
				TriggerInstigator);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Reinforcement cinematic finished."));
}

void AKTReinforcementCinematicDirector::CleanupCinematicActors()
{
	for (FCinematicGuardState& State : GuardStates)
	{
		if (AActor* GuardActor = State.Actor.Get())
		{
			GuardActor->Destroy();
		}
	}
	GuardStates.Reset();

	if (CinematicCamera)
	{
		CinematicCamera->Destroy();
		CinematicCamera = nullptr;
	}
}

void AKTReinforcementCinematicDirector::SetPlayerCinematicMode(bool bEnabled)
{
	if (!PlayerController)
	{
		return;
	}

	PlayerController->SetCinematicMode(
		bEnabled,
		false,
		true,
		true,
		true);

	// 시네마틱 중에는 UI 숨김, 종료되면 다시 표시
	if (AKittyPlayerController* KittyPlayerController =
		Cast<AKittyPlayerController>(PlayerController))
	{
		KittyPlayerController->SetGameplayUIVisible(!bEnabled);
	}
	
	if (bEnabled && CinematicCamera)
	{
		PlayerController->SetViewTarget(CinematicCamera);
	}
}

FRotator AKTReinforcementCinematicDirector::MakeLookAtRotation(
	const FVector& From,
	const FVector& To) const
{
	return (To - From).Rotation();
}
