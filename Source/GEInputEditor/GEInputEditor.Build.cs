// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GEInputEditor : ModuleRules
{
	public GEInputEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;	

		PublicIncludePaths.AddRange(
            new string[] 
			{
                ModuleDirectory,
                ModuleDirectory + "/GEInputEditor",
            }
        );			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core", "CoreUObject", "Engine", "UnrealEd",

                "ClassViewer", "AssetTools", "ToolMenus",

                "InputCore", "Slate", "SlateCore",

                "Kismet", "KismetCompiler",

                "GEInput",
            }
		);
	}
}
