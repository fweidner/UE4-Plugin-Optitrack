// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackBPFunctionLibrary.h"
#include "Transformer.h"

#include "OptitrackPluginModule.h"


void UOptitrackBPFunctionLibrary::NatNetTest()
{

	FOptitrackPluginModule::GetOptiTrackSystem()->PrintVersion();
}

void UOptitrackBPFunctionLibrary::AutoConnectAndInit()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->ConnectAndInit();
}

void UOptitrackBPFunctionLibrary::AutoConnect()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->ConnectToMotive();
}

void UOptitrackBPFunctionLibrary::DisconnectAndResetClient()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->DisconnectAndResetClient();
}

void UOptitrackBPFunctionLibrary::DisconnectClient()
{
	FOptitrackPluginModule::GetOptiTrackSystem()->DisconnectClient();
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

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScaleActor(AActor* _tmpActor, int _ID /*= 1*/, FString _Name /*= ""*/, ERigidBodyIdentifierOptitrack _IdentifierMethod /*= ERigidBodyIdentifierOptitrack::RigidBodyID*/)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(GetCorrectID(_Name, _ID, _IdentifierMethod));


	if (_tmpActor)
	{
		tmpTransform = Optitrack::Transformer::UpdateTransformForActor(_tmpActor, tmpTransform);
	}



	return tmpTransform;
}

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScaleSceneComponent(USceneComponent* _tmpSceneComponent, int _ID /*= 1*/, FString _Name /*= ""*/, ERigidBodyIdentifierOptitrack _IdentifierMethod /*= ERigidBodyIdentifierOptitrack::RigidBodyID*/)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(GetCorrectID(_Name, _ID, _IdentifierMethod));
	if (_tmpSceneComponent)
	{
		tmpTransform = Optitrack::Transformer::UpdateTransformForSceneComponent(_tmpSceneComponent, tmpTransform);
	}

	return tmpTransform;

}

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScalePawn(APawn* _Pawn, int _ID/*=1*/, FString _Name /*= ""*/, ERigidBodyIdentifierOptitrack _IdentifierMethod /*= ERigidBodyIdentifierOptitrack::RigidBodyID*/)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(GetCorrectID(_Name, _ID, _IdentifierMethod));

	if (_Pawn)
	{
		tmpTransform = Optitrack::Transformer::UpdateTransformForPawn(_Pawn, tmpTransform);
	}
	return tmpTransform;
}

void UOptitrackBPFunctionLibrary::ResetRotationPlayer(APawn* _tmp, bool _yaw, bool _pitch, bool _roll)
{

	//X= Roll, Y= Pitch, Z= Yaw
	FRotator tmpRotator = FRotator(0.f, 0.f, 0.f);
	FRotator tmpControlRotator = _tmp->GetController()->GetControlRotation();

	if (_roll)
		tmpRotator.Roll = tmpControlRotator.Roll;
	if (_pitch)
		tmpRotator.Pitch = tmpControlRotator.Pitch;
	if (_yaw)
		tmpRotator.Yaw = tmpControlRotator.Yaw;

	_tmp->GetController()->SetControlRotation(tmpRotator);
}

FTransform UOptitrackBPFunctionLibrary::GetRigidBodyTransform(int _ID, FString _Name, ERigidBodyIdentifierOptitrack _IdentifierMethod)
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(GetCorrectID(_Name, _ID, _IdentifierMethod));
}

int UOptitrackBPFunctionLibrary::GetCorrectID(FString _Name, int _ID, ERigidBodyIdentifierOptitrack _IdentifierMethod)
{
	int tmpID = _ID;
	if (_IdentifierMethod == ERigidBodyIdentifierOptitrack::RigidBodyName)
	{
		tmpID = FOptitrackPluginModule::GetOptiTrackSystem()->GetIdToName(_Name);
	}
	return tmpID;
}

FRotator UOptitrackBPFunctionLibrary::SetCurrentViewDirectionToForward(APawn* _Pawn)
{
	return Optitrack::Transformer::SetViewDirectionForward(_Pawn->GetControlRotation());;
}

FRotator UOptitrackBPFunctionLibrary::GetCurrentViewDirectionToForward()
{
	return Optitrack::Transformer::GetViewDirectionForward();
}
