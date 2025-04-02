using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectT8EditorTarget : TargetRules
{
	public ProjectT8EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("ProjectT8");
	}
}
