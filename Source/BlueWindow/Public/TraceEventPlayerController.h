// Developed by MESO Digital Interiors GmbH. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "TraceEventSourceBase.h"
#include "PointerRay.h"

#include "Input/Events.h"
#include "Blueprint/UserWidget.h"

#include "TraceEventPlayerController.generated.h"

class UTraceEventLocalPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerPointerRayEventSignature, FPointerRay, Pointer);

/**
 * 
 */
UCLASS()
class BLUEWINDOW_API ATraceEventPlayerController
    : public APlayerController
	, public ITraceEventSourceBase
{
	GENERATED_BODY()
private:

	TSet<FKey> PressedButtonsDummy;
	FModifierKeysState ModifierKeysDummy;

	FVector2D VpSize;
	FVector2D LastVpTouchLocation;

protected:

	FVector2D NormalizeScreenCoords(FPointerEvent Pointer);

	FPointerEvent GetPointerEvent(uint32 Handle, FVector2D TouchLocation);
	FPointerEvent GetPointerEventWithDelta(uint32 Handle, FVector2D TouchLocation);
	virtual bool ShouldDrawDebugHelpers() override;

	TMap<uint32, FVector2D> LastTouchLocations;

	virtual TMap<int, FPointerRay>& GetCurrentPointers() override;
	virtual int GetUuid_Internal() override;

public:

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow")
		TMap<int, FPointerRay> CurrentPointers;

	UFUNCTION(BlueprintPure, Category = "BlueWindow")
		virtual FMatrix GetViewProjectionMatrix() override;

	virtual bool InputTouch(
		uint32 Handle,
		ETouchType::Type Type,
		const FVector2D& TouchLocation,
		float Force,
		FDateTime DeviceTimestamp,
		uint32 TouchpadIndex
	) override;

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void BeginPointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void MovePointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void EndPointer(UWidget* SourceWidget, FPointerEvent pointerEvent);

	UFUNCTION()
		void TouchBegin(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPlayerPointerRayEventSignature OnTouchBegin;

	UFUNCTION()
		void TouchFirstMove(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPlayerPointerRayEventSignature OnTouchFirstMove;

	UFUNCTION()
		void TouchMove(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPlayerPointerRayEventSignature OnTouchMove;

	UFUNCTION()
		void TouchStationary(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPlayerPointerRayEventSignature OnTouchStationary;

	UFUNCTION()
		void TouchEnded(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPlayerPointerRayEventSignature OnTouchEnded;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow")
		float RayLength = 250000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
		bool DrawDebugHelpers;
};
