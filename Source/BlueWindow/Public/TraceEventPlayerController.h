// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "PointerRay.h"
#include "Events.h"
#include "UserWidget.h"

#include "TraceEventPlayerController.generated.h"

class UTraceEventLocalPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerPointerRayEventSignature, FPointerRay, Pointer);

/**
 * 
 */
UCLASS()
class BLUEWINDOW_API ATraceEventPlayerController : public APlayerController
{
	GENERATED_BODY()
private:

	void CastPointerRay(FPointerRay& ray, FVector2D NormalizedCoords, bool begin, bool fromLocalPlayer);
	void ComputeRayForViewProjection(FVector2D NormalizedCoords, FVector& Start, FVector& End);

	TSet<FKey> PressedButtonsDummy;
	FModifierKeysState ModifierKeysDummy;

protected:

	FVector2D NormalizeScreenCoords(FPointerEvent Pointer);

	FPointerEvent GetPointerEvent(uint32 Handle, FVector2D TouchLocation);
	FPointerEvent GetPointerEventWithDelta(uint32 Handle, FVector2D TouchLocation);

	TMap<uint32, FVector2D> LastTouchLocations;

public:

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|ManagableViewport")
		TMap<int, FPointerRay> CurrentPointers;

	UFUNCTION(BlueprintPure, Category = "BlueWindow|ManagableViewport")
		FMatrix GetViewProjectionMatrix();

	virtual bool InputTouch(
		uint32 Handle,
		ETouchType::Type Type,
		const FVector2D& TouchLocation,
		float Force,
		FDateTime DeviceTimestamp,
		uint32 TouchpadIndex
	) override;

	void BeginPointer(
		int SourceId,
		ETouchIndex::Type FingerIndex,
		FVector2D NormalizedCoords,
		FPointerEvent pointerEvent,
		bool fromLocalPlayer
	);

	void MovePointer(
		int SourceId,
		ETouchIndex::Type FingerIndex,
		FVector2D NormalizedCoords,
		FPointerEvent pointerEvent,
		bool fromLocalPlayer
	);

	void EndPointer(int SourceId, ETouchIndex::Type FingerIndex, bool fromLocalPlayer);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|ManagableViewport")
		void BeginPointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|ManagableViewport")
		void MovePointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|ManagableViewport")
		void EndPointer(UWidget* SourceWidget, FPointerEvent pointerEvent);

	UFUNCTION()
		void TouchBegin(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FPlayerPointerRayEventSignature OnTouchBegin;

	UFUNCTION()
		void TouchFirstMove(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FPlayerPointerRayEventSignature OnTouchFirstMove;

	UFUNCTION()
		void TouchMove(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FPlayerPointerRayEventSignature OnTouchMove;

	UFUNCTION()
		void TouchStationary(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FPlayerPointerRayEventSignature OnTouchStationary;

	UFUNCTION()
		void TouchEnded(FPointerEvent Pointer);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FPlayerPointerRayEventSignature OnTouchEnded;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|ManagableViewport")
		float RayLength = 250000;
};
