// Fill out your copyright notice in the Description page of Project Settings.

#include "SLineDrawerWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLineDrawerWidget::Construct(const FArguments& InArgs)
{
	Lines = InArgs._Lines.Get();
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}

int32 SLineDrawerWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	for(auto plitn = Lines.CreateConstIterator(); plitn; ++plitn)
	{
		FPolyLine2D polyline = *plitn;
		if(polyline.UseVertexColor)
		{
			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(
					FVector2D::ZeroVector,
					AllottedGeometry.GetLocalSize(),
					1),
				polyline.Vertices,
				polyline.Colors,
				ESlateDrawEffect::None,
				polyline.Tint,
				polyline.Antialias,
				polyline.Thickness);
		}
		else
		{
			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(
					FVector2D::ZeroVector,
					AllottedGeometry.GetLocalSize(),
					1),
				polyline.Vertices,
				ESlateDrawEffect::None,
				polyline.Tint,
				polyline.Antialias,
				polyline.Thickness);
		}
	}
	return LayerId;
}

void SLineDrawerWidget::SetLines(TArray<FPolyLine2D> lines)
{
	Lines = lines;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
