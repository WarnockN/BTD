// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BTD : ModuleRules
{
	public BTD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "HeadMountedDisplay" });
	}
}
