// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "SideScrollingCamera.generated.h"

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

private:
	float CameraX = -700.f;
	
};
