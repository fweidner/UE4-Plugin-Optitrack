// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackBPFunctionLibrary.h"

#include "OptitrackPluginModule.h"

static FRotator ViewDirectionForward = FRotator();

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

	_tmpActor->SetActorLocation(tmpTransform.GetLocation());	
	_tmpActor->SetActorRotation(ConvertRotatorOfTransformFromLHStoRHS(tmpTransform));
	tmpTransform.SetScale3D(_tmpActor->GetActorScale3D());

	return tmpTransform;
}

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScaleSceneComponent(USceneComponent* _tmpSceneComponent, ECoordSystemsoptitrack _coordSystem, int _ID /*= 1*/, FString _Name /*= ""*/, ERigidBodyIdentifierOptitrack _IdentifierMethod /*= ERigidBodyIdentifierOptitrack::RigidBodyID*/)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(GetCorrectID(_Name, _ID, _IdentifierMethod));

	switch (_coordSystem)
	{
	case ECoordSystemsoptitrack::World:
	{
		_tmpSceneComponent->SetWorldLocation(tmpTransform.GetLocation());
		_tmpSceneComponent->SetWorldRotation(ConvertRotatorOfTransformFromLHStoRHS(tmpTransform));

		tmpTransform.SetScale3D(_tmpSceneComponent->GetComponentScale());
		return tmpTransform;
	}
	default:
	{
		return _tmpSceneComponent->GetComponentTransform(); //or return an empty transform?
	}
	}
}

FTransform UOptitrackBPFunctionLibrary::UpdateWithoutScalePawn(APawn* _Pawn, int _ID/*=1*/, FString _Name /*= ""*/, ERigidBodyIdentifierOptitrack _IdentifierMethod /*= ERigidBodyIdentifierOptitrack::RigidBodyID*/)
{
	FTransform tmpTransform = FOptitrackPluginModule::GetOptiTrackSystem()->GetRigidBodyTransform(GetCorrectID(_Name, _ID, _IdentifierMethod));
	
	if (_Pawn)
	{
		_Pawn->SetActorLocation(tmpTransform.GetLocation());

		FRotator tmpRotator = ConvertRotatorOfTransformFromLHStoRHS(tmpTransform);
		tmpRotator-=ViewDirectionForward;
		tmpTransform.SetRotation(tmpRotator.Quaternion());
		_Pawn->GetController()->SetControlRotation(tmpRotator);
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

FRotator UOptitrackBPFunctionLibrary::ConvertRotatorOfTransformFromLHStoRHS(FTransform _tmpTransform)
{
	FQuat tmpQuat = _tmpTransform.GetRotation();
	_tmpTransform.SetRotation(FQuat(tmpQuat.Y, tmpQuat.X, -tmpQuat.Z, tmpQuat.W));
	return _tmpTransform.Rotator();
}

FRotator UOptitrackBPFunctionLibrary::SetCurrentViewDirectionToForward(APawn* _Pawn)
{

	ViewDirectionForward = _Pawn->GetControlRotation();
	return ViewDirectionForward;
}

FRotator UOptitrackBPFunctionLibrary::GetCurrentViewDirectionToForward(APawn* _tmp)
{
	return ViewDirectionForward;
}
