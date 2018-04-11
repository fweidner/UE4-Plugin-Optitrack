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
	
	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Test"))
		static void NatNetTest();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Connect Init"))
		static void ConnectAndInit();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Connect"))
		static void AutoConnect();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Disconnect"))
		static void Disconnect();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Reset"))
		static void ResetClient();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Init"))
		static void InitClient();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack FrameRate"))
		static float GetFrameRate();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack GetDataDescription"))
		static void GetDataDescription();

	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack GetDataDescription"))
		static FTransform GetRigidBodyTransform(int _ID);

	
};
