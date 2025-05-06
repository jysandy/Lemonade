// Fill out your copyright notice in the Description page of Project Settings.


#include "LemonadeGameInstance.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LemonadeSaveGame.h"
#include "LemonadeLevelScriptActor.h"
#include "GameFramework/GameModeBase.h"

DEFINE_LOG_CATEGORY(LogLemonadeGameInstance);

void ULemonadeGameInstance::SaveGame(FVector PlayerLocation)
{
    USaveGame* SaveObject
        = UGameplayStatics::CreateSaveGameObject(ULemonadeSaveGame::StaticClass());
    if (ULemonadeSaveGame* SaveGameInstance = Cast<ULemonadeSaveGame>(SaveObject))
    {
        FAsyncSaveGameToSlotDelegate SavedDelegate;

        SavedDelegate.BindLambda([](const FString& SlotName,
            int32 UserIndex, bool SaveSucceeded)
            {
                if (GEngine)
                {
                    if (SaveSucceeded)
                    {
                        GEngine->AddOnScreenDebugMessage(-1,
                            2.f,
                            FColor::Green,
                            TEXT("Saved game successfully."));
                    }
                    else
                    {
                        GEngine->AddOnScreenDebugMessage(-1,
                            2.f,
                            FColor::Red,
                            TEXT("Saving the game failed!"));
                    }
                }
            });

        SaveGameInstance->PlayerLocation = PlayerLocation;
        SaveGameInstance->SetLevelData(LevelState);
        SaveGameInstance->LevelName = CurrentLevelName();

        ASideScrollingCamera* Camera = FindSideScrollingCamera();
        if (Camera)
        {
            SaveGameInstance->CameraLookingDownPositiveX = Camera->IsLookingDownPositiveX();
            SaveGameInstance->CameraX = Camera->GetCameraX();
            SaveGameInstance->CameraY = Camera->GetCameraY();
        }

        UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance,
            SaveGameInstance->SaveSlotName,
            SaveGameInstance->UserIndex,
            SavedDelegate);
    }
}

ALemonadeLevelScriptActor* ULemonadeGameInstance::GetLevelScriptActor()
{
    return Cast<ALemonadeLevelScriptActor>(
        GetWorld()->GetCurrentLevel()->GetLevelScriptActor());
}

ULemonadeSaveGame* ULemonadeGameInstance::LoadGame()
{
    if (ULemonadeSaveGame* LoadedGame
        = Cast<ULemonadeSaveGame>(UGameplayStatics::LoadGameFromSlot(ULemonadeSaveGame::DefaultSaveSlotName, 0)))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1,
                2.f,
                FColor::Green,
                TEXT("Loaded game successfully."));
        }

        return LoadedGame;
    }

    return nullptr;
}

void ULemonadeGameInstance::ClearSave()
{
    UGameplayStatics::DeleteGameInSlot(ULemonadeSaveGame::DefaultSaveSlotName, 0);
}

void ULemonadeGameInstance::PlayerDied()
{
    APlayerController* Controller
        = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    ULemonadeSaveGame* SaveInstance = LoadGame();
    if (SaveInstance != nullptr)
    {
        RespawnPlayer(SaveInstance);
    }
    else
    {
        AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());

        if (!GameMode)
        {
            return;
        }

        GameMode->RestartPlayer(Controller);
    }
}

