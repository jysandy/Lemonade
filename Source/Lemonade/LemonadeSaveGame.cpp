// Fill out your copyright notice in the Description page of Project Settings.


#include "LemonadeSaveGame.h"
#include "Json.h"
#include "JsonUtilities.h"

FString ULemonadeSaveGame::DefaultSaveSlotName = TEXT("SaveSlot1");

ULemonadeSaveGame::ULemonadeSaveGame()
{
    SaveSlotName = DefaultSaveSlotName;
    UserIndex = 0;
}


TMap<FString, TMap<FString, FString>> ULemonadeSaveGame::GetLevelData() const
{
    TMap<FString, TMap<FString, FString>> Out;

    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(SerializedLevelData);

    if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
    {
        for (const auto& OuterPair : RootObject->Values)
        {
            TMap<FString, FString> InnerMap;

            TSharedPtr<FJsonObject> InnerObject = OuterPair.Value->AsObject();
            if (InnerObject.IsValid())
            {
                for (const auto& InnerPair : InnerObject->Values)
                {
                    InnerMap.Add(InnerPair.Key, InnerPair.Value->AsString());
                }
            }

            Out.Add(OuterPair.Key, InnerMap);
        }
    }

    return Out;
}


void ULemonadeSaveGame::SetLevelData(const TMap<FString, TMap<FString, FString>>& LevelData)
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);

    for (const auto& OuterPair : LevelData)
    {
        TSharedPtr<FJsonObject> InnerObject = MakeShareable(new FJsonObject);

        for (const auto& InnerPair : OuterPair.Value)
        {
            InnerObject->SetStringField(InnerPair.Key, InnerPair.Value);
        }

        RootObject->SetObjectField(OuterPair.Key, InnerObject);
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    SerializedLevelData = OutputString;
}