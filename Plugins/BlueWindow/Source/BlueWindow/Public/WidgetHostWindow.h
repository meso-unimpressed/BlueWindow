// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/SlateCore/Public/Widgets/SWindow.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "UserWidget.h"
#include "WidgetHostWindow.generated.h"

// Delegate for on window opened
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindowOpenedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLUEWINDOW_API UWidgetHostWindow : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWidgetHostWindow();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TSharedPtr<SWindow> sWindow;
	TSharedPtr<SWidget> sContent;
	UUserWidget* uContent;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Opens the associated window
	UFUNCTION(BlueprintCallable)
	void OpenWindow(TSubclassOf<class UUserWidget> content, APlayerController* owner);

	// Called when the window successfully opened
	UPROPERTY(BlueprintAssignable)
	FOnWindowOpenedDelegate OnWindowOpened;
};
