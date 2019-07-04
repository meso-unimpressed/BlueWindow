// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "InteractiveSceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TraceEventActor.h"
#include "SceneView.h"
#include "DrawDebugHelpers.h"

void AInteractiveSceneCapture2D::ComputeViewInfo()
{
	UTextureRenderTarget2D* RenderTexture = CaptureComponent2D->TextureTarget;
	int w = 256; int h = 256;
	if (RenderTexture)
	{
		w = RenderTexture->SizeX;
		h = RenderTexture->SizeY;
	}

	// as everything basic in UE4, getting a view-projection matrix is also extremely unwieldy
	// https://answers.unrealengine.com/questions/158008/get-fsceneview-for-inactive-camera.html
	
	ViewInfo.Location = CaptureComponent2D->GetComponentTransform().GetLocation();
	ViewInfo.Rotation = CaptureComponent2D->GetComponentTransform().GetRotation().Rotator();
	ViewInfo.FOV = CaptureComponent2D->FOVAngle;
	ViewInfo.ProjectionMode = CaptureComponent2D->ProjectionType;
	ViewInfo.AspectRatio = float(w) / float(h);
	ViewInfo.OrthoNearClipPlane = 1;
	ViewInfo.OrthoFarClipPlane = RayLength;
	ViewInfo.OrthoWidth = CaptureComponent2D->OrthoWidth;
	ViewInfo.bConstrainAspectRatio = true;

	FIntRect ScreenRect(0, 0, w, h);
	ProjectionData.ViewOrigin = ViewInfo.Location;

	// do some voodoo rotation that is somehow mandatory.
	ProjectionData.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));

	if (CaptureComponent2D->bUseCustomProjectionMatrix == true) {
		ProjectionData.ProjectionMatrix = CaptureComponent2D->CustomProjectionMatrix;
	}
	else {
		ProjectionData.ProjectionMatrix = ViewInfo.CalculateProjectionMatrix();
	}
	ProjectionData.SetConstrainedViewRectangle(ScreenRect);
	ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
}

void AInteractiveSceneCapture2D::CastPointerRay(FPointerRay& ray, FVector2D NormalizedCoords, bool begin)
{
	FVector s, e;
	ComputeRayForViewProjection(ViewProjectionMatrix, NormalizedCoords, s, e);

	ray.CoordinateOnWidget = NormalizedCoords;

	// get previous actor which was hit by this ray (if there's one) for leave / enter event comparison
	AActor* prevActor = ray.Hit ? ray.HitResult.GetActor() : nullptr;
	uint32 prevActorId = prevActor ? prevActor->GetUniqueID() : 0;
	ATraceEventActor* prevTraceActor = nullptr;
	if (prevActor) prevTraceActor = Cast<ATraceEventActor>(prevActor);

	// Set up collision parameters
	FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActors(ActorsToIgnore);
	CollisionParams.bReturnFaceIndex = false; // TODO: make parameter
	CollisionParams.bReturnPhysicalMaterial = false; // TODO: make parameter
	CollisionParams.bTraceComplex = TraceComplex;

	// Trace the line
	ray.Hit = GetWorld()->LineTraceSingleByObjectType(
		ray.HitResult, s, e,
		FCollisionObjectQueryParams::DefaultObjectQueryParam,
		CollisionParams);

	if (DrawDebugHelpers)
	{
		DrawDebugLine(GetWorld(), s, e, ray.Hit ? FColor(0, 255, 0) : FColor(255, 0, 0));
		DrawDebugPoint(GetWorld(), s, 20, FColor(255, 255, 0));
		DrawDebugPoint(GetWorld(), ray.HitResult.ImpactPoint, 20, FColor(0, 0, 255));
	}

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
				actor->NotifyActorOnInputTouchBegin(ray.FingerIndex.GetValue());
				ray.BeganOnActor = actor;
			}
			else if (leaveEnter)
			{
				actor->NotifyActorOnInputTouchEnter(ray.FingerIndex.GetValue());
				if (prevActor) prevActor->NotifyActorOnInputTouchLeave(ray.FingerIndex.GetValue());
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
		if (prevActor) prevActor->NotifyActorOnInputTouchLeave(ray.FingerIndex.GetValue());
		if (prevTraceActor) prevTraceActor->NotifyActorOnLineTraceHitLeave(ray);
	}
}

