// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Lemonade2DCharacter.generated.h"

class ASideScrollingCamera;

/**
 * Base class for all 2D characters.
 * Contains useful common functionality.
 */
UCLASS()
class LEMONADE_API ALemonade2DCharacter : public APaperCharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	ASideScrollingCamera* SideScrollingCamera;

	// Rotates the velocity so that 
	// +Y always points in the "forward" direction
	UFUNCTION(BlueprintCallable)
	FVector GetCameraRotatedVelocity() const;

	UFUNCTION(BlueprintCallable)
	FVector GetCameraRotatedVector(FVector In) const;

private:
	ASideScrollingCamera* FindSideScrollingCamera();

	
};
