// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "ManagableGameViewportClient.h"
#include "Engine/Engine.h"
#include "SWindow.h"

uint64_t UManagableGameViewportClient::GetMonitorOrderComparer(FMonitorInfo moninfo, int64 minleft, int64 mintop)
{
	uint64_t l = (uint64_t)((int64)moninfo.DisplayRect.Left - minleft) & 0x00000000FFFFFFFF;
	uint64_t t = (uint64_t)((int64)moninfo.DisplayRect.Top - mintop) & 0x00000000FFFFFFFF;
	uint64_t res = (l << 32) | t;
	return res;
}

FPointerEvent UManagableGameViewportClient::GetPointerEvent(uint32 Handle, FVector2D TouchLocation)
{
	return FPointerEvent(
		Handle,
		TouchLocation,
		TouchLocation,
		FVector2D::ZeroVector,
		FTouchKeySet::EmptySet,
		FModifierKeysState()
	);
}

FPointerEvent UManagableGameViewportClient::GetPointerEventWithDelta(uint32 Handle, FVector2D TouchLocation)
{
	if (!LastTouchLocations.Contains(Handle))
		LastTouchLocations.Add(Handle, TouchLocation);
	return FPointerEvent(
		Handle,
		TouchLocation,
		LastTouchLocations[Handle],
		TouchLocation - LastTouchLocations[Handle],
		FTouchKeySet::EmptySet,
		FModifierKeysState()
	);
}

void UManagableGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice /* = true */)
{
	UGameViewportClient::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	UpdateDisplayMetrics();
}

bool UManagableGameViewportClient::InputTouch(FViewport* Viewport, int32 ControllerId, uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	bool res = UGameViewportClient::InputTouch(Viewport, ControllerId, Handle, Type, TouchLocation, Force, DeviceTimestamp, TouchpadIndex);

	FPointerEvent pevent;
	switch (Type)
	{
	case ETouchType::Began:
		pevent = GetPointerEvent(Handle, TouchLocation);
		OnTouchBegin.Broadcast(pevent);
		LastTouchLocations.Add(Handle, TouchLocation);
		break;

	case ETouchType::Moved:
		pevent = GetPointerEventWithDelta(Handle, TouchLocation);
		OnTouchMove.Broadcast(pevent);
		LastTouchLocations[Handle] = TouchLocation;
		break;

	case ETouchType::Stationary:
		pevent = GetPointerEventWithDelta(Handle, TouchLocation);
		OnTouchStationary.Broadcast(pevent);
		LastTouchLocations[Handle] = TouchLocation;
		break;

	case ETouchType::ForceChanged:
		break;

	case ETouchType::FirstMove:
		pevent = GetPointerEventWithDelta(Handle, TouchLocation);
		OnTouchFirstMove.Broadcast(pevent);
		LastTouchLocations[Handle] = TouchLocation;
		break;

	case ETouchType::Ended:
		pevent = GetPointerEvent(Handle, TouchLocation);
		OnTouchEnded.Broadcast(pevent);
		LastTouchLocations.Remove(Handle);
		break;

	default:
		break;
	}

	return res;
}

void UManagableGameViewportClient::UpdateDisplayMetrics()
{
	FMonitorInfo prev;
	bool prevavailable = false;
	if (Monitors.Num() > 0)
	{
		prev = GetMonitor(Settings.TargetMonitor);
		prevavailable = true;
	}

	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
	Monitors.Empty();
	Monitors = DisplayMetrics.MonitorInfo;

	int64 minleft = 0;
	int64 mintop = 0;

	for (auto it = Monitors.CreateConstIterator(); it; ++it)
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

	if (prevavailable)
	{
		if (!prev.ID.Equals(curr.ID))
			SetSettings(Settings, true);
	}
}

void UManagableGameViewportClient::SetSettings(FBlueWindowSettings settings, bool force)
{
#if WITH_EDITOR
	if (IsSimulateInEditorViewport() || bIsPlayInEditorViewport) return;
#endif

	auto sWindow = GetWindow();
	if (sWindow.IsValid()) return;

	if (Settings.TargetMonitor != settings.TargetMonitor ||
		Settings.TopLeftOffset != settings.TopLeftOffset ||
		force)
	{
		FMonitorInfo targetMonitor = GetMonitor(settings.TargetMonitor);
		FVector2D offset = FVector2D(targetMonitor.WorkArea.Left, targetMonitor.WorkArea.Top) + settings.TopLeftOffset;
		sWindow->MoveWindowTo(offset);
	}

	if (Settings.Size != settings.Size || force)
	{
		sWindow->Resize(settings.Size);
		ViewportFrame->ResizeFrame(settings.Size.X, settings.Size.Y, (EWindowMode::Type)settings.WindowMode);
	}

	if (Settings.WindowMode != settings.WindowMode || force)
	{
		sWindow->SetWindowMode((EWindowMode::Type)settings.WindowMode);
		ViewportFrame->ResizeFrame(settings.Size.X, settings.Size.Y, (EWindowMode::Type)settings.WindowMode);
	}

	Settings = settings;
}

UManagableGameViewportClient* UManagableGameViewportClient::GetManagableViewport(bool& Success)
{
	UManagableGameViewportClient* res = Cast<UManagableGameViewportClient>(GEngine->GameViewport);
	Success = res ? true : false;
	return res;
}
