using UnrealBuildTool;

public class PRFUI : ModuleRules
{
    public PRFUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        CircularlyReferencedDependentModules.Add("PROTOProfondeurs");

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "DeveloperSettings",
                "Engine",
                "EnhancedInput",
                "PROTOProfondeurs",
                "Slate",
                "SlateCore",
                "UMG",
                "ENTSaves"
            }
        );
    }
}