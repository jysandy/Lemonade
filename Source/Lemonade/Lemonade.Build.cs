// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lemonade : ModuleRules
{
	public Lemonade(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
            "Paper2D"
        });
	}
}
