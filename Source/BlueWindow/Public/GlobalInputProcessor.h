// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GlobalInputProcessor.generated.h"

class UUserWidget;
class UGlobalInputProcessor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPointerEventDynDel, FGeometry, geometry, FPointerEvent, pointerEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKeyEventDynDel, FGeometry, geometry, FKeyEvent, keyEvent);


UCLASS(BlueprintType, Blueprintable)
class BLUEWINDOW_API UInputProcessorCollection : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, Category = "BlueWindow")
		TMap<FString, UGlobalInputProcessor*> Map;

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnKeyDown(FGeometry geometry, FKeyEvent keyEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnKeyUp(FGeometry geometry, FKeyEvent keyEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnTouchGesture(FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnTouchStarted(FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnTouchMoved(FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnTouchForceChanged(FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnTouchEnded(FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnMouseButtonDown(FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnMouseButtonUp(FGeometry geometry, FPointerEvent pointerEvent);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void OnMouseMove(FGeometry geometry, FPointerEvent pointerEvent);


	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void AddInputTargetWidget(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void RemoveInputTargetWidget(UUserWidget* widget);
};

UCLASS(BlueprintType, Blueprintable)
class BLUEWINDOW_API UGlobalInputProcessor : public UObject
{
    GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BlueWindow")
		TSet<UUserWidget*> TunnelToWidgets;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void OnKeyDown(FGeometry geometry, FKeyEvent keyEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FKeyEventDynDel OnKeyDownEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void OnKeyUp(FGeometry geometry, FKeyEvent keyEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FKeyEventDynDel OnKeyUpEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void OnTouchGesture(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnTouchGestureEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void OnTouchStarted(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnTouchStartedEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void  OnTouchMoved(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnTouchMovedEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void  OnTouchForceChanged(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnTouchForceChangedEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void  OnTouchEnded(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnTouchEndedEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void  OnMouseButtonDown(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnMouseButtonDownEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void  OnMouseButtonUp(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnMouseButtonUpEvent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BlueWindow")
		void  OnMouseMove(FGeometry geometry, FPointerEvent pointerEvent);
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow")
		FPointerEventDynDel OnMouseMoveEvent;

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void AddTargetWidget(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Category = "BlueWindow")
		void RemoveTargetWidget(UUserWidget* widget);
};
