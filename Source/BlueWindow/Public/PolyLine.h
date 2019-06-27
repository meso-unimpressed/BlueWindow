// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PolyLine.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FPolyLine2D
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG/BlueWindow/Line")
		TArray<FVector2D> Vertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG/BlueWindow/Line")
		TArray<FLinearColor> Colors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG/BlueWindow/Line")
		FLinearColor Tint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG/BlueWindow/Line")
		float Thickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG/BlueWindow/Line")
		bool Antialias;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG/BlueWindow/Line")
		bool UseVertexColor;
};