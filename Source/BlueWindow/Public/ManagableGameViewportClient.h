// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "BlueWindowSettings.h"
#include "Events.h"

#include "ManagableGameViewportClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FViewportTouchEventDelegate, FPointerEvent, Pointer);

/**
 * 
 */
UCLASS()
class BLUEWINDOW_API UManagableGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

protected:
	FVector2D prevWindowPos = FVector2D(0, 0);
	FVector2D prevWindowSize = FVector2D(0, 0);
	FVector2D LastMouseLocation;

	TArray<FMonitorInfo> Monitors;
	FDisplayMetrics DisplayMetrics;

	FORCEINLINE FMonitorInfo GetMonitor(int i)
	{
		return Monitors.Num() <= 0 ?
			FMonitorInfo() :
			Monitors[i % Monitors.Num()];
	}

	uint64_t GetMonitorOrderComparer(FMonitorInfo moninfo, int64 minleft, int64 mintop);

	FBlueWindowSettings Settings;

	TMap<uint32, FVector2D> LastTouchLocations;

	FPointerEvent GetPointerEvent(uint32 Handle, FVector2D TouchLocation);
	FPointerEvent GetPointerEventWithDelta(uint32 Handle, FVector2D TouchLocation);
	FPointerEvent GetMouseEventWithDelta(FVector2D MouseLocation);

	UPROPERTY()
		UTexture2D* ViewportCopy;

public:

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;

	virtual bool InputTouch (
		FViewport* Viewport,
		int32 ControllerId,
		uint32 Handle,
		ETouchType::Type Type,
		const FVector2D& TouchLocation,
		float Force,
		FDateTime DeviceTimestamp,
		uint32 TouchpadIndex
	) override;

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|ManagableViewport")
		void UpdateDisplayMetrics();

	UFUNCTION(BlueprintPure, Category = "BlueWindow|ManagableViewport")
		UTexture2D* GetViewportTexture();

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|ManagableViewport")
		FBlueWindowSettings GetSettings() { return Settings; }

	UFUNCTION(BlueprintCallable, Category = "BlueWindow|ManagableViewport")
		void SetSettings(FBlueWindowSettings settings, bool force);

	UFUNCTION(BlueprintPure, Category = "BlueWindow|ManagableViewport")
		static UManagableGameViewportClient* GetManagableViewport(bool& Success);

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FViewportTouchEventDelegate OnMouseMove;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FViewportTouchEventDelegate OnTouchBegin;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FViewportTouchEventDelegate OnTouchFirstMove;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FViewportTouchEventDelegate OnTouchMove;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FViewportTouchEventDelegate OnTouchStationary;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|ManagableViewport")
		FViewportTouchEventDelegate OnTouchEnded;

	virtual void Tick(float DeltaTime) override;


	void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;

	void Activated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent) override;

};
