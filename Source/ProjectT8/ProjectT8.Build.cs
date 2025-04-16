using UnrealBuildTool;

public class ProjectT8 : ModuleRules
{
	public ProjectT8(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AssetRegistry",
			"UMG",
			"AdvancedSteamSessions",
			"AdvancedSessions",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"OnlineSubsystemSteam",
            "Networking",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
            "AIModule",
            "GameplayDebugger",
			"NavigationSystem"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(new string[] { "ProjectT8" });
    }
}
