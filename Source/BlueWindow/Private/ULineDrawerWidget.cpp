// Fill out your copyright notice in the Description page of Project Settings.

#include "ULineDrawerWidget.h"

TSharedRef<SWidget> ULineDrawerWidget::RebuildWidget()
{
	Drawer = SNew(SLineDrawerWidget);
	return Drawer.ToSharedRef();
}

void ULineDrawerWidget::UserDraw()
{
	Drawer->Lines = Lines;
}

void ULineDrawerWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	Drawer->Lines = Lines;
}

void ULineDrawerWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Drawer.Reset();
}
