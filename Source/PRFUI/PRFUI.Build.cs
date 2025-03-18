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
                "Engine",
                "ModelViewViewModel",
                "ModelViewViewModelBlueprint",
                "Slate",
                "SlateCore",
                "UMG"
            }
        );
    }
}