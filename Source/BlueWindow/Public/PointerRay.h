// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PointerRay.generated.h"

class ITraceEventSourceBase;

USTRUCT(BlueprintType)
struct BLUEWINDOW_API FPointerRayOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
	TScriptInterface<ITraceEventSourceBase> RaySource;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
	float RayLength;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
	bool bFromLocalPlayer;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
	bool bIsMouse;

	FPointerRayOptions()
	    : RaySource(nullptr)
		, RayLength(250000)
		, bFromLocalPlayer(false)
		, bIsMouse(false)
	{
	}

    FPointerRayOptions(const TScriptInterface<ITraceEventSourceBase>& RaySource, float RayLength, bool bFromLocalPlayer)
		: RaySource(RaySource)
		, RayLength(RayLength)
		, bFromLocalPlayer(bFromLocalPlayer)
		, bIsMouse(false)
	{
	}

	FPointerRayOptions(const TScriptInterface<ITraceEventSourceBase>& RaySource, float RayLength, bool bFromLocalPlayer, bool bIsMouse)
		: RaySource(RaySource)
		, RayLength(RayLength)
		, bFromLocalPlayer(bFromLocalPlayer)
		, bIsMouse(bIsMouse)
	{
	}
};

USTRUCT(BlueprintType)
struct BLUEWINDOW_API FPointerRay
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		TEnumAsByte<ETouchIndex::Type> FingerIndex;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		bool Hit = false;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		FHitResult HitResult;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		FVector2D CoordinateOnWidget;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		AActor* BeganOnActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		FPointerEvent PointerEvent;

	/**
	 * Computed by SourceId ^ SourceSceneCapture ^ ((int)FingerIndex)
	 */
	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		int UniqueId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		int SourceId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		int AgeInFrames = 0;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		float AgeInSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
	    FPointerRayOptions Options;

	void Age(float delta);
};