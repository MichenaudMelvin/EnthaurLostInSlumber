using UnrealBuildTool;

public class ENTUI : ModuleRules
{
    public ENTUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "CommonUI",
                "Core",
                "ENTCore",
                "UMG"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "DeveloperSettings",
                "Engine",
                "EnhancedInput",
                "GameplayTags",
                "InputCore",
                "Slate",
                "SlateCore",
                "ENTSaves",
                "ENTComponents"
            }
        );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "ENTEditor"
                }
            );
        }
    }
}