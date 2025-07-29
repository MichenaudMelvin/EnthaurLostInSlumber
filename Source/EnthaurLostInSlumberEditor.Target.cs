// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class EnthaurLostInSlumberEditorTarget : TargetRules
{
	public EnthaurLostInSlumberEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		Name = "EnthaurLostInSlumberEditor";

		ExtraModuleNames.AddRange( new string[] { "PROTOProfondeurs" } );
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "PRFUI", "DepthAIModule", "ENTEditor", "ENTDeveloperTools", "ENTSaves", "ENTComponents" });
	}
}
