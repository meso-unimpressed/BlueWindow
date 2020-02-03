// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Engine/SceneCapture.h"
#include "Input/Events.h"
#include "Layout/Geometry.h"
#include "PointerRay.h"

#include "InteractiveSceneCapture2D.generated.h"

UCLASS()
class BLUEWINDOW_API AInteractiveSceneCapture2D : public ASceneCapture
{
	GENERATED_UCLASS_BODY()

private:
	void ComputeViewInfo();

	void CastPointerRay(FPointerRay& ray, FVector2D NormalizedCoords, bool begin);
	
public:	

	void ComputeRayForViewProjection(FMatrix ViewProj, FVector2D NormalizedCoords, FVector& Start, FVector& End);

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

	UFUNCTION()
		bool TraceResultPredicate(FHitResult Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		TMap<int, FPointerRay> CurrentPointers;

	void BeginPointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent);

	void MovePointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords, FPointerEvent pointerEvent);

	void EndPointer(int SourceId, ETouchIndex::Type FingerIndex);

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
