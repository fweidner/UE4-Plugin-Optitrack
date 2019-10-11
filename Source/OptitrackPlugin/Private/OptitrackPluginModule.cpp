// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OptitrackPluginModule.h"

#include "Misc/MessageDialog.h"
#include "Misc/Paths.h" 

#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogNatNetPlugin, Log, All);

#define LOCTEXT_NAMESPACE "FOptitrackPluginModule"

Optitrack::OptitrackSystem* OptitrackSystem_ = NULL;

void FOptitrackPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("OptitrackPlugin")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("/ThirdParty/NatNetSDK/lib/x64/NatNetLib.dll"));
	// 	UE_LOG(LogNatNetPlugin, Warning, TEXT("BaseDir: %s"), *BaseDir);
	// 	UE_LOG(LogNatNetPlugin, Warning, TEXT("LibDir: %s"), *LibraryPath);

	NatNetHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if (NatNetHandle)
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetModule loaded successfully (1st)!"));
		OptitrackSystem_ = new Optitrack::OptitrackSystem();
	}
	else 
	{
		LibraryPath = FPaths::Combine("", TEXT("ThirdParty/NatNetSDK/lib/x64/NatNetLib.dll")); //start packaged app in root
		//UE_LOG(LogNatNetPlugin, Warning, TEXT("LibDir: %s"), *LibraryPath);
		NatNetHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

		if (NatNetHandle)
		{
			UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetModule loaded successfully (2nd try)!"));
			OptitrackSystem_ = new Optitrack::OptitrackSystem();
		}
		else
		{
			LibraryPath = FPaths::Combine("", TEXT("../../../ThirdParty/NatNetSDK/lib/x64/NatNetLib.dll")); //start packaged app in binaries
			//UE_LOG(LogNatNetPlugin, Warning, TEXT("LibDir: %s"), *LibraryPath);
			NatNetHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

			if (NatNetHandle)
			{
				UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetModule loaded successfully (3rd try)!"));
				OptitrackSystem_ = new Optitrack::OptitrackSystem();
			}
			else
			{
				LibraryPath = FPaths::Combine(FPaths::LaunchDir(), TEXT("../../../../../../Plugins/OptitrackPlugin/ThirdParty/NatNetSDK/lib/x64/NatNetLib.dll")); //start staged build from within UE4 via launch
				//UE_LOG(LogNatNetPlugin, Warning, TEXT("LibDir: %s"), *LibraryPath);
				NatNetHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

				if (NatNetHandle)
				{
					UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetModule loaded successfully (4. try)!"));
					OptitrackSystem_ = new Optitrack::OptitrackSystem();
				}
				else
				{
					
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load NatNetLibrary library"));
				}
			}
		}
	}
}

void FOptitrackPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetModule shut down!"));


	// Free the dll handle
	FPlatformProcess::FreeDllHandle(NatNetHandle);
	NatNetHandle = nullptr;
}


Optitrack::OptitrackSystem* FOptitrackPluginModule::GetOptiTrackSystem()
{
	return OptitrackSystem_;
}

void FOptitrackPluginModule::TestOptitrack()
{
	// Call the test function 
	unsigned char ver[4];
	NatNet_GetVersion(ver);
	UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetVersion is %d.%d.%d.%d"), ver[0], ver[1], ver[2], ver[3]);

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOptitrackPluginModule, OptitrackPlugin)