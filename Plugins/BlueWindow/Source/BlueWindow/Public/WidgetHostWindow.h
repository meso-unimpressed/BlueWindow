// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/SlateCore/Public/Widgets/SWindow.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "GenericApplication.h"
#include "UserWidget.h"
#include "BlueWindowSettings.h"
#include "WidgetHostWindow.generated.h"

UCLASS( ClassGroup=(BlueWindow), meta=(BlueprintSpawnableComponent) )
class BLUEWINDOW_API UWidgetHostWindow : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TSharedPtr<SWindow> sWindow;
	TSharedPtr<SWidget> sContent;
	UUserWidget* uContent;

	TArray<FMonitorInfo> Monitors = TArray<FMonitorInfo>();
	FDisplayMetrics DisplayMetrics;

	uint64_t GetMonitorOrderComparer(FMonitorInfo moninfo, int minleft, int mintop)
	{
		uint64_t l = (uint64_t)(moninfo.DisplayRect.Left + minleft) & 0x00000000FFFFFFFF;
		uint64_t t = (uint64_t)(moninfo.DisplayRect.Top + mintop) & 0x00000000FFFFFFFF;
		return (l << 32) | t;
	}

	FBlueWindowSettings Settings;

public:

	// Sets default values for this component's properties
	UWidgetHostWindow();

	~UWidgetHostWindow();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Opens the associated window
	UFUNCTION(BlueprintCallable)
	void OpenWindow(TSubclassOf<class UUserWidget> content, APlayerController* owner, FBlueWindowSettings settings);

	UFUNCTION(BlueprintCallable)
	void UpdateDisplayMetrics();

	UFUNCTION(BlueprintCallable)
	void BringToFront(bool bForce);

	UFUNCTION(BlueprintCallable)
	FBlueWindowSettings GetSettings() { return Settings; }

	UFUNCTION(BlueprintCallable)
	void SetSettings(FBlueWindowSettings settings, bool force);

	UFUNCTION(BlueprintCallable)
	void Close();
};
