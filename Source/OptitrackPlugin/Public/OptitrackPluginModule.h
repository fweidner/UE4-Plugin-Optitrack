// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include <NatNetTypes.h>
#include <NatNetCAPI.h>
#include <NatNetClient.h>

#include <vector>

#include "ModuleManager.h"




class FOptitrackPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	int ConnectToMotive();

private:
	/** Handle to the test dll we will load */
	void*	NatNetHandle;
	
	NatNetClient* g_pClient = NULL;
	
	sServerDescription g_serverDescription;
	
	sNatNetClientConnectParams g_connectParams;
	
	char g_discoveredMulticastGroupAddr[kNatNetIpv4AddrStrLenMax] = NATNET_DEFAULT_MULTICAST_ADDRESS;
	
	int g_analogSamplesPerMocapFrame = 0;
};
