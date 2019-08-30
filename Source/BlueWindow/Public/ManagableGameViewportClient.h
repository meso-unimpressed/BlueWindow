// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "BlueWindowSettings.h"

#include "ManagableGameViewportClient.generated.h"

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

public:

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;

	UFUNCTION(BlueprintCallable, Category = "ManagableViewport")
		void UpdateDisplayMetrics();

	UFUNCTION(BlueprintCallable, Category = "ManagableViewport")
		FBlueWindowSettings GetSettings() { return Settings; }

	UFUNCTION(BlueprintCallable, Category = "ManagableViewport")
		void SetSettings(FBlueWindowSettings settings, bool force);

	UFUNCTION(BlueprintCallable, Category = "ManagableViewport")
		static UManagableGameViewportClient* GetManagableViewport(bool& Success);
};
