// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