void ULemonadeGameInstance::RespawnPlayer(ULemonadeSaveGame* SaveInstance, float Delay)
{
    if (SaveInstance->LevelName != CurrentLevelName())
    {
        UKismetSystemLibrary::PrintString(this,
            FString("Loading level: " + SaveInstance->LevelName));
        UGameplayStatics::OpenLevel(this, FName(SaveInstance->LevelName));
    }

    AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());

    if (!GameMode)
    {
        return;
    }

    LevelState = SaveInstance->GetLevelData();
    PassStateToLevel();

    // Restart the player.
    FTransform Transform;
    Transform.SetComponents(FQuat::Identity,
        SaveInstance->PlayerLocation,
        FVector::OneVector);

    auto CameraLookingDownPositiveX = SaveInstance->CameraLookingDownPositiveX;
    auto CameraX = SaveInstance->CameraX;
    auto CameraY = SaveInstance->CameraY;

    GetWorld()->GetTimerManager().SetTimer(RestartTimer,
        [=, this]()
        {
            // Poll until the PlayerController and camera are both available.

            APlayerController* Controller
                = UGameplayStatics::GetPlayerController(GetWorld(), 0);

            ASideScrollingCamera* Camera = FindSideScrollingCamera();

            if (Controller == nullptr)
            {
                UKismetSystemLibrary::PrintString(this,
                    TEXT("Player controller is null!"));
                return;
            }

            if (Camera == nullptr)
            {
                return;
            }

            APawn* PlayerPawn = Controller->GetPawn();

            if (PlayerPawn != nullptr)
            {
                UKismetSystemLibrary::PrintString(this,
                    TEXT("Teleporting existing player to ") + Transform.GetLocation().ToString(), true, true, 
                    FColorList::DarkGreen, 10.f);

                PlayerPawn->SetActorLocation(Transform.GetLocation(), false, nullptr, ETeleportType::TeleportPhysics);
            }
            else
            {
                UKismetSystemLibrary::PrintString(this,
                    TEXT("Restarting character"));

                AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());

                GameMode->RestartPlayerAtTransform(Controller, Transform);
            }

            // Set camera state.
            Camera->ForceSetCameraState(
                CameraLookingDownPositiveX ?
                ViewDirections::PositiveX : ViewDirections::PositiveY,
                CameraX,
                CameraY
            );

            GetWorld()->GetTimerManager().ClearTimer(RestartTimer);
        },
        0.05f,
        false,
        Delay);

    // Stop polling after 1 second
    FTimerHandle KillTimer;
    GetWorld()->GetTimerManager().SetTimer(KillTimer,
        [this]()
        {
            GetWorld()->GetTimerManager().ClearTimer(RestartTimer);
        },
        0.f, false, 1.f);
}

void ULemonadeGameInstance::PassStateToLevel()
{
    // Seed the level with saved state.
    auto CurrentLevel = GetWorld()->GetCurrentLevel();
    auto LevelScript = Cast<ALemonadeLevelScriptActor>(
        CurrentLevel->GetLevelScriptActor());
    if (LevelScript)
    {
        auto LevelName = CurrentLevelName();
        LevelState.FindOrAdd(LevelName,
            LevelScript->DefaultLevelState);

        LevelScript->TriggerOnSaveLoaded();
    }
}

FString ULemonadeGameInstance::CurrentLevelName() const
{
    FString CleanMapName = GetWorld()->GetMapName();
    CleanMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
    
    // This is probably a terrible hack
    return "/Game/Lemonade/Maps/" + CleanMapName;
}

FString ULemonadeGameInstance::GetLevelState(const FString& LevelName,
    const FString& Key)
{
    LevelData* Data = LevelState.Find(LevelName);

    if (Data)
    {
        FString* Value = Data->Find(Key);
        if (Value)
        {
            return *Value;
        }
    }

    return "";
}

FString ULemonadeGameInstance::GetCurrentLevelState(const FString& Key)
{
    auto LevelName = CurrentLevelName();

    return GetLevelState(LevelName, Key);
}

void ULemonadeGameInstance::SetLevelState(const FString& LevelName,
    const FString& Key,
    const FString& Value)
{
    LevelState.FindOrAdd(LevelName).Add(Key, Value);
}

void ULemonadeGameInstance::SetCurrentLevelState(const FString& Key,
    const FString& Value)
{
    auto LevelName = CurrentLevelName();

    SetLevelState(LevelName, Key, Value);
}

ASideScrollingCamera* ULemonadeGameInstance::FindSideScrollingCamera()
{
    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(),
        TEXT("BP_SideScrollingCamera"),
        ActorsWithTag);

    for (AActor* Actor : ActorsWithTag)
    {
        if (Actor)
        {
            return Cast<ASideScrollingCamera>(Actor);
        }
    }

    return nullptr;
}