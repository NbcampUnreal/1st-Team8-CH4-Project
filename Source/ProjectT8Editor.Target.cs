using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectT8EditorTarget : TargetRules
{
	public ProjectT8EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		bOverrideBuildEnvironment = true;

		ExtraModuleNames.AddRange( new string[] { "ProjectT8" } );
	}
}
