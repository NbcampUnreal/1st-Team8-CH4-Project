using UnrealBuildTool;
using System.Collections.Generic;

public class ProjectT8Target : TargetRules
{
	public ProjectT8Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;

		ExtraModuleNames.AddRange( new string[] { "ProjectT8" } );
	}
}
