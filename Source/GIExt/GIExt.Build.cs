// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GIExt : ModuleRules
{
	public GIExt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
           new string[]
           {
                ModuleDirectory,
                ModuleDirectory + "/GIExt",
           }
       );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "InputCore",
                "EnhancedInput",
                "CommonInput",
                "CommonUI",
                "ModularGameplay",
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
