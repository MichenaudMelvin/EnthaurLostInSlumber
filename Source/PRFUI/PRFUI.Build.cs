using UnrealBuildTool;

public class PRFUI : ModuleRules
{
    public PRFUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "DeveloperSettings",
                "Engine",
                "EnhancedInput",
                "ModelViewViewModel",
                "PROTOProfondeurs",
                "Slate",
                "SlateCore",
                "UMG",
            }
        );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "ModelViewViewModelBlueprint",
                }
            );
        }
    }
}