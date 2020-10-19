// Developed by MESO Digital Interiors GmbH. All rights reserved.

#include "InteractiveSceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TraceEventActor.h"
#include "SceneView.h"
#include "Components/PrimitiveComponent.h"
#include "BlueWindowBPLibrary.h"

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

// Sets default values
AInteractiveSceneCapture2D::AInteractiveSceneCapture2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("NewSceneCaptureComponent2D"));
	CaptureComponent2D->SetupAttachment(RootComponent);
}

void AInteractiveSceneCapture2D::DeprojectScreenToWorld(FVector2D NormalizedCoords, FVector& Start, FVector& End)
{
	auto res = ComputeRayForViewProjection(ViewProjectionMatrix, NormalizedCoords, {this, RayLength, false});
	Start = std::get<0>(res);
	End = std::get<1>(res);
}

FVector2D AInteractiveSceneCapture2D::ProjectWorldToScreen(FVector World, float& Depth)
{
	FVector4 res4 = ViewProjectionMatrix.TransformPosition(World);
	FVector res3 = FVector(res4.X, res4.Y, res4.Z) / res4.W;
	Depth = res3.Z;
	return FVector2D(res3.X, res3.Y);
}

// Called when the game starts or when spawned
void AInteractiveSceneCapture2D::BeginPlay()
{
	Super::BeginPlay();
}

FMatrix AInteractiveSceneCapture2D::GetViewProjectionMatrix()
{
    return ViewProjectionMatrix;
}

TMap<int, FPointerRay>& AInteractiveSceneCapture2D::GetCurrentPointers()
{
    return CurrentPointers;
}

int AInteractiveSceneCapture2D::GetUuid_Internal()
{
    return GetUniqueID();
}

bool AInteractiveSceneCapture2D::ShouldDrawDebugHelpers()
{
    return DrawDebugHelpers;
}

// Called every frame
void AInteractiveSceneCapture2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ComputeViewInfo();

	TraceEventTick(DeltaTime);
}

void AInteractiveSceneCapture2D::BeginPointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());
	FVector2D localCoords = geometry.AbsoluteToLocal(pointerEvent.GetScreenSpacePosition());
	FVector2D normCoords = (localCoords / geometry.GetLocalSize() - 0.5) * FVector2D(2, -2);

	BeginPointer_Internal(GetWorld(), srcid, FingerIndex, normCoords, pointerEvent, { this, RayLength, false });
}

void AInteractiveSceneCapture2D::MovePointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());
	FVector2D localCoords = geometry.AbsoluteToLocal(pointerEvent.GetScreenSpacePosition());
	FVector2D normCoords = (localCoords / geometry.GetLocalSize() - 0.5) * FVector2D(2, -2);

	MovePointer_Internal(GetWorld(), srcid, FingerIndex, normCoords, pointerEvent, { this, RayLength, false });
}

void AInteractiveSceneCapture2D::EndPointer(UWidget* SourceWidget, FPointerEvent pointerEvent)
{
	int srcid = (int)SourceWidget->GetUniqueID();
	ETouchIndex::Type FingerIndex = (ETouchIndex::Type)((char)pointerEvent.GetPointerIndex());

	EndPointer_Internal(GetWorld(), srcid, FingerIndex);
}

void AInteractiveSceneCapture2D::OnInterpToggle(bool bEnable)
{
	CaptureComponent2D->SetVisibility(bEnable);
}

