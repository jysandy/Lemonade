// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
#include "SideScrollingCamera.h"
#include "ParallaxLayerComponent.h"
#include "ParallaxBackground.generated.h"


UCLASS()
class LEMONADE_API AParallaxBackground : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParallaxBackground();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ToggleVisibility();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* StaticBG;

	// The distance of the background from the camera.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceFromCamera;

private:
	AActor* FindSideScrollingCamera();

	UPROPERTY()
	ASideScrollingCamera* SideScrollingCamera;

	// The scale factor needed to make the background fit a 
	// camera vertically.
	float ScaleFactor(ACameraActor* cameraActor, float distance);

};
