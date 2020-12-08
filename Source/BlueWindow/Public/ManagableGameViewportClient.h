// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once


#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "BlueWindowSettings.h"
#include "Input/Events.h"

#include "ManagableGameViewportClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FViewportTouchEventDelegate, FPointerEvent, Pointer);

class UGlobalInputProcessor;
class UInputProcessorCollection;

/**
 * 
 */
UCLASS()
class BLUEWINDOW_API UManagableGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
private:
	//ID3D11Device* D3D11Device = nullptr;
	//ID3D11DeviceContext* pImmediateContext = nullptr;
	//ID3D11Texture2D* nativeVpTex = nullptr;
	//FString currRHI;

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
		FViewport* TargetViewport,
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

	UPROPERTY(BlueprintReadWrite, Category = "BlueWindow")
		UInputProcessorCollection* GlobalInputProcessors;

	UPROPERTY(BlueprintReadWrite, Category = "BlueWindow")
		UGlobalInputProcessor* MainGlobalInput;

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void AddInputTargetWidget(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void RemoveInputTargetWidget(UUserWidget* widget);

	virtual void Tick(float DeltaTime) override;

    virtual void MouseMove(FViewport* InViewport, int32 X, int32 Y) override;

    virtual void Activated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent) override;

	FGeometry GetGeometry();
};
