using UnrealBuildTool;

public class ENTAI : ModuleRules
{
    public ENTAI(ReadOnlyTargetRules Target) : base(Target)
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
                "ENTSaves",
                "ENTComponents"
            }
        );
    }
}