// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowDecalComponent.h"

#include "Utils.h"

UShadowDecalComponent::UShadowDecalComponent()
{
    static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Game/Lemonade/Blueprints/M_ShadowDecal"));
    if (MaterialFinder.Succeeded())
    {
        SetMaterial(0, MaterialFinder.Object);
    }

    SetRelativeRotation(FRotator(-90, 0, 0));
    DecalSize = FVector{ 180.0, DecalDimensions.X, DecalDimensions.Y };
    SetShadowSizeAndOpacity(180.f);

    PrimaryComponentTick.bCanEverTick = true;
}

void UShadowDecalComponent::BeginPlay()
{
    Super::BeginPlay();

    ShadowMID = CreateDynamicMaterialInstance();
    SetMaterial(0, ShadowMID);
}


void UShadowDecalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    USceneComponent* Parent = GetAttachParent();

    if (Owner && Parent)
    {
        FVector LineTraceStart = Parent->GetComponentLocation();
        FVector LineTraceEnd = LineTraceStart + FVector{ 0, 0, -1000 };
        ECollisionChannel TraceChannel = ECC_Visibility;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(Owner);

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
}

void UShadowDecalComponent::SetShadowSizeAndOpacity(float DistanceFromGround)
{
    auto ClampedDistance = std::max(DistanceFromGround, MinGroundDistance);

    DecalSize.X = ClampedDistance;
    SetRelativeLocation({ 0, 0, -ClampedDistance / 2.f });
    float LerpAlpha = InverseLerpClamped(MinGroundDistance, MaxGroundDistance, ClampedDistance);
    ScaleShadowDecal(FMath::Lerp(1.f, MaxShadowScale, LerpAlpha));

    if (ShadowMID)
        ShadowMID->SetScalarParameterValue("OpacityScale",
            FMath::Lerp(1.f, MinShadowOpacity, LerpAlpha));
}

void UShadowDecalComponent::ScaleShadowDecal(float scale)
{
    auto ScaledDimensions = scale * DecalDimensions;

    DecalSize.Y = ScaledDimensions.X;
    DecalSize.Z = ScaledDimensions.Y;
}