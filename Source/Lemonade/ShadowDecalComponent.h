// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "ShadowDecalComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEMONADE_API UShadowDecalComponent : public UDecalComponent
{
	GENERATED_BODY()
	

public:
    UShadowDecalComponent();

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

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void SetShadowSizeAndOpacity(float DistanceFromGround);
    void ScaleShadowDecal(float scale);

    UPROPERTY()
    UMaterialInstanceDynamic* ShadowMID;


};
