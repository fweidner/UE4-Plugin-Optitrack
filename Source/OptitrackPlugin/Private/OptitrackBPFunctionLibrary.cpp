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

float UOptitrackBPFunctionLibrary::GetUnitsToMillimeter()
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->GetUnitsToMillimeter();
}

void UOptitrackBPFunctionLibrary::GetDataDescription()
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->GetDataDescription();
}

bool UOptitrackBPFunctionLibrary::SetPrintDebugMessages(bool _newVal)
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->SetPrintDebugMessages(_newVal);
}

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScaleActor(AActor* _tmpActor, int _ID)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(_ID);

	_tmpActor->SetActorLocation(tmpTransform.GetLocation());
	_tmpActor->SetActorRotation(tmpTransform.GetRotation());

	tmpTransform.SetScale3D(_tmpActor->GetActorScale3D());

	return tmpTransform;
}
FTransform UOptitrackBPFunctionLibrary::GetRigidBodyTransform(int _ID)
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(_ID);
}

