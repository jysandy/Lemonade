// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbook.h"
#include "Logging/LogMacros.h"

#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "PlayableCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayableCharacter, Log, All);

UCLASS(config=Game)
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
	
public:
	APlayableCharacter();		 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animation)
	UPaperFlipbook* IdleRightFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animation)
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

	/** Called for movement input */
	void Move(const FInputActionValue& Value);


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsFacingRight();


private:
	void UpdateFlipbook();
	void SetRunningAnimation();
	void SetJumpingAnimation();
	void SetIdleAnimation();

	bool FacingRight = true;

};
