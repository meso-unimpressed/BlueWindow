// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "PolyLine.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class BLUEWINDOW_API SLineDrawerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLineDrawerWidget) : _Lines()
	{ }

	SLATE_ATTRIBUTE(TArray<FPolyLine2D>, Lines)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetLines(TArray<FPolyLine2D> lines);

	TArray<FPolyLine2D> Lines;
};
