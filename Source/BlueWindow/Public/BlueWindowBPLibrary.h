// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UserWidget.h"

#include "BlueWindowBPLibrary.generated.h"
struct FHitResult;

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FTraceResultFilterDelegate, FHitResult, Hit);

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UBlueWindowBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	 * Draw lines with constant thickness
	 */
	UFUNCTION(BlueprintCallable, Category = "Painting")
	static void DrawLinesThick(
		UPARAM(ref) FPaintContext& Context,
		const TArray<FVector2D>& Points,
		bool bAntiAlias,
		FLinearColor Tint = FLinearColor::White,
		float Thickness = 1.0);

	/**
	 * Draw lines with individual vertex colors and with constant thickness
	 */
	UFUNCTION(BlueprintCallable, Category = "Painting")
	static void DrawLinesColored(
		UPARAM(ref) FPaintContext& Context,
		const TArray<FVector2D>& Points,
		const TArray<FLinearColor>& PointColors,
		bool bAntiAlias,
		FLinearColor Tint = FLinearColor::White,
		float Thickness = 1.0);

	/** Draws a box with any brush (not just asset) */
	UFUNCTION(BlueprintCallable, Category = "Painting")
	static void DrawBoxBrushed(
		UPARAM(ref) FPaintContext& Context,
		FVector2D Position,
		FVector2D Size,
		FSlateBrush Brush,
		FLinearColor Tint = FLinearColor::White);

	/** Draw a cubic bezier spline */
	UFUNCTION(BlueprintCallable, Category = "Painting")
	static void DrawCubicBezierSpline(
		UPARAM(ref) FPaintContext& Context,
		FVector2D P0, FVector2D P1, FVector2D P2, FVector2D P3,
		FLinearColor Tint = FLinearColor::White,
		float Thickness = 1.0);

	/** Draw a Hermite spline */
	UFUNCTION(BlueprintCallable, Category = "Painting")
	static void DrawSpline(
		UPARAM(ref) FPaintContext& Context,
		FVector2D InStart,
		FVector2D InStartDir,
		FVector2D InEnd,
		FVector2D InEndDir,
		FLinearColor Tint = FLinearColor::White,
		float Thickness = 1.0);

	/** Draw a Hermite spline in Draw Space coordinates */
	UFUNCTION(BlueprintCallable, Category = "Painting")
	static void DrawSplineDrawSpace(
		UPARAM(ref) FPaintContext& Context,
		FVector2D InStart,
		FVector2D InStartDir,
		FVector2D InEnd,
		FVector2D InEndDir,
		FLinearColor Tint = FLinearColor::White,
		float Thickness = 1.0);

	/**
	 * Trace a line and filter hits based on a delegate
	 * NOTE: the original trace function returns on the first blocking hit.
	 * So if that fisrt blocking hit fails the delegate then this function
	 * will return no hits and a false. If you want to avoid that set your objects
	 * to Overlap collision. Might be fixed in the future.
	 */
	static bool LineTraceFiltered(
		UWorld* World,
		FVector Start,
		FVector End,
		FTraceResultFilterDelegate Filter,
		TArray<FHitResult>& OutHits,
		FHitResult& FirstHit
	);
};
