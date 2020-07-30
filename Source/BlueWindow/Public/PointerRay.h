// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PointerRay.generated.h"

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

	void Age(float delta);
};