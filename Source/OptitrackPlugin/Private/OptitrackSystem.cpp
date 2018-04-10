// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackSystem.h"

OptitrackSystem::OptitrackSystem()
{
}

OptitrackSystem::~OptitrackSystem()
{
}

void OptitrackSystem::PrintVersion()
{
	unsigned char ver[4];
	NatNet_GetVersion(ver);
	UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNetVersion is %d.%d.%d.%d"), ver[0], ver[1], ver[2], ver[3]);

}

/************************************************************************/
/* MOTIVE / NATNET CORE		                                            */
/************************************************************************/

int OptitrackSystem::ConnectToMotive()
{
	UE_LOG(LogNatNetPlugin, Warning, TEXT("Establish connection to Motive..."));

	if (g_pClient)
	{
		g_pClient->Disconnect();
		g_pClient = NULL;
		delete g_pClient;
	}

	g_pClient = new NatNetClient();



	const unsigned int kDiscoveryWaitTimeMillisec = 1 * 1000;
	const int kMaxDescriptions = 1;
	sNatNetDiscoveredServer servers[kMaxDescriptions];
	int actualNumDescriptions = kMaxDescriptions;
	NatNet_BroadcastServerDiscovery(servers, &actualNumDescriptions);

	if (actualNumDescriptions > kMaxDescriptions)
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("To many servers responded."));
		return ErrorCode_Network;
	}

	if (actualNumDescriptions == 0)
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Server not found. Motive started and ready?"));
		return ErrorCode_Network;
	}

	const sNatNetDiscoveredServer& discoveredServer = servers[0];
	if (discoveredServer.serverDescription.bConnectionInfoValid)
	{
		// Build the connection parameters.
		g_connectParams.connectionType = discoveredServer.serverDescription.ConnectionMulticast ? ConnectionType_Multicast : ConnectionType_Unicast;
		g_connectParams.serverCommandPort = discoveredServer.serverCommandPort;
		g_connectParams.serverDataPort = discoveredServer.serverDescription.ConnectionDataPort;
		g_connectParams.serverAddress = discoveredServer.serverAddress;
		g_connectParams.localAddress = discoveredServer.localAddress;
		g_connectParams.multicastAddress = g_discoveredMulticastGroupAddr;
	}

	// Init Client and connect to NatNet server
	int retCode = g_pClient->Connect(g_connectParams);
	if (retCode != ErrorCode_OK)
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Unable to connect to server.  Error code: %d. Exiting"), retCode);
		return ErrorCode_Internal;
	}
	else
	{
		void* pResult;
		int nBytes = 0;
		ErrorCode ret = ErrorCode_OK;

		// print server info
		memset(&g_serverDescription, 0, sizeof(g_serverDescription));
		ret = g_pClient->GetServerDescription(&g_serverDescription);
		if (ret != ErrorCode_OK || !g_serverDescription.HostPresent)
		{
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Unable to connect to server. Host not present. Exiting."));
			return ErrorCode_Network;
		}
		UE_LOG(LogNatNetPlugin, Warning, TEXT("\n[SampleClient] Server application info:\n"));
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Application: %s (ver. %d.%d.%d.%d)\n"), *FString(g_serverDescription.szHostApp), g_serverDescription.HostAppVersion[0],
			g_serverDescription.HostAppVersion[1], g_serverDescription.HostAppVersion[2], g_serverDescription.HostAppVersion[3]);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNet Version: %d.%d.%d.%d\n"), g_serverDescription.NatNetVersion[0], g_serverDescription.NatNetVersion[1],
			g_serverDescription.NatNetVersion[2], g_serverDescription.NatNetVersion[3]);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Client IP:%s\n"), *FString(g_connectParams.localAddress));
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Server IP:%s\n"), g_connectParams.serverAddress);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Server Name:%s\n"), *FString(g_serverDescription.szHostComputerName));

		// get mocap frame rate
		ret = g_pClient->SendMessageAndWait("FrameRate", &pResult, &nBytes);
		if (ret == ErrorCode_OK)
		{
			float fRate = *((float*)pResult);
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Mocap Framerate : %3.2f\n"), fRate);
		}
		else
		{
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Error getting frame rate.\n"));
		}

		ret = g_pClient->SendMessageAndWait("AnalogSamplesPerMocapFrame", &pResult, &nBytes);
		if (ret == ErrorCode_OK)
		{
			g_analogSamplesPerMocapFrame = *((int*)pResult);
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Analog Samples Per Mocap Frame : %d\n"), g_analogSamplesPerMocapFrame);
		}
		else
		{
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Error getting Analog frame rate.\n"));
		}
	}

	return ErrorCode_OK;
}