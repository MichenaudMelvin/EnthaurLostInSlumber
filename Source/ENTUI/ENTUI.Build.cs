using UnrealBuildTool;

public class ENTUI : ModuleRules
{
    public ENTUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "PROTOProfondeurs"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "DeveloperSettings",
                "Engine",
                "EnhancedInput",
                "Slate",
                "SlateCore",
                "UMG",
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