// Fill out your copyright notice in the Description page of Project Settings.

#include "PropertySynchronizableUserWidget.h"
#include "SlateOptMacros.h"

inline void UPropertySynchronizableUserWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	OnUserPropertiesSynchronize();
}

void UMaterialBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if(Material)
	{
		if (PrevMat != Material || ! Drawer->Material)
		{
			PrevMat = Material;
			MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
			Drawer->SetMaterial(MaterialInstance);
			OnMaterialChanged();
		}
		Drawer->UpdateBrush();
	}

	OnUserPropertiesSynchronize();
}

void UMaterialBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Drawer.Reset();
}

TSharedRef<SWidget> UMaterialBox::RebuildWidget()
{
	if (Material)
	{
		PrevMat = Material;
		MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
		Drawer = SNew(SMaterialBox).Material(MaterialInstance);
		OnMaterialChanged();
	}
	else Drawer = SNew(SMaterialBox);
	return Drawer.ToSharedRef();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMaterialBox::Construct(const FArguments& InArgs)
{
	Material = InArgs._Material.Get();
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}

int32 SMaterialBox::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	if(Material)
	{
		FVector2D size = AllottedGeometry.GetLocalSize();
		FVector2D pos = AllottedGeometry.GetAbsolutePosition();
		Material->SetVectorParameterValue("iResolution", FLinearColor(size.X, size.Y, 0, 0));
		Material->SetVectorParameterValue("iPosition", FLinearColor(size.X, size.Y, 0, 0));
		//(&Brush)->ImageSize = size;

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			&Brush
		);
	}
	return LayerId;
}

void SMaterialBox::SetMaterial(UMaterialInstanceDynamic* mat)
{
	Material = mat;
	Brush.SetResourceObject((UObject*)(Material));
}

void SMaterialBox::UpdateBrush()
{
	Brush.SetResourceObject((UObject*)(Material));
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
