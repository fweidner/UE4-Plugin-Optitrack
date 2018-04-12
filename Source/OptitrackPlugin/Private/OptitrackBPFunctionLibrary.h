// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OptitrackBPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UOptitrackBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Test"))
		static void NatNetTest();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Connect Init"))
		static void ConnectAndInit();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Connect"))
		static void AutoConnect();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Disconnect"))
		static void Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Optitrack", meta = (Keywords = "NatNet Optitrack Reset"))
		static void ResetClient();

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
		static FTransform GetRigidBodyTransform(int _ID);


};
