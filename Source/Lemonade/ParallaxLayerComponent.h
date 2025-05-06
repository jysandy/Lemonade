// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "ParallaxLayerComponent.generated.h"

/**
 *
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class LEMONADE_API UParallaxLayerComponent : public UPaperSpriteComponent
{
    GENERATED_BODY()


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ParallaxSpeed = 0.3f;

    UParallaxLayerComponent();

    void SetParallaxOffset(float Offset);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UMaterialInstanceDynamic* MID;
};
