// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LemonadeGameInstance.h"
#include "Utils.h"

#include <algorithm>

DEFINE_LOG_CATEGORY(LogPlayableCharacter);

APlayableCharacter::APlayableCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    Shadow = CreateDefaultSubobject<UDecalComponent>(TEXT("Shadow"));
    Shadow->SetupAttachment(RootComponent);
    Shadow->SetRelativeRotation(FQuat(FVector::UnitY(), -PI / 2.0));
    Shadow->DecalSize = FVector{ 180.0, DecalDimensions.X, DecalDimensions.Y };
    SetShadowSizeAndOpacity(180.f);

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APlayableCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    DefaultGravityScale = GetCharacterMovement()->GravityScale;
    ShadowMID = Shadow->CreateDynamicMaterialInstance();
    Shadow->SetMaterial(0, ShadowMID);
    SetMovementDirection(true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Add Input Mapping Context
    if (APlayerController* PlayerController
        = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem
            = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent
        = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

        // Jumping
        EnhancedInputComponent->BindAction(JumpAction,
            ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction,
            ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction,
            ETriggerEvent::Triggered, this, &APlayableCharacter::Move);

        // Dash
        EnhancedInputComponent->BindAction(DashAction,
            ETriggerEvent::Started, this, &APlayableCharacter::Dash);

        // Attack
        EnhancedInputComponent->BindAction(AttackAction,
            ETriggerEvent::Started, this, &APlayableCharacter::Attack);

        // Interact
        EnhancedInputComponent->BindAction(InteractAction,
            ETriggerEvent::Started, this, &APlayableCharacter::Interact);
    }
    else
    {
        UE_LOG(LogPlayableCharacter, Error,
            TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void APlayableCharacter::Move(const FInputActionValue& Value)
{
    // Don't move while attacking
    if (bAttacking) return;

    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement 
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void APlayableCharacter::Dash(const FInputActionValue& Value)
{
    if (bDashOnCooldown) return;
    if (bDashing) return; // Prevent multiple dashes at once
    if (bAttacking) return; // Can't dash while attacking

    bDashing = true;
    bDashOnCooldown = true;

    FTimerHandle DashCooldownTimerHandle;
    // TODO: Read from this in the blueprint
    GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, [this]()
        {
            bDashOnCooldown = false;
        },
        DashCooldown, false);

    // Save the gravity scale since it may have been set 
    // in the editor.
    DefaultGravityScale = GetCharacterMovement()->GravityScale;
    GetCharacterMovement()->GravityScale = 0.0f;

    FVector DashDirection = GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal();

    if (DashDirection.IsNearlyZero())
    {
        DashDirection = GetActorForwardVector();
    }

    LaunchCharacter(DashDirection * DashPower, true, true);

    // Set a timer to end the dash
    GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayableCharacter::StopDashing, DashLength, false);

}

void APlayableCharacter::StopDashing()
{
    // Re-enable gravity & Reset boolean
    GetCharacterMovement()->GravityScale = DefaultGravityScale;
    bDashing = false;
}

void APlayableCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    auto velocity = GetCameraRotatedVelocity();

    if (velocity.Y > 0)
    {
        FacingRight = true;
    }
    else if (velocity.Y < 0)
    {
        FacingRight = false;
    }

    UpdateFlipbook();
    UpdateShadowDecal();
}

void APlayableCharacter::ScaleShadowDecal(float scale)
{
    auto ScaledDimensions = scale * DecalDimensions;

    Shadow->DecalSize.Y = ScaledDimensions.X;
    Shadow->DecalSize.Z = ScaledDimensions.Y;
}

void APlayableCharacter::SetShadowSizeAndOpacity(float DistanceFromGround)
{
    auto ClampedDistance = std::max(DistanceFromGround, MinGroundDistance);

    Shadow->DecalSize.X = ClampedDistance;
    Shadow->SetRelativeLocation({ 0, 0, -ClampedDistance / 2.f });
    float LerpAlpha = InverseLerpClamped(MinGroundDistance, MaxGroundDistance, ClampedDistance);
    ScaleShadowDecal(FMath::Lerp(1.f, MaxShadowScale, LerpAlpha));

    if (ShadowMID)
        ShadowMID->SetScalarParameterValue("OpacityScale",
            FMath::Lerp(1.f, MinShadowOpacity, LerpAlpha));
}

void APlayableCharacter::UpdateShadowDecal()
{
    FVector LineTraceStart = GetActorLocation();
    FVector LineTraceEnd = LineTraceStart + FVector{ 0, 0, -1000 };
    ECollisionChannel TraceChannel = ECC_Visibility;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    FHitResult HitResult;
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, TraceChannel, QueryParams);

    if (bHit)
    {
        SetShadowSizeAndOpacity(HitResult.Distance);
    }
    else
    {
        SetShadowSizeAndOpacity(180.f);
    }
}

