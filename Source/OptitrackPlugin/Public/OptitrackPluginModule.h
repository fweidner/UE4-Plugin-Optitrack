// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include <NatNetTypes.h>
#include <NatNetCAPI.h>
#include <NatNetClient.h>

#include "OptitrackSystem.h"

#include <vector>

#include "ModuleManager.h"




class FOptitrackPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	int ConnectToMotive();

	static Optitrack::OptitrackSystem* GetOptiTrackSystem();;

private:
	/** Handle to the test dll we will load */
	void* NatNetHandle;
	
	

};
