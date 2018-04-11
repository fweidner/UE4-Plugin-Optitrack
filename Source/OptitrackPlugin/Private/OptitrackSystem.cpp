// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackSystem.h"

void NATNET_CALLCONV MessageHandler(Verbosity msgType, const char* msg);      // receives NatNet error messages


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
		//g_connectParams.connectionType = discoveredServer.serverDescription.ConnectionMulticast ? ConnectionType_Multicast : ConnectionType_Unicast;
		g_connectParams.connectionType = ConnectionType_Unicast;
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
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Server application info:"));
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Application: %s (ver. %d.%d.%d.%d)"), *FString(g_serverDescription.szHostApp), g_serverDescription.HostAppVersion[0],
			g_serverDescription.HostAppVersion[1], g_serverDescription.HostAppVersion[2], g_serverDescription.HostAppVersion[3]);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("NatNet Version: %d.%d.%d.%d"), g_serverDescription.NatNetVersion[0], g_serverDescription.NatNetVersion[1],
			g_serverDescription.NatNetVersion[2], g_serverDescription.NatNetVersion[3]);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Client IP:%s"), *FString(g_connectParams.localAddress));
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Server IP:%s"), *FString(g_connectParams.serverAddress));
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Server Name:%s"), *FString(g_serverDescription.szHostComputerName));

		// get mocap frame rate
		ret = g_pClient->SendMessageAndWait("FrameRate", &pResult, &nBytes);
		if (ret == ErrorCode_OK)
		{
			float fRate = *((float*)pResult);
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Mocap Framerate : %3.2f"), fRate);
		}
		else
		{
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Error getting frame rate."));
		}

		ret = g_pClient->SendMessageAndWait("AnalogSamplesPerMocapFrame", &pResult, &nBytes);
		if (ret == ErrorCode_OK)
		{
			g_analogSamplesPerMocapFrame = *((int*)pResult);
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Analog Samples Per Mocap Frame : %d"), g_analogSamplesPerMocapFrame);
		}
		else
		{
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Error getting Analog frame rate."));
		}
	}

	UE_LOG(LogNatNetPlugin, Warning, TEXT("Connection to Motive established."));
	return ErrorCode_OK;
}

void OptitrackSystem::ResetClient()
{
	int iSuccess;

	UE_LOG(LogNatNetPlugin, Warning, TEXT("Resetting client..."));

	iSuccess = g_pClient->Disconnect();
	if (iSuccess != 0)
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Client reset failed."));
	}
	else
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Client reset successful.."));
	}
}

void OptitrackSystem::InitClient()
{
	NatNet_SetLogCallback(MessageHandler);
	//g_pClient->SetFrameReceivedCallback(DataHandler, g_pClient);	// this function will receive data from the server

}

float OptitrackSystem::GetFrameRate()
{

	void* pResult;
	int nBytes = 0;
	float fRate = 0.f;
	ErrorCode ret = ErrorCode_OK;

	ret = g_pClient->SendMessageAndWait("FrameRate", &pResult, &nBytes);
	if (ret == ErrorCode_OK)
	{
		fRate = *((float*)pResult);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Mocap Framerate : %3.2f"), fRate);
	}
	else
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Error getting frame rate."));

	return fRate;
}

