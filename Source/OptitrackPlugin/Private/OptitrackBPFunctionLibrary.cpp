// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackBPFunctionLibrary.h"

#include "OptitrackPluginModule.h"


void UOptitrackBPFunctionLibrary::NatNetTest()
{
	OptitrackSystem* tmpSystem = FOptitrackPluginModule::GetOptiTrackSystem();
	tmpSystem->PrintVersion();
}

void UOptitrackBPFunctionLibrary::Connect()
{
	OptitrackSystem* tmpSystem = FOptitrackPluginModule::GetOptiTrackSystem();
	tmpSystem->ConnectToMotive();

}