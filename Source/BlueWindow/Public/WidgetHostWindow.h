// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "GenericPlatform/GenericApplication.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SBox.h"
#include "BlueWindowSettings.h"
#include "SInputPropagator.h"
#include "SInputPropagatingWindow.h"

#include "WidgetHostWindow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBlueWindowTransformed, FVector2D, Position, FVector2D, Size);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlueWindowResized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlueWindowClosed);

class UGlobalInputProcessor;

UCLASS( ClassGroup=(BlueWindow), meta=(BlueprintSpawnableComponent) )
class BLUEWINDOW_API UWidgetHostWindow : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TSharedPtr<SInputPropagatingWindow> sWindow;
	TSharedPtr<SWidget> sContent;
	TSharedPtr<SConstraintCanvas> sCanvas;
	TSharedPtr<SInputPropagator> sBox;

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

	// Sets default values for this component's properties
	UWidgetHostWindow();

	~UWidgetHostWindow();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Opens the associated window
	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
	void OpenWindow(
		TSubclassOf<class UUserWidget> content, APlayerController* owner, FBlueWindowSettings settings,
		UUserWidget*& outContent
	);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void UpdateDisplayMetrics();

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void BringToFront(bool bForce);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		FBlueWindowSettings GetSettings() { return Settings; }

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void SetSettings(FBlueWindowSettings settings, bool force);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void Close();

	UPROPERTY(BlueprintReadOnly, Category = "BlueWindow")
		UUserWidget* Content;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FOnBlueWindowTransformed OnWindowMoved;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FOnBlueWindowTransformed OnWindowResized;

	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FOnBlueWindowClosed OnWindowClosed;

	UPROPERTY(BlueprintReadWrite, Category = "BlueWindow")
		UInputProcessorCollection* GlobalInputProcessors;

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void AddInputTargetWidget(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void RemoveInputTargetWidget(UUserWidget* widget);

	void BeginDestroy() override;

	void PreDestroy();

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
