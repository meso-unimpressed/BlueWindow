// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UserWidget.h"
#include "Engine/SceneCapture.h"

#include "InteractiveSceneCapture2D.generated.h"

USTRUCT(BlueprintType)
struct BLUEWINDOW_API FPointerRay
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		TEnumAsByte<ETouchIndex::Type> FingerIndex;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		bool Hit = false;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		FHitResult HitResult;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		FVector2D CoordinateOnWidget;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		AActor* BeganOnActor = nullptr;

	/**
	 * Computed by SourceId ^ SourceSceneCapture ^ ((int)FingerIndex)
	 */
	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		int UniqueId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		int SourceId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		int AgeInFrames = 0;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		float AgeInSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		class AInteractiveSceneCapture2D* SourceSceneCapture = nullptr;

	void Age(float delta);
};

UCLASS()
class BLUEWINDOW_API AInteractiveSceneCapture2D : public ASceneCapture
{
	GENERATED_UCLASS_BODY()

private:
	void ComputeViewInfo();

	void CastPointerRay(FPointerRay& ray, FVector2D NormalizedCoords, bool begin);
	
public:	

	static void ComputeRayForViewProjection(FMatrix ViewProj, FVector2D NormCoord, FVector& Start, FVector& End);

	UPROPERTY(Category = DecalActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneCaptureComponent2D* CaptureComponent2D;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow|InteractiveSceneCapture")
		TMap<int, FPointerRay> CurrentPointers;

	void BeginPointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords);

	void MovePointer(int SourceId, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords);

	void EndPointer(int SourceId, ETouchIndex::Type FingerIndex);

	UFUNCTION(BlueprintCallable, Category = "Rendering")
		void OnInterpToggle(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		void BeginPointer(UWidget* SourceWidget, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		void MovePointer(UWidget* SourceWidget, ETouchIndex::Type FingerIndex, FVector2D NormalizedCoords);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|InteractiveSceneCapture")
		void EndPointer(UWidget* SourceWidget, ETouchIndex::Type FingerIndex);

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
		float OrthoFarClipPlane = 50000;

	FMinimalViewInfo ViewInfo;
	FSceneViewProjectionData ProjectionData;

	FMatrix ViewProjectionMatrix;

	///** Returns CaptureComponent2D subobject **/
	//BLUEWINDOW_API class USceneCaptureComponent2D* GetCaptureComponent2D() const { return CaptureComponent2D; }
};
