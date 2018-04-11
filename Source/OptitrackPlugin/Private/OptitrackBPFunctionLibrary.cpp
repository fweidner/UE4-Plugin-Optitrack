// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackBPFunctionLibrary.h"

#include "OptitrackPluginModule.h"


void UOptitrackBPFunctionLibrary::NatNetTest()
{
	
	FOptitrackPluginModule::GetOptiTrackSystem()->PrintVersion();
}

void UOptitrackBPFunctionLibrary::ConnectAndInit()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->ConnectAndInit();
}

void UOptitrackBPFunctionLibrary::AutoConnect()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->ConnectToMotive();
}

void UOptitrackBPFunctionLibrary::Disconnect()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->DisconnectFromMotive();
}

void UOptitrackBPFunctionLibrary::ResetClient()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->ResetClient();
}

void UOptitrackBPFunctionLibrary::InitClient()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->InitClient();
}

float UOptitrackBPFunctionLibrary::GetFrameRate()
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->GetFrameRate();
}

void UOptitrackBPFunctionLibrary::GetDataDescription()
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->GetDataDescription();
}
