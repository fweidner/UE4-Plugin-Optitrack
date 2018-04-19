// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OptitrackBPFunctionLibrary.generated.h"


/**
 * 
 */

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ECoordSystemsoptitrack : uint8
{
	World 	UMETA(DisplayName = "adjust in world space"),
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ERigidBodyIdentifierOptitrack: uint8
{
	RigidBodyName 	UMETA(DisplayName = "RigidBody Name"),
	RigidBodyID		UMETA(DisplayName = "RigidBody ID"),
};



UCLASS()
class UOptitrackBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

//////////////////////////////////////////////////////////////////////////
	
	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Test"))
		static void NatNetTest();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Connect Init"))
		static void AutoConnectAndInit();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Connect"))
		static void AutoConnect();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Disconnect"))
		static void DisconnectAndResetClient();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Reset"))
		static void DisconnectClient();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Init"))
		static void InitClient();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack FrameRate"))
		static float GetFrameRate();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Units Millimeter"))
		static float GetUnitsToMillimeter();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack GetDataDescription"))
		static void GetDataDescription();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Print Debug Messages"))
		static bool SetPrintDebugMessages(bool _newVal);


	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack GetDataDescription"))
		static FTransform GetRigidBodyTransform(int _ID = 1, FString _Name = "", ERigidBodyIdentifierOptitrack _IdentifierMethod = ERigidBodyIdentifierOptitrack::RigidBodyID);

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Update Actor Scale"))
		static FTransform UpdateWithoutScaleActor(AActor* _tmpActor, int _ID = 1, FString _Name = "", ERigidBodyIdentifierOptitrack _IdentifierMethod = ERigidBodyIdentifierOptitrack::RigidBodyID);

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Update Actor Scale"))
		static FTransform UpdateWithoutScaleSceneComponent(USceneComponent* _tmpSceneComponent, ECoordSystemsoptitrack _coordSystem, int _ID = 1, FString _Name = "", ERigidBodyIdentifierOptitrack _IdentifierMethod = ERigidBodyIdentifierOptitrack::RigidBodyID);

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Update Character Scale"))
		static FTransform UpdateWithoutScalePawn(APawn* _tmp, int _ID=1, FString _Name = "", ERigidBodyIdentifierOptitrack _IdentifierMethod = ERigidBodyIdentifierOptitrack::RigidBodyID);

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Update Reset Control Rotation"))
		static void ResetRotationPlayer(APawn* _tmp, bool _yaw = true, bool _pitch=true, bool _roll=true);

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Set View Direction Forward"))
		static FRotator SetCurrentViewDirectionToForward(APawn* _Pawn);

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Get View Direction Forward"))
		static FRotator GetCurrentViewDirectionToForward(APawn* _tmp);

private:

		static int GetCorrectID(FString _Name, int _ID, ERigidBodyIdentifierOptitrack _IdentifierMethod);
		static FRotator ConvertRotatorOfTransformFromLHStoRHS(FTransform _tmpTransform);
};
