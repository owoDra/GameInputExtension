// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GEInput : ModuleRules
{
	public GEInput(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GEInput",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine",

                "GameplayTags", "GameFeatures", "ModularGameplay",

                "InputCore", "EnhancedInput", 

                "GFCore",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}
