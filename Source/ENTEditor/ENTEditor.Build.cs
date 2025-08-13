using UnrealBuildTool;

public class ENTEditor : ModuleRules
{
    public ENTEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "DataValidation"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "NavigationSystem",
                "Blutility",
                "VREditor",
                "ToolMenus",
                "DeveloperSettings",
                "EditorScriptingUtilities"
            }
        );
    }
}