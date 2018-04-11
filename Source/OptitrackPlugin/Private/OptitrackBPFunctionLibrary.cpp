// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackBPFunctionLibrary.h"

#include "OptitrackPluginModule.h"


void UOptitrackBPFunctionLibrary::NatNetTest()
{
	
	FOptitrackPluginModule::GetOptiTrackSystem()->PrintVersion();
}

void UOptitrackBPFunctionLibrary::AutoConnect()
{
	
	FOptitrackPluginModule::GetOptiTrackSystem()->ConnectToMotive();


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

