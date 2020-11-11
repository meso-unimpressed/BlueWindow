// Developed by MESO Digital Interiors GmbH. All rights reserved.

#include "TraceEventSourceBase.h"

#include "BlueWindowBPLibrary.h"
#include "DrawDebugHelpers.h"
#include "PointerRay.h"
#include "TraceEventActor.h"

#include "Components/PrimitiveComponent.h"

std::tuple<FVector, FVector> ITraceEventSourceBase::ComputeRayForViewProjection(FMatrix ViewProj, FVector2D NormCoord, const FPointerRayOptions& RayOptions)
{
	FVector screenStart(NormCoord.X, NormCoord.Y, 1);
	FVector screenEnd(NormCoord.X, NormCoord.Y, 0.001);
	FVector4 Start4 = ViewProj.InverseFast().TransformPosition(screenStart);
	FVector4 End4 = ViewProj.InverseFast().TransformPosition(screenEnd);
	auto Start = FVector(Start4.X, Start4.Y, Start4.Z) / Start4.W;
	auto End = FVector(End4.X, End4.Y, End4.Z) / End4.W;

	FVector dir = End - Start;
	dir.Normalize();
	End = Start + dir * RayOptions.RayLength;
	return {Start, End};
}

bool ITraceEventSourceBase::ShouldTriggerMoveEventOnOriginatedActor(AActor* Target)
{
    if(!Target) return false;
    return Target->Implements<UTriggerMoveEventOnOriginatedActorTag>()
	    || Target->Tags.Contains(TEXT("TriggerMoveEventOnOriginatedActor"));
}

bool ITraceEventSourceBase::ShouldTriggerEndEventOnOriginatedActor(AActor* Target)
{
	if (!Target) return false;
	return Target->Implements<UTriggerEndEventOnOriginatedActorTag>()
		|| Target->Tags.Contains(TEXT("TriggerEndEventOnOriginatedActor"));
}

void ITraceEventSourceBase::CastPointerRay(UWorld* World, FPointerRay& ray, FVector2D NormalizedCoords, bool begin)
{
	auto [s, e] = ComputeRayForViewProjection(GetViewProjectionMatrix(), NormalizedCoords, ray.Options);

	ray.CoordinateOnWidget = NormalizedCoords;

	// get previous actor which was hit by this ray (if there's one) for leave / enter event comparison
	AActor* prevActor = ray.Hit ? ray.HitResult.GetActor() : nullptr;
	uint32 prevActorId = prevActor ? prevActor->GetUniqueID() : 0;
	ATraceEventActor* prevTraceActor = nullptr;
	if (prevActor) prevTraceActor = Cast<ATraceEventActor>(prevActor);

	TArray<FHitResult> tempHits;
	FTraceResultFilterStaticDel filter;
	filter.BindRaw(this, &ITraceEventSourceBase::TraceResultPredicate);

	ray.Hit = UBlueWindowBPLibrary::LineTraceFiltered(
		World, s, e, filter, tempHits, ray.HitResult);

	if (ShouldDrawDebugHelpers())
	{
		DrawDebugLine(World, s, e, ray.Hit ? FColor(0, 255, 0) : FColor(255, 0, 0));
		DrawDebugPoint(World, s, 20, FColor(255, 255, 0));
		DrawDebugPoint(World, ray.HitResult.ImpactPoint, 20, FColor(0, 0, 255));
	}

	//DrawDebugLineTraceSingle(World, s, e, DrawDebugType.GetValue(), ray.Hit, ray.HitResult, TraceColor, TraceHitColor, DrawTime);

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
				if (!ray.Options.bFromLocalPlayer) actor->NotifyActorOnInputTouchBegin(ray.FingerIndex.GetValue());
				ray.BeganOnActor = actor;
			}
			else if (leaveEnter)
			{
			    if (!ray.Options.bFromLocalPlayer)
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
		if (prevActor && !ray.Options.bFromLocalPlayer) prevActor->NotifyActorOnInputTouchLeave(ray.FingerIndex.GetValue());
		if (prevTraceActor) prevTraceActor->NotifyActorOnLineTraceHitLeave(ray);
	}
}

