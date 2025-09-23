using UnrealBuildTool;

public class ENTCore : ModuleRules
{
    public ENTCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "CableComponent",
            "Core",
            "CoreUObject",
            "DeveloperSettings",
            "Engine",
            "EngineCameras",
            "EnhancedInput",
            "InputCore",
            "PhysicsCore",
            "UMG",
            "AIModule",
            "AkAudio",
            "ENTSaves"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "ENTComponents",
            "OnlineSubsystem",
            "OnlineSubsystemSteam"
        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "ENTEditor",
                    "UnrealEd"
                }
            );
        }

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}