// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class PROTOProfondeurs : ModuleRules
{
	public PROTOProfondeurs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CircularlyReferencedDependentModules.Add("PRFUI");

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"CableComponent",
			"Core",
			"CoreUObject",
			"DeveloperSettings",
			"Engine",
			"EngineCameras",
			"EnhancedInput",
			"FCTween",
			"InputCore",
			"PhysicsCore",
			"UMG",
			"AIModule",
			"AkAudio",
			"ENTSaves"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"PRFUI",
			"ENTComponents"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"ENTEditor",
					"UnrealEd"
				}
			);
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
