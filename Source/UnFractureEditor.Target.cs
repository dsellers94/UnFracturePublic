// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnFractureEditorTarget : TargetRules
{
	public UnFractureEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("UnFracture");
        ExtraModuleNames.Add("GameplaySwitch");
        ExtraModuleNames.Add("LevelRecorder");
        ExtraModuleNames.Add("CatSystem");
		ExtraModuleNames.Add("Compressable");
        ExtraModuleNames.Add("LevelRecorderEd");
    }
}
