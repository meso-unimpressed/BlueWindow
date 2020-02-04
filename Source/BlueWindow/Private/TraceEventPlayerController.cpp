// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "TraceEventPlayerController.h"
#include "SceneView.h"
#include "Engine/GameViewportClient.h"
#include "ManagableGameViewportClient.h"
#include "Engine/Engine.h"
#include "TraceEventActor.h"
#include "Components/PrimitiveComponent.h"
#include "BlueWindowBPLibrary.h"

void ATraceEventPlayerController::CastPointerRay(FPointerRay& ray, FVector2D NormalizedCoords, bool begin, bool fromLocalPlayer)
{

	FVector s, e;
	ComputeRayForViewProjection(NormalizedCoords, s, e);

	ray.CoordinateOnWidget = NormalizedCoords;

	// get previous actor which was hit by this ray (if there's one) for leave / enter event comparison
	AActor* prevActor = ray.Hit ? ray.HitResult.GetActor() : nullptr;
	uint32 prevActorId = prevActor ? prevActor->GetUniqueID() : 0;
	ATraceEventActor* prevTraceActor = nullptr;
	if (prevActor) prevTraceActor = Cast<ATraceEventActor>(prevActor);

	TArray<FHitResult> tempHits;
	FTraceResultFilterDelegate filter;
	filter.BindDynamic(this, &ATraceEventPlayerController::TraceResultPredicate);

	ray.Hit = UBlueWindowBPLibrary::LineTraceFiltered(
		GetWorld(), s, e, filter, tempHits, ray.HitResult);

	//DrawDebugLineTraceSingle(GetWorld(), s, e, DrawDebugType.GetValue(), ray.Hit, ray.HitResult, TraceColor, TraceHitColor, DrawTime);

	if (ray.Hit)
	{
		AActor* actor = ray.HitResult.GetActor();
		if (actor)
		{
			// Get the hit actor and also get if it's an actor supporting LineTrace events
			uint32 actorId = actor ? actor->GetUniqueID() : 0;
			ATraceEventActor* traceActor = Cast<ATraceEventActor>(actor);

			// leave / enter when the previous and the current actor ID's don't match
			bool leaveEnter = !begin && prevActorId != actorId;

			// Actuate events on the hit and previous actor
			if (begin)
			{
				if(!fromLocalPlayer) actor->NotifyActorOnInputTouchBegin(ray.FingerIndex.GetValue());
				ray.BeganOnActor = actor;
			}
			else if (leaveEnter)
			{
				if (!fromLocalPlayer)
				{
					actor->NotifyActorOnInputTouchEnter(ray.FingerIndex.GetValue());
					if (prevActor) prevActor->NotifyActorOnInputTouchLeave(ray.FingerIndex.GetValue());
				}
				if (prevTraceActor) prevTraceActor->NotifyActorOnLineTraceHitLeave(ray);
			}

			if (traceActor)
			{
				if (begin)
				{
					traceActor->NotifyActorOnLineTraceHitBegin(ray);
				}
				else if (leaveEnter)
				{
					traceActor->NotifyActorOnLineTraceHitEnter(ray);
				}
				else // on move
				{
					traceActor->NotifyActorOnLineTraceHitMove(ray);
				}
			}
		}
	}
	else
	{
		// notify previous actor that this line left them
		if (prevActor && !fromLocalPlayer) prevActor->NotifyActorOnInputTouchLeave(ray.FingerIndex.GetValue());
		if (prevTraceActor) prevTraceActor->NotifyActorOnLineTraceHitLeave(ray);
	}
}

void ATraceEventPlayerController::ComputeRayForViewProjection(FVector2D NormalizedCoords, FVector& Start, FVector& End)
{
	FMatrix ivp = GetViewProjectionMatrix().InverseFast();

	FVector screenStart(NormalizedCoords.X, NormalizedCoords.Y, 1);
	FVector screenEnd(NormalizedCoords.X, NormalizedCoords.Y, 0.001);
	FVector4 Start4 = ivp.TransformPosition(screenStart);
	FVector4 End4 = ivp.TransformPosition(screenEnd);
	Start = FVector(Start4.X, Start4.Y, Start4.Z) / Start4.W;
	End = FVector(End4.X, End4.Y, End4.Z) / End4.W;

	FVector dir = End - Start;
	dir.Normalize();
	End = Start + dir * RayLength;
}

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

bool ATraceEventPlayerController::TraceResultPredicate(FHitResult Hit)
{
	bool ignore = false;
	if (Hit.Actor.IsValid())
		ignore = Hit.Actor->bHidden || Hit.Actor->Tags.Contains(TEXT("TraceIgnore"));
	if (!ignore && Hit.Component.IsValid())
		ignore = Hit.Component->bHiddenInGame || Hit.Component->ComponentTags.Contains(TEXT("TraceIgnore"));
	return !ignore;
}

void ATraceEventPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto kvp : CurrentPointers)
	{
		kvp.Value.Age(DeltaTime);
	}
}

// void ATraceEventPlayerController::PlayerAdded(UGameViewportClient* InViewportClient, int32 InControllerID)
// {
// 	ULocalPlayer::PlayerAdded(InViewportClient, InControllerID);
// 
// 	UManagableGameViewportClient* manvp = Cast<UManagableGameViewportClient>(InViewportClient);
// 	if (!manvp) return;
// 
// 	manvp->OnTouchBegin.AddDynamic(this, &ATraceEventPlayerController::TouchBegin);
// 	manvp->OnTouchFirstMove.AddDynamic(this, &ATraceEventPlayerController::TouchFirstMove);
// 	manvp->OnTouchMove.AddDynamic(this, &ATraceEventPlayerController::TouchMove);
// 	manvp->OnTouchStationary.AddDynamic(this, &ATraceEventPlayerController::TouchStationary);
// 	manvp->OnTouchEnded.AddDynamic(this, &ATraceEventPlayerController::TouchEnded);
// }

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

	BeginPointer(srcid, FingerIndex, normCoords, pointerEvent, false);
}

void ATraceEventPlayerController::BeginPointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent, bool fromLocalPlayer)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (CurrentPointers.Contains(currid))
	{
		MovePointer(SourceId, FingerIndex, NormalizedCoords, pointerEvent, fromLocalPlayer);
		return;
	}

	FPointerRay ray;
	ray.FingerIndex = FingerIndex;
	ray.UniqueId = currid;
	ray.SourceId = SourceId;
	//ray.SourceSceneCapture = this;
	ray.PointerEvent = pointerEvent;

	CastPointerRay(ray, NormalizedCoords, true, fromLocalPlayer);
	CurrentPointers.Add(currid, ray);
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

	MovePointer(srcid, FingerIndex, normCoords, pointerEvent, false);
}

void ATraceEventPlayerController::MovePointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent, bool fromLocalPlayer)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (!CurrentPointers.Contains(currid))
	{
		BeginPointer(SourceId, FingerIndex, NormalizedCoords, pointerEvent, fromLocalPlayer);
		return;
	}

	FPointerRay ray = CurrentPointers[currid];
	ray.PointerEvent = pointerEvent;
	CastPointerRay(ray, NormalizedCoords, false, fromLocalPlayer);

	if (ray.BeganOnActor)
	{
		ATraceEventActor* traceActor = Cast<ATraceEventActor>(ray.BeganOnActor);
		//if (TriggerMoveEventOnOriginatedActor && traceActor)
		//    traceActor->NotifyActorOnLineTraceHitMove(ray);
	}

	CurrentPointers[currid] = ray;
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

	EndPointer(srcid, FingerIndex, false);
}

void ATraceEventPlayerController::EndPointer(int SourceId, ETouchIndex::Type FingerIndex, bool fromLocalPlayer)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (!CurrentPointers.Contains(currid)) return;

	FPointerRay ray = CurrentPointers[currid];
	if (ray.BeganOnActor)
	{
		ATraceEventActor* traceActor = Cast<ATraceEventActor>(ray.BeganOnActor);
		//if (!fromLocalPlayer)
		//    ray.BeganOnActor->NotifyActorOnInputTouchEnd(ray.FingerIndex.GetValue());
		if (traceActor)
			traceActor->NotifyActorOnOriginatedLineTraceEnd(ray);
	}
	if (ray.Hit)
	{
		AActor* actor = ray.HitResult.GetActor();
		ATraceEventActor* traceActor = actor ? Cast<ATraceEventActor>(actor) : nullptr;

		if (actor) actor->NotifyActorOnInputTouchEnd(ray.FingerIndex.GetValue());
		if (traceActor) traceActor->NotifyActorOnLineTraceHitEnd(ray);
	}
	CurrentPointers.Remove(currid);
}

void ATraceEventPlayerController::TouchBegin(FPointerEvent Pointer)
{
	int srcid = (int)GetLocalPlayer()->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)Pointer.GetPointerIndex());

	BeginPointer(srcid, FingerIndex, NormalizeScreenCoords(Pointer), Pointer, true);
}

void ATraceEventPlayerController::TouchFirstMove(FPointerEvent Pointer)
{
	//TODO
}

void ATraceEventPlayerController::TouchMove(FPointerEvent Pointer)
{
	int srcid = (int)GetLocalPlayer()->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)Pointer.GetPointerIndex());

	MovePointer(srcid, FingerIndex, NormalizeScreenCoords(Pointer), Pointer, true);
}

void ATraceEventPlayerController::TouchStationary(FPointerEvent Pointer)
{
	//TODO ?
}

void ATraceEventPlayerController::TouchEnded(FPointerEvent Pointer)
{
	int srcid = (int)GetLocalPlayer()->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)Pointer.GetPointerIndex());

	EndPointer(srcid, FingerIndex, true);
}
