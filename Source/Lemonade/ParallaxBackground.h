// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* StaticBG;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* Layer1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* Layer2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* Layer3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* Layer4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* Layer5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> ParallaxSpeeds;

	// The distance of the background from the camera.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceFromCamera;

private:
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> LayerMIDs;

	AActor* FindSideScrollingCamera();

	// The scale factor needed to make the background fit a 
	// camera vertically.
	float ScaleFactor(ACameraActor* cameraActor, float distance);

};
