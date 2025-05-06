// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "LemonadeSaveGame.generated.h"

/**
 *
 */
UCLASS()
class LEMONADE_API ULemonadeSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    FVector PlayerLocation;

    // Level data map serialized as JSON.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    FString SerializedLevelData;

    // The level name.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    FString LevelName;

    // Camera state

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    bool CameraLookingDownPositiveX;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    float CameraX;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    float CameraY;

    // End camera state


    TMap<FString, TMap<FString, FString>> GetLevelData() const;

    void SetLevelData(const TMap<FString, TMap<FString, FString>>& LevelData);

    // Default stuff
    static FString DefaultSaveSlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basic")
    int32 UserIndex;

    ULemonadeSaveGame();

};
