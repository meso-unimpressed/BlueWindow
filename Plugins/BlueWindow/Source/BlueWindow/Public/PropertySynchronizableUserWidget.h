// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "PropertySynchronizableUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUserWidgetPropertiesSynchronize);

/**
 * 
 */
UCLASS()
class BLUEWINDOW_API UPropertySynchronizableUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SynchronizeProperties() override;

	UFUNCTION(BlueprintImplementableEvent)
    void OnUserPropertiesSynchronize();
};

/**
 *
 */
class BLUEWINDOW_API SMaterialBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMaterialBox) : _Material()
	{ }

	SLATE_ATTRIBUTE(UMaterialInstanceDynamic*, Material)

	SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetMaterial(UMaterialInstanceDynamic* mat);

	void UpdateBrush();

	UMaterialInstanceDynamic* Material;
	FSlateBrush Brush;
};

/**
 *
 */
UCLASS()
class BLUEWINDOW_API UMaterialBox : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	TSharedPtr<SMaterialBox> Drawer;

	UMaterialInterface* PrevMat;
public:

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Widget")
		UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		UMaterialInterface* Material;

	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual void NativePaint(FPaintContext& InContext) const override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnUserPropertiesSynchronize();

	UFUNCTION(BlueprintImplementableEvent)
		void OnMaterialChanged();
};