// Developed by MESO Digital Interiors GmbH. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputCoreTypes.h"
#include "Engine/EngineTypes.h"

#include <tuple>

#include "TraceEventSourceBase.generated.h"

struct FPointerRayOptions;
struct FPointerRay;

UINTERFACE(BlueprintType)
class BLUEWINDOW_API UTriggerMoveEventOnOriginatedActorTag : public UInterface
{
    GENERATED_BODY()
};

class ITriggerMoveEventOnOriginatedActorTag
{
    GENERATED_BODY()
};

UINTERFACE(BlueprintType)
class BLUEWINDOW_API UTriggerEndEventOnOriginatedActorTag : public UInterface
{
    GENERATED_BODY()
};

class ITriggerEndEventOnOriginatedActorTag
{
    GENERATED_BODY()
};

UINTERFACE()
class BLUEWINDOW_API UTraceEventSourceBase : public UInterface
{
    GENERATED_BODY()
};

class BLUEWINDOW_API ITraceEventSourceBase
{
    GENERATED_BODY()

protected:
    static std::tuple<FVector, FVector> ComputeRayForViewProjection(FMatrix ViewProj, FVector2D NormCoord, const FPointerRayOptions& RayOptions);

    virtual FMatrix GetViewProjectionMatrix() = 0;
    virtual TMap<int, FPointerRay>& GetCurrentPointers() = 0;
    virtual int GetUuid_Internal() = 0;

    virtual bool ShouldDrawDebugHelpers() { return false; }
    virtual bool ShouldTriggerMoveEventOnOriginatedActor(AActor* Target);
    virtual bool ShouldTriggerEndEventOnOriginatedActor(AActor* Target);
    virtual void CastPointerRay(UWorld* World, FPointerRay& ray, FVector2D NormalizedCoords, bool begin);

    virtual void BeginPointer_Internal(UWorld* World, int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent, const FPointerRayOptions& RayOptions);
    virtual void MovePointer_Internal(UWorld* World, int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent, const FPointerRayOptions& RayOptions);
    virtual void EndPointer_Internal(UWorld* World, int SourceId, ETouchIndex::Type FingerIndex);

    void TraceEventTick(float DeltaTime);

    bool TraceResultPredicate(FHitResult Hit);
};