// Fill out your copyright notice in the Description page of Project Settings.


#include "ParallaxLayerComponent.h"

UParallaxLayerComponent::UParallaxLayerComponent()
{
    static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Game/Lemonade/Environment/M_Parallax"));
    if (MaterialFinder.Succeeded())
    {
        SetMaterial(0, MaterialFinder.Object);
    }
}

void UParallaxLayerComponent::BeginPlay()
{
    Super::BeginPlay();

    MID = CreateDynamicMaterialInstance(0);
    MID->SetScalarParameterValue("ParallaxSpeed", ParallaxSpeed);
}

void UParallaxLayerComponent::SetParallaxOffset(float Offset)
{
    MID->SetScalarParameterValue("ParallaxOffset", Offset);
}