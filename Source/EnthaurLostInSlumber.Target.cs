// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class EnthaurLostInSlumberTarget : TargetRules
{
	public EnthaurLostInSlumberTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		Name = "EnthaurLostInSlumber";

		ExtraModuleNames.AddRange( new string[] { "ENTCore", "ENTUI", "ENTArtificialIntelligence", "ENTDeveloperTools", "ENTSaves", "ENTComponents" } );
	}
}