void APlayableCharacter::UpdateFlipbook()
{
    auto flipbookComponent = this->GetSprite();

    auto velocity = GetCameraRotatedVelocity();

    if (bDashing)
    {
        SetDashingAnimation();
    }
    else if (bAttacking)
    {
        SetAttackingAnimation();
    }
    else if (GetMovementComponent()->IsFalling())
    {
        SetJumpingAnimation();
    }
    else if (velocity.Length() > 0)
    {
        SetRunningAnimation();
    }
    else
    {
        SetIdleAnimation();
    }
}

void APlayableCharacter::SetRunningAnimation()
{
    auto sprite = this->GetSprite();

    if (FacingRight)
        sprite->SetFlipbook(RunRightFlipbook);
    else
        sprite->SetFlipbook(RunLeftFlipbook);
}

void APlayableCharacter::SetIdleAnimation()
{
    auto sprite = this->GetSprite();

    if (FacingRight)
        sprite->SetFlipbook(IdleRightFlipbook);
    else
        sprite->SetFlipbook(IdleLeftFlipbook);
}

void APlayableCharacter::SetDashingAnimation()
{
    auto sprite = this->GetSprite();

    if (FacingRight)
        sprite->SetFlipbook(DashRightFlipbook);
    else
        sprite->SetFlipbook(DashLeftFlipbook);
}

void APlayableCharacter::SetAttackingAnimation()
{
    auto sprite = this->GetSprite();

    if (FacingRight)
        sprite->SetFlipbook(AttackRightFlipbook);
    else
        sprite->SetFlipbook(AttackLeftFlipbook);
}

void APlayableCharacter::SetJumpingAnimation()
{
    auto sprite = this->GetSprite();

    auto velocity = GetCameraRotatedVelocity();

    if (velocity.Z > 0)
    {
        // Jump takeoff

        if (FacingRight)
            sprite->SetFlipbook(JumpLiftoffRight);
        else
            sprite->SetFlipbook(JumpLiftoffLeft);
    }
    else if (velocity.Z < -10.f)
    {
        // Falling

        if (FacingRight)
            sprite->SetFlipbook(FallRight);
        else
            sprite->SetFlipbook(FallLeft);
    }
}

bool APlayableCharacter::IsFacingRight()
{
    return FacingRight;
}

void APlayableCharacter::ToggleRotation()
{
    SetMovementDirection(SideScrollingCamera->IsLookingDownPositiveX());
}

void APlayableCharacter::SetMovementDirection(bool inMovingAlongY)
{
    if (Controller != nullptr)
    {
        const FRotator ControllerRotation = Controller->GetControlRotation();

        if (inMovingAlongY)
        {
            Controller->SetControlRotation(FRotator(ControllerRotation.Pitch,
                0,
                ControllerRotation.Roll));
        }
        else
        {
            Controller->SetControlRotation(FRotator(ControllerRotation.Pitch,
                90,
                ControllerRotation.Roll));
        }
    }
}

void APlayableCharacter::Kill()
{
    Destroy();

    if (UWorld* World = GetWorld())
    {
        if (ULemonadeGameInstance* GameInstance = Cast<ULemonadeGameInstance>(UGameplayStatics::GetGameInstance(World)))
        {
            GameInstance->PlayerDied();
        }
    }
}

void APlayableCharacter::FellOutOfWorld(const UDamageType& DmgType)
{
    Super::FellOutOfWorld(DmgType);

    Kill();
}

void APlayableCharacter::Interact(const FInputActionValue& Value)
{
    UInteractComponent* Interactible = GetClosestInteractible();

    if (Interactible != nullptr)
    {
        Interactible->TriggerInteractEvent();
    }
}

UInteractComponent* APlayableCharacter::GetClosestInteractible()
{
    TSet<UPrimitiveComponent*> OverlappingComponents;

    GetOverlappingComponents(OverlappingComponents);

    UInteractComponent* NearestInteractComponent = nullptr;
    float NearestDistSquared = 1000000.f;

    FVector PlayerLocation = GetActorLocation();

    for (auto& Component : OverlappingComponents)
    {
        if (UInteractComponent* Interactible = Cast<UInteractComponent>(Component))
        {
            if (float DistSquared = FVector::DistSquared(PlayerLocation,
                Interactible->GetComponentLocation()); DistSquared < NearestDistSquared)
            {
                NearestInteractComponent = Interactible;
                NearestDistSquared = DistSquared;
            }
        }
    }

    return NearestInteractComponent;
}

void APlayableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
    auto Interactible = OtherActor->FindComponentByClass<UInteractComponent>();

    if (Interactible)
    {
        Interactible->TriggerEnterEvent();
    }
}

void APlayableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
    auto Interactible = OtherActor->FindComponentByClass<UInteractComponent>();

    if (Interactible)
    {
        Interactible->TriggerExitEvent();
    }
}
