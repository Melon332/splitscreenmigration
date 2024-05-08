// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SplitScreenCPP : ModuleRules
{
	public SplitScreenCPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
