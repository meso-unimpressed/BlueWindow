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
		if (Monitors.Num() <= 0) return FMonitorInfo();
		return Monitors[i % Monitors.Num()];
	}

	uint64_t GetMonitorOrderComparer(FMonitorInfo moninfo, int64 minleft, int64 mintop);

	FBlueWindowSettings Settings;

public:

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void UpdateDisplayMetrics();

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		FBlueWindowSettings GetSettings() { return Settings; }

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void SetSettings(FBlueWindowSettings settings, bool force);
};
