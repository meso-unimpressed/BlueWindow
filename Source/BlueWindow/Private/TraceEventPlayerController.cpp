// Developed by MESO Digital Interiors GmbH. All rights reserved.

#include "TraceEventPlayerController.h"
#include "SceneView.h"
#include "Engine/GameViewportClient.h"
#include "ManagableGameViewportClient.h"
#include "Engine/Engine.h"
#include "TraceEventActor.h"
#include "Components/PrimitiveComponent.h"
#include "BlueWindowBPLibrary.h"

#include <algorithm>

FVector2D ATraceEventPlayerController::NormalizeScreenCoords(FPointerEvent Pointer)
{
	FVector2D localCoords = Pointer.GetScreenSpacePosition();
	FVector2D vpsize;
	GetLocalPlayer()->ViewportClient->GetViewportSize(vpsize);
	return (localCoords / vpsize - 0.5) * FVector2D(2, -2);
}

FPointerEvent ATraceEventPlayerController::GetPointerEvent(uint32 Handle, FVector2D TouchLocation)
{
	return FPointerEvent(
		Handle,
		TouchLocation,
		TouchLocation,
		FVector2D::ZeroVector,
		FTouchKeySet::EmptySet,
		ModifierKeysDummy
	);
}

FPointerEvent ATraceEventPlayerController::GetPointerEventWithDelta(uint32 Handle, FVector2D TouchLocation)
{
	if (!LastTouchLocations.Contains(Handle))
		LastTouchLocations.Add(Handle, TouchLocation);
	return FPointerEvent(
		Handle,
		TouchLocation,
		LastTouchLocations[Handle],
		TouchLocation - LastTouchLocations[Handle],
		FTouchKeySet::EmptySet,
		ModifierKeysDummy
	);
}

FMatrix ATraceEventPlayerController::GetViewProjectionMatrix()
{
	ULocalPlayer* const LP = GetLocalPlayer();
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		{
			return ProjectionData.ComputeViewProjectionMatrix();
		}
	}
	return FMatrix::Identity;
}

TMap<int, FPointerRay>& ATraceEventPlayerController::GetCurrentPointers()
{
	return CurrentPointers;
}

int ATraceEventPlayerController::GetUuid_Internal()
{
	return GetUniqueID();
}

bool ATraceEventPlayerController::ShouldDrawDebugHelpers()
{
	return DrawDebugHelpers;
}

void ATraceEventPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceEventTick(DeltaTime);
}

bool ATraceEventPlayerController::InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	bool res = APlayerController::InputTouch(Handle, Type, TouchLocation, Force, DeviceTimestamp, TouchpadIndex);

	FPointerEvent pevent;
	switch (Type)
	{
	case ETouchType::Began:
		pevent = GetPointerEvent(Handle, TouchLocation);
		TouchBegin(pevent);
		LastTouchLocations.Add(Handle, TouchLocation);
		break;

	case ETouchType::Moved:
		pevent = GetPointerEventWithDelta(Handle, TouchLocation);
		TouchMove(pevent);
		LastTouchLocations[Handle] = TouchLocation;
		break;

	case ETouchType::Stationary:
		pevent = GetPointerEventWithDelta(Handle, TouchLocation);
		//TODO
		LastTouchLocations[Handle] = TouchLocation;
		break;

	case ETouchType::ForceChanged:
		break;

	case ETouchType::FirstMove:
		pevent = GetPointerEventWithDelta(Handle, TouchLocation);
		//TODO
		LastTouchLocations[Handle] = TouchLocation;
		break;

	case ETouchType::Ended:
		pevent = GetPointerEvent(Handle, TouchLocation);
		TouchEnded(pevent);
		LastTouchLocations.Remove(Handle);
		break;

	default:
		break;
	}
	return res;
}

void ATraceEventPlayerController::BeginPointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());
	FVector2D localCoords = geometry.AbsoluteToLocal(pointerEvent.GetScreenSpacePosition());

	FVector2D oneCoords = localCoords / geometry.GetLocalSize();
	FVector2D normCoords = (oneCoords - 0.5) * FVector2D(2, -2);
	GEngine->GameViewport->GetViewportSize(VpSize);
	LastVpTouchLocation = oneCoords * VpSize;

	APlayerController::InputTouch(
		FingerIndex,
		ETouchType::Began,
		LastVpTouchLocation,
		pointerEvent.GetTouchForce(),
		FDateTime::UtcNow(), 0
	);

	BeginPointer_Internal(GetWorld(), srcid, FingerIndex, normCoords, pointerEvent, { this, RayLength, false });
}

void ATraceEventPlayerController::MovePointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());
	FVector2D localCoords = geometry.AbsoluteToLocal(pointerEvent.GetScreenSpacePosition());

	FVector2D oneCoords = localCoords / geometry.GetLocalSize();
	FVector2D normCoords = (oneCoords - 0.5) * FVector2D(2, -2);
	GEngine->GameViewport->GetViewportSize(VpSize);
	LastVpTouchLocation = oneCoords * VpSize;

	APlayerController::InputTouch(
		FingerIndex,
		ETouchType::Moved,
		LastVpTouchLocation,
		pointerEvent.GetTouchForce(),
		FDateTime::UtcNow(), 0
	);

	MovePointer_Internal(GetWorld(), srcid, FingerIndex, normCoords, pointerEvent, { this, RayLength, false });
}

void ATraceEventPlayerController::EndPointer(UWidget* SourceWidget, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());

	APlayerController::InputTouch(
		FingerIndex,
		ETouchType::Ended,
		LastVpTouchLocation,
		pointerEvent.GetTouchForce(),
		FDateTime::UtcNow(), 0
	);

	EndPointer_Internal(GetWorld(), srcid, FingerIndex);
}

void ATraceEventPlayerController::TouchBegin(FPointerEvent Pointer)
{
	int srcid = (int)GetLocalPlayer()->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)Pointer.GetPointerIndex());

	BeginPointer_Internal(GetWorld(), srcid, FingerIndex, NormalizeScreenCoords(Pointer), Pointer, { this, RayLength, true });
}

void ATraceEventPlayerController::TouchFirstMove(FPointerEvent Pointer)
{
	//TODO
}

void ATraceEventPlayerController::TouchMove(FPointerEvent Pointer)
{
	int srcid = (int)GetLocalPlayer()->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)Pointer.GetPointerIndex());

	MovePointer_Internal(GetWorld(), srcid, FingerIndex, NormalizeScreenCoords(Pointer), Pointer, { this, RayLength, true });
}

void ATraceEventPlayerController::TouchStationary(FPointerEvent Pointer)
{
	//TODO ?
}

void ATraceEventPlayerController::TouchEnded(FPointerEvent Pointer)
{
	int srcid = (int)GetLocalPlayer()->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)Pointer.GetPointerIndex());

	EndPointer_Internal(GetWorld(), srcid, FingerIndex);
}
