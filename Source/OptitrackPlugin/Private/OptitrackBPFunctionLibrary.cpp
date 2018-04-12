// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackBPFunctionLibrary.h"

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

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScaleActor(AActor* _tmpActor, int _ID)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(_ID);

	_tmpActor->SetActorLocation(tmpTransform.GetLocation());
	_tmpActor->SetActorRotation(tmpTransform.GetRotation());

	tmpTransform.SetScale3D(_tmpActor->GetActorScale3D());

	return tmpTransform;
}

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScaleSceneComponent(USceneComponent* _tmpSceneComponent, ECoordSystemsoptitrack _coordSystem, int _ID /*= 1*/)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(_ID);

	switch (_coordSystem)
	{
	case ECoordSystemsoptitrack::World:
	{
		_tmpSceneComponent->SetWorldLocation(tmpTransform.GetLocation());
		_tmpSceneComponent->SetWorldRotation(tmpTransform.GetRotation());
		tmpTransform.SetScale3D(_tmpSceneComponent->GetComponentScale());
		return tmpTransform;
	}
	default:
	{
		return _tmpSceneComponent->GetComponentTransform(); //or return an empty transform?
	}
	}
}

void UOptitrackBPFunctionLibrary::UpdateWithoutScalePlayer(APawn* _tmp, int _ID)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(_ID);
	
	if (_tmp)
	{
		_tmp->SetActorLocation(tmpTransform.GetLocation());
		_tmp->GetController()->SetControlRotation(tmpTransform.GetRotation().Rotator());
	}
}

void UOptitrackBPFunctionLibrary::ResetRotationPlayer(APawn* _tmp, bool _yaw, bool _pitch, bool _roll)
{

	//X= Roll, Y= Pitch, Z= Yaw
	FRotator tmpRotator = FRotator();
	FRotator tmpControlRotator = _tmp->GetController()->GetControlRotation();

	if (_roll)
		tmpRotator.Roll = tmpControlRotator.Roll;
	if (_pitch)
		tmpRotator.Pitch = tmpControlRotator.Pitch;
	if (_yaw)
		tmpRotator.Yaw = tmpControlRotator.Yaw;

	_tmp->GetController()->SetControlRotation(tmpRotator);
}

FTransform UOptitrackBPFunctionLibrary::GetRigidBodyTransform(int _ID)
{
	return FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(_ID);
}

