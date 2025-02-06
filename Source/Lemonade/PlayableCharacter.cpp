// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "PaperFlipbookComponent.h"

DEFINE_LOG_CATEGORY(LogPlayableCharacter);

APlayableCharacter::APlayableCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

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
    }
    else
    {
        UE_LOG(LogPlayableCharacter, Error,
            TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void APlayableCharacter::Move(const FInputActionValue& Value)
{
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

void APlayableCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // TODO: Transform the velocity into camera space.
    auto velocity = this->GetVelocity();

    if (velocity.Y > 0)
    {
        FacingRight = true;
    }
    else if (velocity.Y < 0)
    {
        FacingRight = false;
    }

    UpdateFlipbook();
}

void APlayableCharacter::UpdateFlipbook()
{
    auto flipbookComponent = this->GetSprite();

    // TODO: Transform the velocity into camera space.
    auto velocity = this->GetVelocity();

    if (GetMovementComponent()->IsFalling())
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

void APlayableCharacter::SetJumpingAnimation()
{
    auto sprite = this->GetSprite();

    // TODO: Transform the velocity into camera space.
    auto velocity = this->GetVelocity();

    if (velocity.Z > -200 && velocity.Z < 200)
    {
        // Apex

        if (FacingRight)
            sprite->SetFlipbook(JumpApexRight);
        else
            sprite->SetFlipbook(JumpApexLeft);
    }
    else if (velocity.Z > 0)
    {
        // Jump takeoff

        if (FacingRight)
            sprite->SetFlipbook(JumpLiftoffRight);
        else
            sprite->SetFlipbook(JumpLiftoffLeft);
    }
    else if (velocity.Z < 0)
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