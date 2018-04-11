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

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Connect"))
		static void AutoConnect();

	UFUNCTION(BlueprintCallable, Category = "OptiTrack", meta = (Keywords = "NatNet OptiTrack Reset"))
		static void ResetClient();


	
	
};
