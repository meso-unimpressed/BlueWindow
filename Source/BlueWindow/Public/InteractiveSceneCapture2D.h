// Developed by MESO Digital Interiors GmbH. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TraceEventSourceBase.h"
#include "Blueprint/UserWidget.h"
#include "Engine/SceneCapture.h"
#include "SceneView.h"
#include "Input/Events.h"
#include "Layout/Geometry.h"
#include "PointerRay.h"

#include "InteractiveSceneCapture2D.generated.h"

UCLASS()
class BLUEWINDOW_API AInteractiveSceneCapture2D
    : public ASceneCapture
	, public ITraceEventSourceBase
{
	GENERATED_UCLASS_BODY()

private:
	void ComputeViewInfo();
	
public:

	/**
	 * Deproject a screen coordinate to a world position with start and end points.
	 * @param NormCoord: (-1, -1) bottom left to (1, 1) top right normalized projection coordinates
	 */
	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		void DeprojectScreenToWorld(FVector2D NormCoord, FVector& Start, FVector& End);

	/**
	 * Project a world coordinate to screen coordinates
	 * @param Depth: normalized screen depth
	 * @return NormCoord: (-1, -1) bottom left to (1, 1) top right normalized projection coordinates
	 */
	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		FVector2D ProjectWorldToScreen(FVector World, float& Depth);

	UPROPERTY(Category = DecalActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneCaptureComponent2D* CaptureComponent2D;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual FMatrix GetViewProjectionMatrix() override;
	virtual TMap<int, FPointerRay>& GetCurrentPointers() override;
	virtual int GetUuid_Internal() override;
	virtual bool ShouldDrawDebugHelpers() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		TMap<int, FPointerRay> CurrentPointers;

	UFUNCTION(BlueprintCallable, Category = "Rendering")
		void OnInterpToggle(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		void BeginPointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		void MovePointer(UWidget* SourceWidget, FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		void EndPointer(UWidget* SourceWidget, FPointerEvent pointerEvent);

	//UPROPERTY(BlueprintAssignable, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
	//TArray<ECollisionChannel> CollisionChannels;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
		bool TraceComplex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
		TArray<AActor*> ActorsToIgnore;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
		bool DrawDebugHelpers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
		bool TriggerEndEventOnOriginatedActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
		bool TriggerMoveEventOnOriginatedActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow|InteractiveSceneCapture")
		float RayLength = 250000;

	FMinimalViewInfo ViewInfo;
	FSceneViewProjectionData ProjectionData;

	FMatrix ViewProjectionMatrix;

	///** Returns CaptureComponent2D subobject **/
	//BLUEWINDOW_API class USceneCaptureComponent2D* GetCaptureComponent2D() const { return CaptureComponent2D; }
};
