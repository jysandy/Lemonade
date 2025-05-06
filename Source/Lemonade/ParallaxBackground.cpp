// Fill out your copyright notice in the Description page of Project Settings.


#include "ParallaxBackground.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PaperSprite.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

// Sets default values
AParallaxBackground::AParallaxBackground()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;

    USceneComponent* RootSceneComp
        = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    RootComponent = RootSceneComp;

    StaticBG
        = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("StaticBG"));
    StaticBG->SetupAttachment(RootComponent);

    StaticBG->SetRelativeRotation(FRotator(0, 90, 0));

    DistanceFromCamera = 10000.f;
}

AActor* AParallaxBackground::FindSideScrollingCamera()
{
    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(),
        TEXT("BP_SideScrollingCamera"),
        ActorsWithTag);

    for (AActor* Actor : ActorsWithTag)
    {
        if (Actor)
        {
            return Actor;
        }
    }

    return nullptr;
}

// Called when the game starts or when spawned
void AParallaxBackground::BeginPlay()
{
    Super::BeginPlay();

    ASideScrollingCamera* cameraActor = Cast<ASideScrollingCamera>(FindSideScrollingCamera());
    check(cameraActor);

    AttachToActor(cameraActor, FAttachmentTransformRules::KeepRelativeTransform);

    SideScrollingCamera = cameraActor;

    // Position, rotate and scale the background 
    // so that it fills the screen.
    RootComponent->SetRelativeLocation({ DistanceFromCamera, 0, 0 });
    RootComponent->SetRelativeRotation(FRotator());

    FTimerHandle ScaleTimer;
    GetWorldTimerManager().SetTimer(ScaleTimer,
        [this]()
        {
            // Run this after a delay because the viewport dimensions 
            // aren't immediately available.
            float scale = ScaleFactor(SideScrollingCamera, DistanceFromCamera);
            StaticBG->SetRelativeScale3D({ scale, 1.f, scale });
        },
        0.2f,
        false,
        0.2f);
}

float AParallaxBackground::ScaleFactor(ACameraActor* cameraActor, float distance)
{
    UCameraComponent* cameraComponent = cameraActor->GetCameraComponent();

    auto sprite = StaticBG->GetSprite();
    auto bounds = StaticBG->GetLocalBounds();


    auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    
    int32 ViewportWidth, ViewportHeight;
    PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);

    float ViewportAspectRatio = static_cast<float>(ViewportWidth) / static_cast<float>(ViewportHeight);

    float BgAspectRatio = bounds.BoxExtent.X / bounds.BoxExtent.Z;

    float requiredZExtent = DistanceFromCamera
        * FMath::Tan(FMath::DegreesToRadians(cameraComponent->FieldOfView) / 2.f);

    if (BgAspectRatio >= ViewportAspectRatio)
    {
        float scaleFactor = requiredZExtent
            / (bounds.BoxExtent.Z * ViewportAspectRatio);

        return scaleFactor;
    }

    float requiredXExtent = requiredZExtent 
        * ViewportAspectRatio;
    float scaleFactor = requiredXExtent / bounds.BoxExtent.X;
    return scaleFactor;
}

// Called every frame
void AParallaxBackground::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float ParallaxOffset = SideScrollingCamera->GetParallaxOffset();

    TArray<USceneComponent*> layers;
    StaticBG->GetChildrenComponents(false, layers);

    for (int i = 0; i < layers.Num(); i++)
    {
        UParallaxLayerComponent* layer = Cast<UParallaxLayerComponent>(layers[i]);
        if (layer == nullptr) continue;
        
        layer->SetParallaxOffset(ParallaxOffset);
    }
}

void AParallaxBackground::ToggleVisibility()
{

    TArray<USceneComponent*> layers;
    StaticBG->GetChildrenComponents(false, layers);
    for (int i = 0; i < layers.Num(); i++)
    {
        UParallaxLayerComponent* layer = Cast<UParallaxLayerComponent>(layers[i]);
        if (layer == nullptr) continue;

        layer->ToggleVisibility();
    }
    StaticBG->ToggleVisibility();
}

