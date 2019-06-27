// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "InteractiveSceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TraceEventActor.h"
#include "SceneView.h"

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
	ViewInfo.OrthoFarClipPlane = 1000;
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
					if (prevActor) prevTraceActor->NotifyActorOnLineTraceHitLeave(ray);
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
AInteractiveSceneCapture2D::AInteractiveSceneCapture2D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractiveSceneCapture2D::ComputeRayForViewProjection(FMatrix ViewProj, FVector2D NormCoord, FVector& Start, FVector& End)
{
	FVector4 screenStart(NormCoord.X, NormCoord.Y, 0, 1);
	FVector4 screenEnd(NormCoord.X, NormCoord.Y, 1, 1);
	FVector4 Start4 = ViewProj.InverseFast().TransformFVector4(screenStart);
	FVector4 End4 = ViewProj.InverseFast().TransformFVector4(screenStart);
	Start = FVector(Start4.X, Start4.Y, Start4.Z) / Start4.W;
	End = FVector(End4.X, End4.Y, End4.Z) / End4.W;
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

void AInteractiveSceneCapture2D::BeginPointer(UWidget* SourceWidget, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	BeginPointer(srcid, FingerIndex, NormalizedCoords);
}

void AInteractiveSceneCapture2D::BeginPointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (CurrentPointers.Contains(currid))
	{
		MovePointer(SourceId, FingerIndex, NormalizedCoords);
		return;
	}

	FPointerRay ray;
	ray.FingerIndex = FingerIndex;
	ray.UniqueId = currid;
	ray.SourceId = SourceId;
	ray.SourceSceneCapture = this;

	CastPointerRay(ray, NormalizedCoords, true);
	CurrentPointers.Add(currid, ray);
}

void AInteractiveSceneCapture2D::MovePointer(UWidget* SourceWidget, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	MovePointer(srcid, FingerIndex, NormalizedCoords);
}

void AInteractiveSceneCapture2D::MovePointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (!CurrentPointers.Contains(currid))
	{
		BeginPointer(SourceId, FingerIndex, NormalizedCoords);
		return;
	}

	FPointerRay ray = CurrentPointers[currid];
	CastPointerRay(ray, NormalizedCoords, false);
	CurrentPointers[currid] = ray;
}

void AInteractiveSceneCapture2D::EndPointer(UWidget* SourceWidget, ETouchIndex::Type FingerIndex)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	EndPointer(srcid, FingerIndex);
}

void AInteractiveSceneCapture2D::EndPointer(int SourceId, ETouchIndex::Type FingerIndex)
{
	int currid = SourceId ^ ((int)GetUniqueID()) ^ ((int)FingerIndex);
	if (!CurrentPointers.Contains(currid)) return;

	FPointerRay ray = CurrentPointers[currid];
	if (ray.Hit)
	{
		AActor* actor = ray.HitResult.GetActor();
		ATraceEventActor* traceActor = actor ? Cast<ATraceEventActor>(actor) : nullptr;

		if (actor) actor->NotifyActorOnInputTouchEnd(ray.FingerIndex.GetValue());
		if (traceActor) traceActor->NotifyActorOnLineTraceHitEnd(ray);
	}
	CurrentPointers.Remove(currid);
}

void FPointerRay::Age(float delta)
{
	AgeInFrames++;
	AgeInSeconds += delta;
}
