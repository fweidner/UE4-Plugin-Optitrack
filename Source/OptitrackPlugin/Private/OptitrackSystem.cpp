// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackSystem.h"

void NATNET_CALLCONV MessageHandler(Verbosity msgType, const char* msg);     
void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData);   
int g_analogSamplesPerMocapFrame = 0;

TMap<int32_t, sRigidBodyData> RigidBodyData;
bool PrintDebugMessages = false;


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

void OptitrackSystem::ConnectAndInit()
{
	ConnectToMotive();
	InitClient();
}

int OptitrackSystem::ConnectToMotive()
{
	UE_LOG(LogNatNetPlugin, Warning, TEXT("Establish connection to Motive..."));

	DisconnectFromMotive();

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


void OptitrackSystem::DisconnectFromMotive()
{
	if (g_pClient)
	{
		g_pClient->Disconnect();
		g_pClient = NULL;
		delete g_pClient;
	}
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
	g_pClient->SetFrameReceivedCallback(DataHandler, g_pClient);
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

float OptitrackSystem::GetUnitsToMillimeter()
{
	void* pResult;
	int nBytes = 0;
	float unitstomm = 0.f;
	ErrorCode ret = ErrorCode_OK;

	ret = g_pClient->SendMessageAndWait("UnitsToMillimeters", &pResult, &nBytes);
	if (ret == ErrorCode_OK)
	{
		unitstomm = *((float*)pResult);
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Mocap UnitsToMillimeters : %3.2f"), unitstomm);
	}
	else
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Error getting UnitsToMillimeters."));

	UnitsToMM = unitstomm;

	return unitstomm;
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

FTransform OptitrackSystem::GetRigidBodyTransform(int _ID)
{
	sRigidBodyData* tmpRigidBodyData = RigidBodyData.Find(_ID);
	
	if (tmpRigidBodyData)
	{
		return FTransform(
			FRotator(FQuat(tmpRigidBodyData->qx, tmpRigidBodyData->qy, tmpRigidBodyData->qz, tmpRigidBodyData->qw)),
			FVector(tmpRigidBodyData->x*UnitsToMM, tmpRigidBodyData->y*UnitsToMM, tmpRigidBodyData->z*UnitsToMM),
			FVector(1, 1, 1)
		);
	}
	else
	{
		return FTransform();
	}
}

bool OptitrackSystem::SetPrintDebugMessages(bool _newVal)
{
	PrintDebugMessages = _newVal;
	return PrintDebugMessages;
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

void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	NatNetClient* pClient = (NatNetClient*)pUserData;

///
// Software latency here is defined as the span of time between:

// 	//   a) The reception of a complete group of 2D frames from the camera system (CameraDataReceivedTimestamp)
// 	// and
// 	//   b) The time immediately prior to the NatNet frame being transmitted over the network (TransmitTimestamp)
// 	//
// 	// This figure may appear slightly higher than the "software latency" reported in the Motive user interface,
// 	// because it additionally includes the time spent preparing to stream the data via NatNet.
// 	const uint64_t softwareLatencyHostTicks = data->TransmitTimestamp - data->CameraDataReceivedTimestamp;
// 	const double softwareLatencyMillisec = (softwareLatencyHostTicks * 1000) / static_cast<double>(g_serverDescription.HighResClockFrequency);
// 
// 	// Transit latency is defined as the span of time between Motive transmitting the frame of data, and its reception by the client (now).
// 	// The SecondsSinceHostTimestamp method relies on NatNetClient's internal clock synchronization with the server using Cristian's algorithm.
// 	const double transitLatencyMillisec = pClient->SecondsSinceHostTimestamp(data->TransmitTimestamp) * 1000.0;
// 
// 
// 
// 	int i = 0;
// 
// 	printf("FrameID : %d\n", data->iFrame);
// 	printf("Timestamp : %3.2lf\n", data->fTimestamp);
// 	printf("Software latency : %.2lf milliseconds\n", softwareLatencyMillisec);
// 
// 	// Only recent versions of the Motive software in combination with ethernet camera systems support system latency measurement.
// 	// If it's unavailable (for example, with USB camera systems, or during playback), this field will be zero.
// 	const bool bSystemLatencyAvailable = data->CameraMidExposureTimestamp != 0;
// 
// 	if (bSystemLatencyAvailable)
// 	{
// 		// System latency here is defined as the span of time between:
// 		//   a) The midpoint of the camera exposure window, and therefore the average age of the photons (CameraMidExposureTimestamp)
// 		// and
// 		//   b) The time immediately prior to the NatNet frame being transmitted over the network (TransmitTimestamp)
// 		const uint64_t systemLatencyHostTicks = data->TransmitTimestamp - data->CameraMidExposureTimestamp;
// 		const double systemLatencyMillisec = (systemLatencyHostTicks * 1000) / static_cast<double>(g_serverDescription.HighResClockFrequency);
// 
// 		// Client latency is defined as the sum of system latency and the transit time taken to relay the data to the NatNet client.
// 		// This is the all-inclusive measurement (photons to client processing).
// 		const double clientLatencyMillisec = pClient->SecondsSinceHostTimestamp(data->CameraMidExposureTimestamp) * 1000.0;
// 
// 		// You could equivalently do the following (not accounting for time elapsed since we calculated transit latency above):
// 		//const double clientLatencyMillisec = systemLatencyMillisec + transitLatencyMillisec;
// 
// 		printf("System latency : %.2lf milliseconds\n", systemLatencyMillisec);
// 		printf("Total client latency : %.2lf milliseconds (transit time +%.2lf ms)\n", clientLatencyMillisec, transitLatencyMillisec);
// 	}
// 	else
// 	{
// 		printf("Transit latency : %.2lf milliseconds\n", transitLatencyMillisec);
// 	}

	bool bTrackedModelsChanged = ((data->params & 0x02) != 0);
	if (bTrackedModelsChanged)
		UE_LOG(LogNatNetPlugin, Warning, TEXT("Models Changed.\n"));

	// Rigid Bodies
	UE_LOG(LogNatNetPlugin, Warning, TEXT("Rigid Bodies [Count=%d]"), data->nRigidBodies);
	
	int i = 0;
	
	for (i = 0; i < data->nRigidBodies; i++)
	{
		// params
		// 0x01 : bool, rigid body was successfully tracked in this frame
		if (PrintDebugMessages)
		{
			bool bTrackingValid = data->RigidBodies[i].params & 0x01;

			//UE_LOG(LogNatNetPlugin, Warning, TEXT("\tx\ty\tz\tqx\tqy\tqz\tqw"));
			UE_LOG(LogNatNetPlugin, Warning, TEXT("Rigid Body [ID=%d  Error=%3.2f  Valid=%d]: \t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f"), data->RigidBodies[i].ID, data->RigidBodies[i].MeanError, bTrackingValid,
				data->RigidBodies[i].x,
				data->RigidBodies[i].y,
				data->RigidBodies[i].z,
				data->RigidBodies[i].qx,
				data->RigidBodies[i].qy,
				data->RigidBodies[i].qz,
				data->RigidBodies[i].qw);

			RigidBodyData.Add(data->RigidBodies[i].ID, data->RigidBodies[i]);
		}
	}

///
// Skeletons

// 	printf("Skeletons [Count=%d]\n", data->nSkeletons);
// 	for (i = 0; i < data->nSkeletons; i++)
// 	{
// 		sSkeletonData skData = data->Skeletons[i];
// 		printf("Skeleton [ID=%d  Bone count=%d]\n", skData.skeletonID, skData.nRigidBodies);
// 		for (int j = 0; j < skData.nRigidBodies; j++)
// 		{
// 			sRigidBodyData rbData = skData.RigidBodyData[j];
// 			printf("Bone %d\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
// 				rbData.ID, rbData.x, rbData.y, rbData.z, rbData.qx, rbData.qy, rbData.qz, rbData.qw);
// 		}
// 	}

///
// labeled markers - this includes all markers (Active, Passive, and 'unlabeled' (markers with no asset but a PointCloud ID)

// 	bool bOccluded;     // marker was not visible (occluded) in this frame
// 	bool bPCSolved;     // reported position provided by point cloud solve
// 	bool bModelSolved;  // reported position provided by model solve
// 	bool bHasModel;     // marker has an associated asset in the data stream
// 	bool bUnlabeled;    // marker is 'unlabeled', but has a point cloud ID that matches Motive PointCloud ID (In Motive 3D View)
// 	bool bActiveMarker; // marker is an actively labeled LED marker
// 
// 	UE_LOG(LogNatNetPlugin, Warning, TEXT("Markers [Count=%d]"), data->nLabeledMarkers);
// 	for (i = 0; i < data->nLabeledMarkers; i++)
// 	{
// 		bOccluded = ((data->LabeledMarkers[i].params & 0x01) != 0);
// 		bPCSolved = ((data->LabeledMarkers[i].params & 0x02) != 0);
// 		bModelSolved = ((data->LabeledMarkers[i].params & 0x04) != 0);
// 		bHasModel = ((data->LabeledMarkers[i].params & 0x08) != 0);
// 		bUnlabeled = ((data->LabeledMarkers[i].params & 0x10) != 0);
// 		bActiveMarker = ((data->LabeledMarkers[i].params & 0x20) != 0);
// 
// 		sMarker marker = data->LabeledMarkers[i];
// 
// 		// Marker ID Scheme:
// 		// Active Markers:
// 		//   ID = ActiveID, correlates to RB ActiveLabels list
// 		// Passive Markers: 
// 		//   If Asset with Legacy Labels
// 		//      AssetID 	(Hi Word)
// 		//      MemberID	(Lo Word)
// 		//   Else
// 		//      PointCloud ID
// 		int modelID, markerID;
// 		NatNet_DecodeID(marker.ID, &modelID, &markerID);
// 
// 		FString szMarkerType = "?";
// 		if (bActiveMarker)
// 			szMarkerType = "Active";
// 		else if (bUnlabeled)
// 			szMarkerType = "Unlabeled";
// 		else
// 			szMarkerType = "Labeled";
// 
// 		UE_LOG(LogNatNetPlugin, Warning, TEXT("%s Marker [ModelID=%d, MarkerID=%d, Occluded=%d, PCSolved=%d, ModelSolved=%d] [size=%3.2f] [pos=%3.2f,%3.2f,%3.2f]"),
// 			*szMarkerType, modelID, markerID, bOccluded, bPCSolved, bModelSolved, marker.size, marker.x, marker.y, marker.z);
// 	}

///
 // force plates

// 	printf("Force Plate [Count=%d]\n", data->nForcePlates);
// 	for (int iPlate = 0; iPlate < data->nForcePlates; iPlate++)
// 	{
// 		printf("Force Plate %d\n", data->ForcePlates[iPlate].ID);
// 		for (int iChannel = 0; iChannel < data->ForcePlates[iPlate].nChannels; iChannel++)
// 		{
// 			printf("\tChannel %d:\t", iChannel);
// 			if (data->ForcePlates[iPlate].ChannelData[iChannel].nFrames == 0)
// 			{
// 				printf("\tEmpty Frame\n");
// 			}
// 			else if (data->ForcePlates[iPlate].ChannelData[iChannel].nFrames != g_analogSamplesPerMocapFrame)
// 			{
// 				printf("\tPartial Frame [Expected:%d   Actual:%d]\n", g_analogSamplesPerMocapFrame, data->ForcePlates[iPlate].ChannelData[iChannel].nFrames);
// 			}
// 			for (int iSample = 0; iSample < data->ForcePlates[iPlate].ChannelData[iChannel].nFrames; iSample++)
// 				printf("%3.2f\t", data->ForcePlates[iPlate].ChannelData[iChannel].Values[iSample]);
// 			printf("\n");
// 		}
// 	}

///
//devices

// 	UE_LOG(LogNatNetPlugin, Warning, TEXT("Device [Count=%d]"), data->nDevices);
// 	for (int iDevice = 0; iDevice < data->nDevices; iDevice++)
// 	{
// 		UE_LOG(LogNatNetPlugin, Warning, TEXT("Device %d"), data->Devices[iDevice].ID);
// 		for (int iChannel = 0; iChannel < data->Devices[iDevice].nChannels; iChannel++)
// 		{
// 			UE_LOG(LogNatNetPlugin, Warning, TEXT("\tChannel %d:\t"), iChannel);
// 			if (data->Devices[iDevice].ChannelData[iChannel].nFrames == 0)
// 			{
// 				UE_LOG(LogNatNetPlugin, Warning, TEXT("\tEmpty Frame"));
// 			}
// 			else if (data->Devices[iDevice].ChannelData[iChannel].nFrames != g_analogSamplesPerMocapFrame)
// 			{
// 				UE_LOG(LogNatNetPlugin, Warning, TEXT("\tPartial Frame [Expected:%d   Actual:%d]"), g_analogSamplesPerMocapFrame, data->Devices[iDevice].ChannelData[iChannel].nFrames);
// 			}
// 			for (int iSample = 0; iSample < data->Devices[iDevice].ChannelData[iChannel].nFrames; iSample++)
// 				UE_LOG(LogNatNetPlugin, Warning, TEXT("%3.2f\t"), data->Devices[iDevice].ChannelData[iChannel].Values[iSample]);
// 			
// 		}
// 	}
}