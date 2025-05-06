// Fill out your copyright notice in the Description page of Project Settings.


#include "SideScrollingCamera.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Containers/Ticker.h"
#include "Utils.h"

ASideScrollingCamera::ASideScrollingCamera()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;
    CameraZOffset = 400.f;
}

void ASideScrollingCamera::BeginPlay()
{
    Super::BeginPlay();


    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (Player != nullptr)
    {
        FVector PlayerLocation = Player->GetActorLocation();
        CameraX = PlayerLocation.X - CameraHorizontalOffset;
    }
}

APlayableCharacter* ASideScrollingCamera::FindPlayableCharacter()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);

    return Cast<APlayableCharacter>(Player);
}

void ASideScrollingCamera::StartRotationAnimation(FVector inRotationRoot,
    FVector inRotationStartHandle,
    FVector inRotationEndHandle,
    FRotator inStartRotation,
    FRotator inEndRotation)
{
    AnimationStartTime = GetWorld()->GetUnpausedTimeSeconds();
    RotationRoot = inRotationRoot;
    RotationStartHandle = inRotationStartHandle;
    RotationEndHandle = inRotationEndHandle;
    RotationStartQuat = inStartRotation.Quaternion();
    RotationEndQuat = inEndRotation.Quaternion();
}

bool ASideScrollingCamera::IsAnimating(double CurrentTime)
{
    return CurrentTime <= (AnimationStartTime + AnimationDuration);
}

void ASideScrollingCamera::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    TickRotationAnimation();
}

bool ASideScrollingCamera::TickRotationAnimation()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (Player != nullptr)
    {
        FVector PlayerLocation = Player->GetActorLocation();

        double CurrentTime = GetWorld()->GetUnpausedTimeSeconds();
        if (IsAnimating(CurrentTime))
        {
            float Alpha = AnimationAlpha(CurrentTime);
            FVector AnimatedHandle
                = SlerpAndScaleVector(RotationStartHandle,
                    RotationEndHandle,
                    Alpha);

            RootComponent->SetWorldLocation(RotationRoot + AnimatedHandle);
            RootComponent->SetWorldRotation(FQuat::Slerp(
                RotationStartQuat,
                RotationEndQuat,
                Alpha));


            auto CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
            if (CameraManager)
            {
                CameraManager->UpdateCamera(0.0f);
            }

            return true;
        }
        else if (ViewDirection == ViewDirections::PositiveX)
        {
            RootComponent->SetWorldLocation(
                GetTargetLocation(PlayerLocation, ViewDirection)
            );
        }
        else if (ViewDirection == ViewDirections::PositiveY)
        {
            RootComponent->SetWorldLocation(
                GetTargetLocation(PlayerLocation, ViewDirection)
            );
        }
    }

    return false;
}

FVector ASideScrollingCamera::GetTargetLocation(FVector PlayerLocation,
    ViewDirections Direction)
{
    if (Direction == ViewDirections::PositiveY)
    {
        return {
                   PlayerLocation.X,
                   CameraY,
                   PlayerLocation.Z + CameraZOffset
        };
    }

    return {
            CameraX,
            PlayerLocation.Y,
            PlayerLocation.Z + CameraZOffset
    };
}

void ASideScrollingCamera::SetViewDirection(
    ViewDirections Direction,
    float PositionAlongAxis,
    bool bShouldAnimate)
{
    if (Direction == ViewDirection) return;

    double CurrentTime = GetWorld()->GetUnpausedTimeSeconds();
    if (bShouldAnimate && IsAnimating(CurrentTime)) return;

    auto Rotation = GetActorRotation();

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (Player == nullptr) return;
    FVector CameraLocation = GetActorLocation();
    FVector PlayerLocation = Player->GetActorLocation();

    if (Direction == ViewDirections::PositiveY)
    {
        CameraY = PositionAlongAxis;
        ViewDirection = Direction;

        if (bShouldAnimate)
        {
            FVector TargetLocation
                = GetTargetLocation(PlayerLocation, Direction);

            StartRotationAnimation(PlayerLocation,
                CameraLocation - PlayerLocation,
                TargetLocation - PlayerLocation,
                Rotation,
                FRotator(Rotation.Pitch, 90, Rotation.Roll));
        }
        else
        {
            RootComponent->SetWorldRotation(FRotator(Rotation.Pitch, 90, Rotation.Roll));
        }

        return;
    }

    if (Direction == ViewDirections::PositiveX)
    {
        CameraX = PositionAlongAxis;
        ViewDirection = Direction;

        if (bShouldAnimate)
        {
            FVector TargetLocation
                = GetTargetLocation(PlayerLocation, Direction);

            StartRotationAnimation(PlayerLocation,
                CameraLocation - PlayerLocation,
                TargetLocation - PlayerLocation,
                Rotation,
                FRotator(Rotation.Pitch, 0, Rotation.Roll));
        }
        else
        {
            RootComponent->SetWorldRotation(FRotator(Rotation.Pitch, 0, Rotation.Roll));
        }

        return;
    }
}

