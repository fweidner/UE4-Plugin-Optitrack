// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class OptitrackPlugin : ModuleRules
{
	public OptitrackPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
  
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"OptitrackPlugin/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"OptitrackPluginLibrary",
                "Projects",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
                // ... add any modules that your module loads dynamically here ...
			}
			);

        LoadNatNetLib(Target);
	}

    private void LoadNatNetLib(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;
        string LibrariesPath = "";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            isLibrarySupported = true;
            LibrariesPath = Path.Combine(ThirdPartyPath, "NatNetSDK", "lib", "x64");
        }

        if (isLibrarySupported)
        {
            // Include Libs
            PublicLibraryPaths.Add(LibrariesPath);

            PublicAdditionalLibraries.Add("NatNetLib.lib");

            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "NatNetSDK", "include"));

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("NatNetLib.dll");
        }

    }


    // Get the full path the additional library to load
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }
    private string ModulePath
    {
        // get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
        get { return ModuleDirectory; }
    }
}
