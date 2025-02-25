// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbook.h"
#include "Logging/LogMacros.h"

#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "PlayableCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayableCharacter, Log, All);

UCLASS(config = Game)
class LEMONADE_API APlayableCharacter : public APaperCharacter
{
    GENERATED_BODY()

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    /** Attack Input */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* AttackAction;

    /** Dash Input */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* DashAction;

public:
    APlayableCharacter();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShadowDecal)
    UDecalComponent* Shadow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShadowDecal)
    FVector2D DecalDimensions = { 44.8, 32.0 };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShadowDecal)
    float MinGroundDistance = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShadowDecal)
    float MaxGroundDistance = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShadowDecal)
    float MaxShadowScale = 1.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShadowDecal)
    float MinShadowOpacity = 0.4f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* IdleRightFlipbook;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* RunRightFlipbook;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* IdleLeftFlipbook;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* RunLeftFlipbook;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* JumpApexLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* JumpLiftoffLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* FallLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* JumpApexRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* JumpLiftoffRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UPaperFlipbook* FallRight;

protected:

    //Dash&Timer Variables
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    bool dashing = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float dashPower = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float dashLength = 0.3f;

    FTimerHandle DashTimerHandle;

    void StopDashing();

    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Call for Dash */
    void Dash(const FInputActionValue& Value);

    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    void Attack();

protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // To add mapping context
    virtual void BeginPlay();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    bool IsFacingRight();


private:
    void SetShadowSizeAndOpacity(float distanceFromGround);
    void ScaleShadowDecal(float scale);
    void UpdateShadowDecal();
    void UpdateFlipbook();
    void SetRunningAnimation();
    void SetJumpingAnimation();
    void SetIdleAnimation();

    bool FacingRight = true;

    UPROPERTY()
    UMaterialInstanceDynamic* ShadowMID;

};