void ASideScrollingCamera::ToggleLookDirection()
{
    ToggleLookDirectionAndRotatePlayer(CameraHorizontalOffset, CameraHorizontalOffset);
}

void ASideScrollingCamera::ForceSetCameraState(ViewDirections inViewDirection,
    float CameraOffsetX, float CameraOffsetY)
{
    CameraX = CameraOffsetX;
    CameraY = CameraOffsetY;

    SetViewDirection(inViewDirection,
        inViewDirection == ViewDirections::PositiveX ? CameraX : CameraY,
        false
    );

    APlayableCharacter* Player = Cast<APlayableCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (Player != nullptr)
    {
        bool MovingAlongY = inViewDirection == ViewDirections::PositiveX;

        Player->SetMovementDirection(MovingAlongY);
    }
}

void ASideScrollingCamera::ToggleLookDirectionWithOffsets(float CameraOffsetX, float CameraOffsetY)
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (Player != nullptr)
    {
        FVector PlayerLocation = Player->GetActorLocation();

        if (ViewDirection == ViewDirections::PositiveX)
        {
            SetViewDirection(ViewDirections::PositiveY, PlayerLocation.Y - CameraOffsetY);
        }
        else if (ViewDirection == ViewDirections::PositiveY)
        {
            SetViewDirection(ViewDirections::PositiveX, PlayerLocation.X - CameraOffsetX);
        }
    }
}

void ASideScrollingCamera::ToggleLookDirectionAndRotatePlayer(float CameraOffsetX, float CameraOffsetY)
{
    ToggleLookDirectionWithOffsets(CameraOffsetX, CameraOffsetY);
    APlayableCharacter* Player = FindPlayableCharacter();
    APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (Player != nullptr && Controller != nullptr)
    {
        Player->ToggleRotation();
        UGameplayStatics::SetGamePaused(GetWorld(), true);

        double TimerEnd = GetWorld()->GetUnpausedTimeSeconds() + AnimationDuration;
        FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateLambda([this, TimerEnd](float DeltaTime)
                {
                    if (GetWorld()->GetUnpausedTimeSeconds() >= TimerEnd)
                    {
                        UGameplayStatics::SetGamePaused(GetWorld(), false);
                        return false;
                    }

                    return true;
                }));

    }
}

float ASideScrollingCamera::AnimationAlpha(double CurrentTime)
{
    return FMath::InterpEaseInOut(0.f, 1.f, InverseLerpClamped(AnimationStartTime,
        AnimationStartTime + AnimationDuration,
        CurrentTime),
        2.f);
}

float ASideScrollingCamera::GetParallaxOffset()
{
    auto Location = GetActorLocation();
    double CurrentTime = GetWorld()->GetUnpausedTimeSeconds();

    if (IsAnimating(CurrentTime))
    {
        float Alpha = AnimationAlpha(CurrentTime);

        if (ViewDirection == ViewDirections::PositiveX)
        {
            // Rotating from Y to X
            return FMath::Lerp(-Location.X, Location.Y, Alpha);
        }

        // Rotating from X to Y
        return FMath::Lerp(Location.Y, -Location.X, Alpha);
    }

    if (ViewDirection == ViewDirections::PositiveX)
    {
        return Location.Y;
    }

    return -Location.X;
}

bool ASideScrollingCamera::IsLookingDownPositiveX() const
{
    return ViewDirection == ViewDirections::PositiveX;
}

float ASideScrollingCamera::GetCameraX() const
{
    return CameraX;
}

float ASideScrollingCamera::GetCameraY() const
{
    return CameraY;
}      

ViewDirections ASideScrollingCamera::GetViewDirection() const
{
    return ViewDirection;
}