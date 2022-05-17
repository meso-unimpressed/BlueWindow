// Some copyright should be here...

using UnrealBuildTool;

public class BlueWindowEditor : ModuleRules
{
	public BlueWindowEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
        CppStandard = CppStandardVersion.Cpp17;
        bEnableUndefinedIdentifierWarnings = false;

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
	            "BlueWindow",
            }
        );

        PublicDependencyModuleNames.AddRange(
			new string[]
            {
                "ApplicationCore",
                "Core",
                "CoreUObject",
                "Engine",
                "UMG",
                "Slate",
                "SlateCore",
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "Slate",
                "SlateCore",
                "UMG",
                
                "MESOUSLCPP",
                "BlueWindow",
            }
		);

        if(Target.Type == TargetType.Editor)
            PrivateDependencyModuleNames.AddRange(
	            new []
	            {
		            "UnrealEd",
		            "MESOUSLCPPEditor"
	            }
	        );

        DynamicallyLoadedModuleNames.AddRange(
			new string[] { }
		);
	}
}
