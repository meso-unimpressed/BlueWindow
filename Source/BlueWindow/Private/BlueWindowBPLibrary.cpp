// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlueWindowBPLibrary.h"
#include "BlueWindow.h"
#include "CoreMinimal.h"
#include "Rendering/DrawElements.h"
#include <limits>

UBlueWindowBPLibrary::UBlueWindowBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UBlueWindowBPLibrary::DrawLinesColored(
	FPaintContext& Context,
	const TArray<FVector2D>& Points,
	const TArray<FLinearColor>& PointColors,
	bool bAntiAlias,
	FLinearColor Tint,
	float Thickness)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeLines(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		Points,
		PointColors,
		ESlateDrawEffect::None,
		Tint,
		bAntiAlias,
		Thickness);
}

void UBlueWindowBPLibrary::DrawBoxBrushed(
	FPaintContext& Context,
	FVector2D Position,
	FVector2D Size,
	FSlateBrush Brush,
	FLinearColor Tint)
{

	Context.MaxLayer++;

	FSlateDrawElement::MakeBox(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(Position, Size),
		&Brush,
		ESlateDrawEffect::None,
		Tint);
}

void UBlueWindowBPLibrary::DrawCubicBezierSpline(
	UPARAM(ref) FPaintContext& Context,
	FVector2D P0, FVector2D P1, FVector2D P2, FVector2D P3,
	FLinearColor Tint,
	float Thickness)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeCubicBezierSpline(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		P0, P1, P2, P3,
		Thickness,
		ESlateDrawEffect::None,
		Tint);
}

void UBlueWindowBPLibrary::DrawSpline(
	FPaintContext& Context,
	FVector2D InStart, FVector2D InStartDir,
	FVector2D InEnd, FVector2D InEndDir,
	FLinearColor Tint,
	float Thickness)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeSpline(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		InStart, InStartDir, InEnd, InEndDir,
		Thickness,
		ESlateDrawEffect::None,
		Tint);
}

void UBlueWindowBPLibrary::DrawSplineDrawSpace(
	FPaintContext& Context,
	FVector2D InStart, FVector2D InStartDir,
	FVector2D InEnd, FVector2D InEndDir,
	FLinearColor Tint,
	float Thickness)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeDrawSpaceSpline(
		Context.OutDrawElements,
		Context.MaxLayer,
		InStart, InStartDir, InEnd, InEndDir,
		Thickness,
		ESlateDrawEffect::None,
		Tint);
}

bool UBlueWindowBPLibrary::LineTraceFiltered(UWorld* World, FVector Start, FVector End, FTraceResultFilterDelegate Filter, TArray<FHitResult>& OutHits, FHitResult& FirstHit)
{
	OutHits = TArray<FHitResult>();
	TArray<FHitResult> tempHits;

	FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActors(ActorsToIgnore);
	CollisionParams.bReturnFaceIndex = false;
	CollisionParams.bReturnPhysicalMaterial = false;
	CollisionParams.bTraceComplex = false;

	FirstHit = FHitResult();

	float mindist = std::numeric_limits<float>::max();

	World->LineTraceMultiByObjectType(
		tempHits, Start, End,
		FCollisionObjectQueryParams::DefaultObjectQueryParam,
		CollisionParams);

	for(FHitResult hit : tempHits)
	{
		if(hit.Distance <= 0) continue;
		if (!Filter.Execute(hit)) continue;

		OutHits.Add(hit);
		if(hit.Distance < mindist)
		{
			mindist = hit.Distance;
			FirstHit = hit;
		}
	}
	return OutHits.Num() > 0;
}

void UBlueWindowBPLibrary::DrawLinesThick(
	FPaintContext& Context,
	const TArray<FVector2D>& Points,
	bool bAntiAlias,
	FLinearColor Tint,
	float Thickness)
{
	Context.MaxLayer++;

	FSlateDrawElement::MakeLines(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		Tint,
		bAntiAlias,
		Thickness);
}

