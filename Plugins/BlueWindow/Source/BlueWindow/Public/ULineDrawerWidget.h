// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/UMG.h"
#include "SLineDrawerWidget.h"
#include "PolyLine.h"
#include "Blueprint/UserWidget.h"

#include "ULineDrawerWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLUEWINDOW_API ULineDrawerWidget : public UWidget
{
	GENERATED_BODY()
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	TSharedPtr<SLineDrawerWidget> Drawer;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Drawer")
	TArray<FPolyLine2D> Lines;

	UFUNCTION(BlueprintCallable, Category = "Line Drawer")
	void UserDraw();

	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
};