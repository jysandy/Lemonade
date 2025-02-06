// Fill out your copyright notice in the Description page of Project Settings.


#include "ParallaxBackground.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AParallaxBackground::AParallaxBackground()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootSceneComp
        = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

    StaticBG
        = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("StaticBG"));
    Layer1
        = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Layer1"));
    Layer2
        = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Layer2"));
    Layer3
        = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Layer3"));
    Layer4
        = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Layer4"));
    Layer5
        = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Layer5"));

    RootComponent = RootSceneComp;

    StaticBG->SetupAttachment(RootComponent);

    Layer1->SetupAttachment(StaticBG);
    Layer2->SetupAttachment(StaticBG);
    Layer3->SetupAttachment(StaticBG);
    Layer4->SetupAttachment(StaticBG);
    Layer5->SetupAttachment(StaticBG);

    ParallaxSpeeds.Add(5.f);
    ParallaxSpeeds.Add(3.f);
    ParallaxSpeeds.Add(1.5f);
    ParallaxSpeeds.Add(0.8f);
    ParallaxSpeeds.Add(0.5f);

    RootComponent->SetWorldRotation(FQuat(FVector::ZAxisVector, PI / 2.f));

    CameraXOffset = 2500.f;
    CameraZOffset = -75.f;
}

// Called when the game starts or when spawned
void AParallaxBackground::BeginPlay()
{
    Super::BeginPlay();

    TArray<USceneComponent*> layers;
    StaticBG->GetChildrenComponents(false, layers);

    for (int i = 0; i < layers.Num(); i++)
    {
        UPaperSpriteComponent* layer = Cast<UPaperSpriteComponent>(layers[i]);
        check(layer);
        auto mid = layer->CreateDynamicMaterialInstance(0);
        mid->SetScalarParameterValue("ParallaxSpeed", ParallaxSpeeds[i]);
        layer->SetMaterial(0, mid);
        LayerMIDs.Add(mid);
    }

    APlayerCameraManager* camManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    check(camManager);
    AActor* cameraActor = camManager->GetViewTarget();
    check(cameraActor);

    FVector cameraLocation = camManager->GetCameraLocation();

    RootComponent->SetWorldLocation({
    cameraLocation.X + CameraXOffset,
    cameraLocation.Y,
    cameraLocation.Z + CameraZOffset
        });

    AttachToActor(cameraActor, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called every frame
void AParallaxBackground::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector cameraLocation
        = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();

    for (auto& mid : LayerMIDs)
    {
        mid->SetVectorParameterValue("CameraLocation", cameraLocation);
    }
}

