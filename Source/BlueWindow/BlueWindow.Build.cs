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
                "MovieScene",
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
                "Slate",
                "SlateCore",
                "RenderCore",
                "D3D12RHI",
                "UMG",
                
                "MESOUSLCPP",
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

        if (Target.Platform.IsInGroup(UnrealPlatformGroup.Windows) ||
            Target.Platform == UnrealTargetPlatform.HoloLens)
        {
            AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11");
        }

        DynamicallyLoadedModuleNames.AddRange(
			new string[] { }
		);
	}
}
