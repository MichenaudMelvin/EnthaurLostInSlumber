// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class EnthaurLostInSlumberTarget : TargetRules
{
	public EnthaurLostInSlumberTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		Name = "EnthaurLostInSlumber";

		ExtraModuleNames.AddRange( new string[] { "ENTCore", "ENTUI", "ENTAI", "ENTDeveloperTools", "ENTSaves", "ENTComponents" } );
	}
}
