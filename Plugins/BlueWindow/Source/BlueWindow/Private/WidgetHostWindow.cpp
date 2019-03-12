// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetHostWindow.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWidgetHostWindow::UWidgetHostWindow()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWidgetHostWindow::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWidgetHostWindow::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto actor = GetOwner();
	
	// ...
}

void UWidgetHostWindow::OpenWindow(TSubclassOf<class UUserWidget> content, APlayerController* owner)
{
	if(!owner)
	{
		owner = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	uContent = CreateWidget<UUserWidget>(owner, content);

	sWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Blue Window")))
		.HasCloseButton(true)
		.IsInitiallyMaximized(false)
		.ScreenPosition(FVector2D(0, 0))
		.ClientSize(FVector2D(1000, 1000))
		.UseOSWindowBorder(true)
		.CreateTitleBar(true)
		.SizingRule(ESizingRule::UserSized)
		.SupportsMaximize(false)
		.SupportsMinimize(true);

	sContent = uContent->TakeWidget();

	sWindow->SetContent(sContent.ToSharedRef());

	auto refWindow = sWindow.ToSharedRef();
	FSlateApplication& slateApp = FSlateApplication::Get();
	slateApp.AddWindow(refWindow, true);
	sWindow->SetContent(sContent.ToSharedRef());
}


