// Fill out your copyright notice in the Description page of Project Settings.

#include "WidgetHostWindow.h"
#include "Kismet/GameplayStatics.h"

#include "GlobalInputProcessor.h"

// Sets default values for this component's properties
UWidgetHostWindow::UWidgetHostWindow()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetCanEverAffectNavigation(false);

	// ...
}

UWidgetHostWindow::~UWidgetHostWindow()
{
	if(sWindow.IsValid())
	{
		sWindow.Get()->RequestDestroyWindow();
	}
}


// Called when the game starts
void UWidgetHostWindow::BeginPlay()
{
	Super::BeginPlay();
	UpdateDisplayMetrics();
	GlobalInputProcessors = NewObject<UInputProcessorCollection>(this);
}

// Called every frame
void UWidgetHostWindow::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//auto actor = GetOwner();

	if(sWindow.IsValid())
	{
		FVector2D currpos = sWindow->GetPositionInScreen();
		FVector2D currsize = sWindow->GetSizeInScreen();

		if(currsize != prevWindowSize)
			OnWindowResized.Broadcast(currpos, currsize);

		prevWindowPos = currpos;
		prevWindowSize = currsize;
	}
}

#define SWINDOW_KEYEVENT_TUNNEL(name) \
	sWindow->##name##Event.AddLambda([this](FGeometry geometry, FKeyEvent keyEvent) { \
		GlobalInputProcessors->##name##(geometry, keyEvent); })

#define SWINDOW_TOUCHEVENT_TUNNEL(name) \
	sWindow->##name##Event.AddLambda([this](FGeometry geometry, FPointerEvent pointerEvent) { \
		GlobalInputProcessors->##name##(geometry, pointerEvent); })

