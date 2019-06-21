// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "UObject/Interface.h"
#include "BlueWindowSettings.generated.h"


UENUM(BlueprintType)
enum class EBlueWindowMode : uint8
{
	Fullscreen = EWindowMode::Type::Fullscreen,
	WindowedFullscreen = EWindowMode::Type::WindowedFullscreen,
	Windowed = EWindowMode::Type::Windowed,
	NumWindowModes = EWindowMode::Type::NumWindowModes
};

UENUM(BlueprintType)
enum class EBlueWindowTransparencyMode : uint8
{
	None = (uint8)EWindowTransparency::None,
	PerPixel = (uint8)EWindowTransparency::PerPixel,
	PerWindow = (uint8)EWindowTransparency::PerWindow
};

// This class does not need to be modified.
USTRUCT(BlueprintType)
struct FBlueWindowSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		FText Title = FText::FromString(TEXT("UE4 Window"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		bool CreateTitleBar = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		bool HasCloseButton = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		FMargin ResizeBorder = FMargin(5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		bool SupportsMinimize = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		bool SupportsMaximize = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		bool UseOSWindowBorder = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
	//bool AppearsInTaskbar = false;

	/**
	 * Screen ID here != the one in the OS. It's always ordered left to right then top to bottom
	 * 0 is the leftmost monitor. This is done to make industrial situations safer.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		int TargetMonitor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		FVector2D TopLeftOffset = FVector2D(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		FVector2D Size = FVector2D(1280, 720);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		EBlueWindowMode WindowMode = EBlueWindowMode::Windowed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlueWindow")
		float Opacity = 1.0;
};
