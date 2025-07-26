﻿using UnrealBuildTool;

public class DepthAIModule : ModuleRules
{
    public DepthAIModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "AIModule",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "AIModule",
                "GameplayTasks",
                "NavigationSystem",
                "PROTOProfondeurs",
                "ENTSaves"
            }
        );
    }
}