// Fill out your copyright notice in the Description page of Project Settings.

#include "PropertySynchronizableUserWidget.h"

inline void UPropertySynchronizableUserWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	OnUserPropertiesSynchronize();
}
