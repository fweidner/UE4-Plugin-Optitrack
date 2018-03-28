// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OptitrackPlugin.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "NatNetCAPI.h"

DEFINE_LOG_CATEGORY_STATIC(LogNatNetPlugin, Log, All);


#define LOCTEXT_NAMESPACE "FOptitrackPluginModule"

void FOptitrackPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("OptitrackPlugin")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("/ThirdParty/NatNetSDK/lib/x64/NatNetLib.dll"));

	NatNetHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (NatNetHandle)
	{
		// Call the test function 
		unsigned char ver[4];
		NatNet_GetVersion(ver);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetModule loaded successfully.!"));
		
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOptitrackPluginModule, OptitrackPlugin)