void ITraceEventSourceBase::BeginPointer_Internal(UWorld* World, int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent, const FPointerRayOptions& RayOptions)
{
	int currid = SourceId ^ GetUuid_Internal() ^ ((int)FingerIndex);
	if (GetCurrentPointers().Contains(currid))
	{
		MovePointer_Internal(World, SourceId, FingerIndex, NormalizedCoords, pointerEvent, RayOptions);
		return;
	}

	FPointerRay ray;
	ray.Options = RayOptions;
	ray.FingerIndex = FingerIndex;
	ray.UniqueId = currid;
	ray.SourceId = SourceId;
	//ray.SourceSceneCapture = this;
	ray.PointerEvent = pointerEvent;

	CastPointerRay(World, ray, NormalizedCoords, true);
	GetCurrentPointers().Add(currid, ray);
}

void ITraceEventSourceBase::MovePointer_Internal(UWorld* World, int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent, const FPointerRayOptions& RayOptions)
{
	int currid = SourceId ^ GetUuid_Internal() ^ ((int)FingerIndex);
	if (!GetCurrentPointers().Contains(currid))
	{
		BeginPointer_Internal(World, SourceId, FingerIndex, NormalizedCoords, pointerEvent, RayOptions);
		return;
	}

	FPointerRay ray = GetCurrentPointers()[currid];
	ray.PointerEvent = pointerEvent;
	CastPointerRay(World, ray, NormalizedCoords, false);

	ATraceEventActor* traceActor = Cast<ATraceEventActor>(ray.BeganOnActor);
	if (ShouldTriggerMoveEventOnOriginatedActor(traceActor))
		traceActor->NotifyActorOnLineTraceHitMove(ray);

	GetCurrentPointers()[currid] = ray;
}

void ITraceEventSourceBase::EndPointer_Internal(UWorld* World, int SourceId, ETouchIndex::Type FingerIndex)
{
	int currid = SourceId ^ GetUuid_Internal() ^ ((int)FingerIndex);
	if (!GetCurrentPointers().Contains(currid)) return;

	FPointerRay ray = GetCurrentPointers()[currid];
	if (ray.BeganOnActor)
	{
		ATraceEventActor* traceActor = Cast<ATraceEventActor>(ray.BeganOnActor);
		if (ShouldTriggerEndEventOnOriginatedActor(traceActor))
			ray.BeganOnActor->NotifyActorOnInputTouchEnd(ray.FingerIndex.GetValue());
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
	GetCurrentPointers().Remove(currid);
}

void ITraceEventSourceBase::TraceEventTick(float DeltaTime)
{
	for (auto& kvp : GetCurrentPointers())
	{
		kvp.Value.Age(DeltaTime);
	}
}

bool ITraceEventSourceBase::TraceResultPredicate(FHitResult Hit)
{
	bool ignore = false;
	AActor* hitActor = Hit.Actor.Get();
	if (Hit.Actor.IsValid())
		ignore = hitActor->IsHidden() || hitActor->Tags.Contains(TEXT("TraceIgnore"));
	if (!ignore && Hit.Component.IsValid())
		ignore = Hit.Component->ComponentTags.Contains(TEXT("TraceIgnore"));

#if 0 // enable only if the above alone causes trouble
	if (!ignore)
	{
		const auto& components = Hit.Actor->GetComponents();

		ignore = std::all_of(components.begin(), components.end(), [](UActorComponent* comp)
			{
				auto primComp = Cast<UPrimitiveComponent>(comp);
				if (primComp)
					return static_cast<bool>(primComp->bHiddenInGame);
				return true;
			});
	}
#endif
	return !ignore;
}
