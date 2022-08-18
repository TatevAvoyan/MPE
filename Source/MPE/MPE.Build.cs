// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MPE : ModuleRules
{
	public MPE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "HeadMountedDisplay", 
            "UMG" 
        });

        PublicIncludePaths.AddRange(new string[]
        {
            "MPE",
            "MPE/Elevator",
            "MPE/Elevator/Shaft",
            "MPE/UI",
            "MPE/UI/Button",
            "MPE/Interface",
            "MPE/PlayerController"
        });
    }
}