void UWidgetHostWindow::OpenWindow(
	TSubclassOf<class UUserWidget> content, APlayerController* owner, FBlueWindowSettings settings,
	UUserWidget*& outContent
)
{
	if(!owner)
	{
		owner = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	if(sCanvas.IsValid()) sCanvas.Reset();

	Settings = settings;

	FMonitorInfo targetMonitor = GetMonitor(settings.TargetMonitor);
	FVector2D offset = FVector2D(targetMonitor.WorkArea.Left, targetMonitor.WorkArea.Top) + settings.TopLeftOffset;

	sWindow = SNew(SInputPropagatingWindow)
		.WindowArgs(SWindow::FArguments()
			.Title(settings.Title)
			.HasCloseButton(settings.HasCloseButton)
			.IsInitiallyMaximized(false)
			.ScreenPosition(offset)
			.ClientSize(settings.Size)
			.UseOSWindowBorder(settings.UseOSWindowBorder)
			.CreateTitleBar(settings.CreateTitleBar)
			.SizingRule(ESizingRule::UserSized)
			.SupportsMaximize(settings.SupportsMaximize)
			.SupportsMinimize(settings.SupportsMinimize)
			.InitialOpacity(settings.Opacity)
			.UserResizeBorder(settings.ResizeBorder)
		)
		.TargetWorld(GetWorld())
		.HandleEvents(false)
		.PropagateKeys(true)
		.PropagateTouchGestures(true);

	SWINDOW_KEYEVENT_TUNNEL(OnKeyDown);
	SWINDOW_KEYEVENT_TUNNEL(OnKeyUp);
	SWINDOW_TOUCHEVENT_TUNNEL(OnTouchGesture);
	SWINDOW_TOUCHEVENT_TUNNEL(OnTouchStarted);
	SWINDOW_TOUCHEVENT_TUNNEL(OnTouchMoved);
	SWINDOW_TOUCHEVENT_TUNNEL(OnTouchForceChanged);
	SWINDOW_TOUCHEVENT_TUNNEL(OnTouchEnded);
	SWINDOW_TOUCHEVENT_TUNNEL(OnMouseButtonDown);
	SWINDOW_TOUCHEVENT_TUNNEL(OnMouseButtonUp);
	SWINDOW_TOUCHEVENT_TUNNEL(OnMouseMove);

	auto refWindow = sWindow.ToSharedRef();
	FSlateApplication& slateApp = FSlateApplication::Get();
	slateApp.AddWindow(refWindow, true);

	outContent = Content = CreateWidget<UUserWidget>(owner, content);
	sContent = Content->TakeWidget();

	sBox = SNew(SInputPropagator)
		.BoxArgs(SBox::FArguments()
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.VAlign(EVerticalAlignment::VAlign_Fill)
			.WidthOverride(FOptionalSize(settings.Size.X))
			.HeightOverride(FOptionalSize(settings.Size.Y))
		)
		.TargetWorld(GetWorld())
		.HandleEvents(true)
		.PropagateKeys(false)
		.PropagateTouchGestures(false)
		.Content()[ sContent.ToSharedRef() ];

	FOnWindowClosed onclosed;
	onclosed.BindLambda([this](const TSharedRef < SWindow >& window)
	{
		OnWindowClosed.Broadcast();
	});
	sWindow->SetOnWindowClosed(onclosed);

	FOnWindowMoved onmoved;
	onmoved.BindLambda([this](const TSharedRef < SWindow >& window)
	{
		/*sBox->SetWidthOverride(FOptionalSize(window->GetClientSizeInScreen().X));
		sBox->SetHeightOverride(FOptionalSize(window->GetClientSizeInScreen().Y));*/
		OnWindowMoved.Broadcast(
			sWindow->GetPositionInScreen(),
			sWindow->GetSizeInScreen()
		);
	});
	sWindow->SetOnWindowMoved(onmoved);

	sWindow->SetContent(sBox.ToSharedRef());
	SetSettings(settings, true);
}

void UWidgetHostWindow::UpdateDisplayMetrics()
{
	FMonitorInfo prev;
	bool prevavailable = false;
	if(Monitors.Num() > 0)
	{
		prev = GetMonitor(Settings.TargetMonitor);
		prevavailable = true;
	}

	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
	Monitors.Empty();
	Monitors = DisplayMetrics.MonitorInfo;

	int64 minleft = 0;
	int64 mintop = 0;

	for(auto it = Monitors.CreateConstIterator(); it; ++it)
	{
		auto monitor = *it;
		minleft = FMath::Min(minleft, (int64)monitor.DisplayRect.Left);
		mintop = FMath::Min(mintop, (int64)monitor.DisplayRect.Top);
	}

	Monitors.Sort([this, minleft, mintop](const FMonitorInfo& A, const FMonitorInfo& B)
	{
		return GetMonitorOrderComparer(A, minleft, mintop) < GetMonitorOrderComparer(B, minleft, mintop);
	});

	FMonitorInfo curr = GetMonitor(Settings.TargetMonitor);

	if(prevavailable)
	{
		if (!prev.ID.Equals(curr.ID))
			SetSettings(Settings, true);
	}
}

uint64_t UWidgetHostWindow::GetMonitorOrderComparer(FMonitorInfo moninfo, int64 minleft, int64 mintop)
{
	uint64_t l = (uint64_t)((int64)moninfo.DisplayRect.Left - minleft) & 0x00000000FFFFFFFF;
	uint64_t t = (uint64_t)((int64)moninfo.DisplayRect.Top - mintop) & 0x00000000FFFFFFFF;
	uint64_t res = (l << 32) | t;
	return res;
}

#pragma optimize("", on)

void UWidgetHostWindow::BringToFront(bool bForce) { sWindow->BringToFront(bForce); }

void UWidgetHostWindow::SetSettings(FBlueWindowSettings settings, bool force)
{
	if (!sWindow.IsValid()) return;

	if (!Settings.Title.EqualTo(settings.Title) || force)
		sWindow->SetTitle(settings.Title);

	if (Settings.TargetMonitor != settings.TargetMonitor ||
		Settings.TopLeftOffset != settings.TopLeftOffset ||
		force)
	{
		FMonitorInfo targetMonitor = GetMonitor(settings.TargetMonitor);
		FVector2D offset = FVector2D(targetMonitor.WorkArea.Left, targetMonitor.WorkArea.Top) + settings.TopLeftOffset;
		sWindow->MoveWindowTo(offset);
	}

	if(Settings.Size != settings.Size || force)
	{
		sWindow->Resize(settings.Size);
		/*sBox->SetWidthOverride(FOptionalSize(settings.Size.X));
		sBox->SetHeightOverride(FOptionalSize(settings.Size.Y));*/
	}

	if(Settings.Opacity != settings.Opacity || force)
		sWindow->SetOpacity(settings.Opacity);

	if (Settings.WindowMode != settings.WindowMode || force)
		sWindow->SetWindowMode((EWindowMode::Type)settings.WindowMode);

	Settings = settings;
}

void UWidgetHostWindow::Close()
{
	if(!sWindow.IsValid()) return;
	sWindow->RequestDestroyWindow();
}

void UWidgetHostWindow::AddInputTargetWidget(UUserWidget* widget)
{
	GlobalInputProcessors->AddInputTargetWidget(widget);
}

void UWidgetHostWindow::RemoveInputTargetWidget(UUserWidget* widget)
{
	GlobalInputProcessors->RemoveInputTargetWidget(widget);
}