void OptitrackSystem::GetDataDescription()
{
	UE_LOG(LogNatNetPlugin, Warning, TEXT("Requesting Data Descriptions..."));
	sDataDescriptions* pDataDefs = NULL;
	int iResult = g_pClient->GetDataDescriptionList(&pDataDefs);
	if (iResult != ErrorCode_OK || pDataDefs == NULL)
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Unable to retrieve Data Descriptions."));
	}
	else
	{
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Received %d Data Descriptions:"), pDataDefs->nDataDescriptions);
		for (int i = 0; i < pDataDefs->nDataDescriptions; i++)
		{
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Data Description # %d (type=%d)"), i, pDataDefs->arrDataDescriptions[i].type);
			if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet)
			{
				// MarkerSet
				sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
				UE_LOG(LogNatNetPlugin, Warning, TEXT("MarkerSet Name : %s"), *FString(pMS->szName));
				for (int i = 0; i < pMS->nMarkers; i++)
					UE_LOG(LogNatNetPlugin, Warning, TEXT("%s"), *FString(pMS->szMarkerNames[i]));

			}
			else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody)
			{
				// RigidBody
				sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
				UE_LOG(LogNatNetPlugin, Warning, TEXT("RigidBody Name : %s"), *FString(pRB->szName));
				UE_LOG(LogNatNetPlugin, Warning, TEXT("RigidBody ID : %d"), pRB->ID);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("RigidBody Parent ID : %d"), pRB->parentID);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Parent Offset : %3.2f,%3.2f,%3.2f"), pRB->offsetx, pRB->offsety, pRB->offsetz);

				if (pRB->MarkerPositions != NULL && pRB->MarkerRequiredLabels != NULL)
				{
					for (int markerIdx = 0; markerIdx < pRB->nMarkers; ++markerIdx)
					{
						const MarkerData& markerPosition = pRB->MarkerPositions[markerIdx];
						const int markerRequiredLabel = pRB->MarkerRequiredLabels[markerIdx];

						UE_LOG(LogNatNetPlugin, Warning, TEXT("\tMarker #%d:"), markerIdx);
						UE_LOG(LogNatNetPlugin, Warning, TEXT("\t\tPosition: %.2f, %.2f, %.2f"), markerPosition[0], markerPosition[1], markerPosition[2]);

						if (markerRequiredLabel != 0)
						{
							UE_LOG(LogNatNetPlugin, Warning, TEXT("\t\tRequired active label: %d"), markerRequiredLabel);
						}
					}
				}
			}
			else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton)
			{
				// Skeleton
				sSkeletonDescription* pSK = pDataDefs->arrDataDescriptions[i].Data.SkeletonDescription;
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Skeleton Name : %s"), *FString(pSK->szName));
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Skeleton ID : %d"), pSK->skeletonID);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("RigidBody (Bone) Count : %d"), pSK->nRigidBodies);
				for (int j = 0; j < pSK->nRigidBodies; j++)
				{
					sRigidBodyDescription* pRB = &pSK->RigidBodies[j];
					UE_LOG(LogNatNetPlugin, Warning, TEXT("  RigidBody Name : %s"), *FString(pRB->szName));
					UE_LOG(LogNatNetPlugin, Warning, TEXT("  RigidBody ID : %d"), pRB->ID);
					UE_LOG(LogNatNetPlugin, Warning, TEXT("  RigidBody Parent ID : %d"), pRB->parentID);
					UE_LOG(LogNatNetPlugin, Warning, TEXT("  Parent Offset : %3.2f,%3.2f,%3.2f"), pRB->offsetx, pRB->offsety, pRB->offsetz);
				}
			}
			else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_ForcePlate)
			{
				// Force Plate
				sForcePlateDescription* pFP = pDataDefs->arrDataDescriptions[i].Data.ForcePlateDescription;
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate ID : %d"), pFP->ID);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Serial : %s"), *FString(pFP->strSerialNo));
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Width : %3.2f"), pFP->fWidth);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Length : %3.2f"), pFP->fLength);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Electrical Center Offset (%3.3f, %3.3f, %3.3f)"), pFP->fOriginX, pFP->fOriginY, pFP->fOriginZ);
				for (int iCorner = 0; iCorner < 4; iCorner++)
					UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Corner %d : (%3.4f, %3.4f, %3.4f)"), iCorner, pFP->fCorners[iCorner][0], pFP->fCorners[iCorner][1], pFP->fCorners[iCorner][2]);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Type : %d"), pFP->iPlateType);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Data Type : %d"), pFP->iChannelDataType);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Force Plate Channel Count : %d"), pFP->nChannels);
				for (int iChannel = 0; iChannel < pFP->nChannels; iChannel++)
					UE_LOG(LogNatNetPlugin, Warning, TEXT("\tChannel %d : %s"), iChannel, *FString(pFP->szChannelNames[iChannel]));
			}
			else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Device)
			{
				// Peripheral Device
				sDeviceDescription* pDevice = pDataDefs->arrDataDescriptions[i].Data.DeviceDescription;
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Device Name : %s"), *FString(pDevice->strName));
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Device Serial : %s"), *FString(pDevice->strSerialNo));
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Device ID : %d"), pDevice->ID);
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Device Channel Count : %d"), pDevice->nChannels);
				for (int iChannel = 0; iChannel < pDevice->nChannels; iChannel++)
					UE_LOG(LogNatNetPlugin, Warning, TEXT("\tChannel %d : %s"), iChannel, *FString(pDevice->szChannelNames[iChannel]));
			}
			else
			{
				UE_LOG(LogNatNetPlugin, Warning, TEXT("Unknown data type."));
				// Unknown
			}
		}
	}
}

void NATNET_CALLCONV MessageHandler(Verbosity msgType, const char* msg)
{
	if (msgType < Verbosity_Info)
	{
		return;
	}
	
	switch (msgType)
	{
	case Verbosity_Debug:
		UE_LOG(LogNatNetPlugin, Warning, TEXT("[DEBUG] %s"), *FString(msg));
		break;
	case Verbosity_Info:
		UE_LOG(LogNatNetPlugin, Warning, TEXT("[INFO] %s"), *FString(msg));
		break;
	case Verbosity_Warning:
		UE_LOG(LogNatNetPlugin, Warning, TEXT("[WARN] %s"), *FString(msg));
		break;
	case Verbosity_Error:
		UE_LOG(LogNatNetPlugin, Warning, TEXT("[ERROR] %s"), *FString(msg));
		break;
	default:
		UE_LOG(LogNatNetPlugin, Warning, TEXT("[?????] %s"), *FString(msg));
		
		break;
	}


}