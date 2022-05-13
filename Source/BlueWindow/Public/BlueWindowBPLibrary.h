// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Blueprint/UserWidget.h"

#include "BlueWindowBPLibrary.generated.h"
struct FHitResult;

/*
 * Based on HWND insertion option for SetWindowPos() in WIN32 API 
 */
UENUM(Blueprintable)
enum EWindowOrder
{
	BOTTOM = 1, /* Places the window at the bottom of the Z order. If the hWnd parameter identifies a topmost window, the window loses its topmost status and is placed at the bottom of all other windows. */
	TOP = 0, /* Places the window at the top of the Z order. */
};


DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FTraceResultFilterDelegate, FHitResult, Hit);
DECLARE_DELEGATE_RetVal_OneParam(bool, FTraceResultFilterStaticDel, FHitResult);

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
    UFUNCTION(BlueprintCallable, Category = "Trace")
    static bool LineTraceFiltered(
        UWorld* World,
        FVector Start,
        FVector End,
        FTraceResultFilterDelegate Filter,
        TArray<FHitResult>& OutHits,
        FHitResult& FirstHit
    );
    static bool LineTraceFiltered(
        UWorld* World,
        FVector Start,
        FVector End,
        FTraceResultFilterStaticDel Filter,
        TArray<FHitResult>& OutHits,
        FHitResult& FirstHit
    );

    /*
     * Simple transform combination as it was matrix multiplication
     */
    UFUNCTION(BlueprintPure, Category = "Widgets")
    static FWidgetTransform CombineTransform(FWidgetTransform LHS, FWidgetTransform RHS);

    /*
     * Simple transform inversion as it was matrix inversion
     */
    UFUNCTION(BlueprintPure, Category = "Widgets")
    static FWidgetTransform InverseTransform(FWidgetTransform Input);

    /*
     * Get all the combined transformations and opacity which might
     * influence the look of the target widget.
     *
     * It is a recursive operation, keep max-depth as low as possible,
     * 100 should be enough for all sane situations
     *
     * Shear is not supported because this function uses Slate widgets instead of UMG
     * and their transformation is stored as proper 2x2 matrix + translation
     * while UMG widgets' transformation is stored as individual components (I despise that decision :P).
     * Until I figure out how to extract shear information from 2x2 matrix,
     * shearing remains unsupported, and yields undefined behavior.
     */
    UFUNCTION(BlueprintPure, Category = "Widgets")
    static void GetAccumulatedWidgetRender(UWidget* Target, FWidgetTransform& Transform, float& Opacity, int MaxDepth = 100);

	UFUNCTION(BlueprintCallable, Category = "Window", meta = (WorldContext = WorldContextObject))
	static void SetFocusToGameWindow(UObject* WorldContextObject, bool EnableCapture = true);
	
	/*
	* Move OS window to specific Z.
	*/
    UFUNCTION(BlueprintCallable, Category = "Window", meta = (WorldContext = WorldContextObject))
	static void MoveViewportWindowToZ(UObject* WorldContextObject, EWindowOrder WindowOrder = TOP);
	
	UFUNCTION(BlueprintCallable, Category = "Window", meta = (WorldContext = WorldContextObject))
	static void MakeViewportActiveWindow(UObject* WorldContextObject, bool SetFocus = true);
	
	static bool IsInEditorAndNotPlaying();
	
	UFUNCTION(BlueprintPure, Category = "Window|Editor")
	static FVector2D ProjectEditorWorldSpacePointToScreenSpace(FVector Point);

	static TSharedPtr<SWidget> GetChildWidgetOfType(TSharedPtr<SWidget> InWidget, FName InType);
	static TSharedPtr<SOverlay> GetEditorViewportOverlay();
	
	UFUNCTION(BlueprintCallable, Category = "Window|Editor")
	static void AddWidgetOverlayToEditorViewport(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "Window|Editor")
	static void RemoveWidgetOverlayFromEditorViewport(UWidget* Widget);
	
	
};