// Sets default values
AInteractiveSceneCapture2D::AInteractiveSceneCapture2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("NewSceneCaptureComponent2D"));
	CaptureComponent2D->SetupAttachment(RootComponent);
}

void AInteractiveSceneCapture2D::ComputeRayForViewProjection(FMatrix ViewProj, FVector2D NormCoord, FVector& Start, FVector& End)
{
	FVector screenStart(NormCoord.X, NormCoord.Y, 1);
	FVector screenEnd(NormCoord.X, NormCoord.Y, 0.001);
	FVector4 Start4 = ViewProj.InverseFast().TransformPosition(screenStart);
	FVector4 End4 = ViewProj.InverseFast().TransformPosition(screenEnd);
	Start = FVector(Start4.X, Start4.Y, Start4.Z) / Start4.W;
	End = FVector(End4.X, End4.Y, End4.Z) / End4.W;

	FVector dir = End - Start;
	dir.Normalize();
	End = Start + dir * RayLength;
}

// Called when the game starts or when spawned
void AInteractiveSceneCapture2D::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractiveSceneCapture2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ComputeViewInfo();

	for (auto kvp : CurrentPointers)
	{
		kvp.Value.Age(DeltaTime);
	}
}

void AInteractiveSceneCapture2D::BeginPointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());
	FVector2D localCoords = geometry.AbsoluteToLocal(pointerEvent.GetScreenSpacePosition());
	FVector2D normCoords = (localCoords / geometry.GetLocalSize() - 0.5) * FVector2D(2, -2);

	BeginPointer(srcid, FingerIndex, normCoords, pointerEvent);
}

void AInteractiveSceneCapture2D::BeginPointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (CurrentPointers.Contains(currid))
	{
		MovePointer(SourceId, FingerIndex, NormalizedCoords, pointerEvent);
		return;
	}

	FPointerRay ray;
	ray.FingerIndex = FingerIndex;
	ray.UniqueId = currid;
	ray.SourceId = SourceId;
	ray.SourceSceneCapture = this;
	ray.PointerEvent = pointerEvent;

	CastPointerRay(ray, NormalizedCoords, true);
	CurrentPointers.Add(currid, ray);
}

void AInteractiveSceneCapture2D::MovePointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());
	FVector2D localCoords = geometry.AbsoluteToLocal(pointerEvent.GetScreenSpacePosition());
	FVector2D normCoords = (localCoords / geometry.GetLocalSize() - 0.5) * FVector2D(2, -2);

	MovePointer(srcid, FingerIndex, normCoords, pointerEvent);
}

void AInteractiveSceneCapture2D::MovePointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (!CurrentPointers.Contains(currid))
	{
		BeginPointer(SourceId, FingerIndex, NormalizedCoords, pointerEvent);
		return;
	}

	FPointerRay ray = CurrentPointers[currid];
	ray.PointerEvent = pointerEvent;
	CastPointerRay(ray, NormalizedCoords, false);

	if (ray.BeganOnActor)
	{
		ATraceEventActor* traceActor = Cast<ATraceEventActor>(ray.BeganOnActor);
		if (TriggerMoveEventOnOriginatedActor && traceActor)
			traceActor->NotifyActorOnLineTraceHitMove(ray);
	}

	CurrentPointers[currid] = ray;
}

void AInteractiveSceneCapture2D::EndPointer(UWidget* SourceWidget, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());

	EndPointer(srcid, FingerIndex);
}

void AInteractiveSceneCapture2D::EndPointer(int SourceId, ETouchIndex::Type FingerIndex)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (!CurrentPointers.Contains(currid)) return;

	FPointerRay ray = CurrentPointers[currid];
	if (ray.BeganOnActor)
	{
		ATraceEventActor* traceActor = Cast<ATraceEventActor>(ray.BeganOnActor);
		if (TriggerEndEventOnOriginatedActor)
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
	CurrentPointers.Remove(currid);
}

void AInteractiveSceneCapture2D::OnInterpToggle(bool bEnable)
{
	CaptureComponent2D->SetVisibility(bEnable);
}

void FPointerRay::Age(float delta)
{
	AgeInFrames++;
	AgeInSeconds += delta;
}
