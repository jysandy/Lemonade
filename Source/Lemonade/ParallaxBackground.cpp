// Fill out your copyright notice in the Description page of Project Settings.


#include "ParallaxBackground.h"
#include "Kismet/GameplayStatics.h"
#include "PaperSprite.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

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

    ACameraActor* cameraActor = Cast<ACameraActor>(FindSideScrollingCamera());
    check(cameraActor);

    AttachToActor(cameraActor, FAttachmentTransformRules::KeepRelativeTransform);

    // Position, rotate and scale the background 
    // so that it fills the screen.
    RootComponent->SetRelativeLocation({ DistanceFromCamera, 0, 0 });
    RootComponent->SetRelativeRotation(FRotator());

    float scale = ScaleFactor(cameraActor, DistanceFromCamera);

    StaticBG->SetRelativeScale3D({ scale, 1.f, scale });
}

float AParallaxBackground::ScaleFactor(ACameraActor* cameraActor, float distance)
{
    UCameraComponent* cameraComponent = cameraActor->GetCameraComponent();

    auto sprite = StaticBG->GetSprite();
    auto bounds = StaticBG->GetLocalBounds();

    float requiredZExtent = DistanceFromCamera
        * FMath::Tan(FMath::DegreesToRadians(cameraComponent->FieldOfView) / 2.f);

    float scaleFactor = requiredZExtent
        / (bounds.BoxExtent.Z * cameraComponent->AspectRatio);

    return scaleFactor;
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

