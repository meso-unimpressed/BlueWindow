// Some copyright should be here...

using UnrealBuildTool;

public class BlueWindow : ModuleRules
{
	public BlueWindow(ReadOnlyTargetRules Target) : base(Target)
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
                "ImageWrapper",
                "TargetPlatform",
            }
        );

        PublicDependencyModuleNames.AddRange(
			new string[]
            {
                "ApplicationCore",
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "UMG",
                "Slate",
                "SlateCore",
                "RenderCore",
                "RHI",
                "MovieScene"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "RHI",
                "Slate",
                "SlateCore",
                "RenderCore",
                "UMG",
            }
		);

        if(Target.Type == TargetType.Editor)
            PrivateDependencyModuleNames.Add("UnrealEd");

        if (Target.Type != TargetType.Server)
        {
            PrivateIncludePathModuleNames.AddRange(
                new string[] {
                    "SlateRHIRenderer",
                }
            );

            DynamicallyLoadedModuleNames.AddRange(
                new string[] {
                    "ImageWrapper",
                    "SlateRHIRenderer",
                }
            );
        }

        DynamicallyLoadedModuleNames.AddRange(
			new string[] { }
		);
	}
}
