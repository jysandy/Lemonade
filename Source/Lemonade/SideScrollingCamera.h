// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "PlayableCharacter.h"
#include "SideScrollingCamera.generated.h"

UENUM(BlueprintType)
enum class ViewDirections : uint8
{
    PositiveX UMETA(DisplayName = "Positive X"),
    PositiveY UMETA(DisplayName = "Positive Y")
};

/**
 *
 */
UCLASS()
class LEMONADE_API ASideScrollingCamera : public ACameraActor
{
    GENERATED_BODY()

public:
    ASideScrollingCamera();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CameraZOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CameraHorizontalOffset = 1680.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AnimationDuration = 0.5f;

    UFUNCTION(BlueprintCallable)
    ViewDirections GetViewDirection() const;

    void SetViewDirection(ViewDirections ViewDirection,
        float PositionAlongAxis,
        bool bShouldAnimate = true);

    // Toggles about the current player position.
    UFUNCTION(BlueprintCallable)
    void ToggleLookDirection();

    UFUNCTION(BlueprintCallable)
    void ToggleLookDirectionWithOffsets(float CameraOffsetX, float CameraOffsetY);

    UFUNCTION(BlueprintCallable)
    void ToggleLookDirectionAndRotatePlayer(float CameraOffsetX, float CameraOffsetY);

    // Sets camera and player state without any animation.
    UFUNCTION(BlueprintCallable)
    void ForceSetCameraState(ViewDirections ViewDirection,
        float CameraOffsetX, float CameraOffsetY);

    UFUNCTION(BlueprintCallable)
    float GetParallaxOffset();

    UFUNCTION(BlueprintCallable)
    bool IsLookingDownPositiveX() const;
    
    float GetCameraX() const;
    float GetCameraY() const;

private:
    ViewDirections ViewDirection
        = ViewDirections::PositiveX;
    float CameraX = -700.f;
    float CameraY = 0.f;

    // Animation state
    FVector RotationRoot;
    FVector RotationStartHandle;
    FVector RotationEndHandle;
    FQuat RotationStartQuat;
    FQuat RotationEndQuat;
    double AnimationStartTime = -5.f;

    void StartRotationAnimation(FVector RotationRoot,
        FVector RotationStartHandle,
        FVector RotationEndHandle,
        FRotator inStartRotation,
        FRotator inEndRotation);

    bool IsAnimating(double CurrentTime);
    float AnimationAlpha(double CurrentTime);
    bool TickRotationAnimation();
    FVector GetTargetLocation(FVector PlayerLocation, ViewDirections Direction);

    APlayableCharacter* FindPlayableCharacter();
};
