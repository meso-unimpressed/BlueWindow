// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "ManagableGameViewportClient.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SWindow.h"
#include "HardwareInfo.h"
#include "Blueprint/UserWidget.h"
#include "GlobalInputProcessor.h"

//#if PLATFORM_WINDOWS && !defined(WINDOWS_PLATFORM_TYPES_GUARD)
//#include "AllowWindowsPlatformTypes.h"
//#define LOCAL_WINDOWS_PLATFORM_TYPES_GUARD
//#endif
//
//#pragma warning (push)
//#pragma warning (disable : 4389; disable : 4018; disable : 4245)
//
//#include <d3d11.h>
//
//#pragma warning (pop)
//
//#if PLATFORM_WINDOWS && defined(LOCAL_WINDOWS_PLATFORM_TYPES_GUARD)
//#include "HideWindowsPlatformTypes.h"
//#undef LOCAL_WINDOWS_PLATFORM_TYPES_GUARD
//#endif

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

FPointerEvent UManagableGameViewportClient::GetMouseEventWithDelta(FVector2D MouseLocation)
{
	return FPointerEvent(
		0,
		MouseLocation,
		LastMouseLocation,
		MouseLocation - LastMouseLocation,
		FTouchKeySet::EmptySet,
		FModifierKeysState()
	);
}

void UManagableGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice /* = true */)
{
	UGameViewportClient::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	GlobalInputProcessors = NewObject<UInputProcessorCollection>(this);

	//currRHI = FHardwareInfo::GetHardwareInfo(NAME_RHI);
	//if (currRHI.Equals("D3D11"))
	//{
	//	D3D11Device = (ID3D11Device*)GDynamicRHI->RHIGetNativeDevice();
	//	D3D11Device->GetImmediateContext(&pImmediateContext);
	//}

	UpdateDisplayMetrics();
}

bool UManagableGameViewportClient::InputTouch(FViewport* TargetViewport, int32 ControllerId, uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	bool res = UGameViewportClient::InputTouch(TargetViewport, ControllerId, Handle, Type, TouchLocation, Force, DeviceTimestamp, TouchpadIndex);

	FPointerEvent pevent;
	FGeometry geom = GetGeometry();
	switch (Type)
	{
	case ETouchType::Began:
		pevent = GetPointerEvent(Handle, TouchLocation);
		GlobalInputProcessors->OnTouchStarted(geom, pevent);
		LastTouchLocations.Add(Handle, TouchLocation);
		break;

	case ETouchType::Moved:
		pevent = GetPointerEventWithDelta(Handle, TouchLocation);
		GlobalInputProcessors->OnTouchMoved(geom, pevent);
		LastTouchLocations[Handle] = TouchLocation;
		break;

	case ETouchType::ForceChanged:
		pevent = GetPointerEvent(Handle, TouchLocation);
		GlobalInputProcessors->OnTouchForceChanged(geom, pevent);
		break;

	case ETouchType::Ended:
		pevent = GetPointerEvent(Handle, TouchLocation);
		GlobalInputProcessors->OnTouchEnded(geom, pevent);
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

UTexture2D* UManagableGameViewportClient::GetViewportTexture()
{
	return ViewportCopy;
}

void UManagableGameViewportClient::SetSettings(FBlueWindowSettings settings, bool force)
{
#if WITH_EDITOR
	if (IsSimulateInEditorViewport() || bIsPlayInEditorViewport) return;
#endif

	auto sWindow = GetWindow();

	if (!Viewport) return;
	if (!sWindow) return;

	FMonitorInfo targetMonitor = GetMonitor(settings.TargetMonitor);
	auto monwa = targetMonitor.DisplayRect;

	if (settings.WindowMode == EBlueWindowMode::Fullscreen ||
		settings.WindowMode == EBlueWindowMode::WindowedFullscreen)
	{
		Viewport->MoveWindow(
			monwa.Left, monwa.Top,
			monwa.Right - monwa.Left, monwa.Bottom - monwa.Top
		);
		sWindow->MoveWindowTo(FVector2D(monwa.Left, monwa.Top));
		sWindow->Resize(FVector2D(monwa.Right - monwa.Left, monwa.Bottom - monwa.Top));
		sWindow->SetWindowMode(EWindowMode::WindowedFullscreen);
	}
	else
	{
		FVector2D offset = FVector2D(monwa.Left, monwa.Top) + settings.TopLeftOffset;
		Viewport->MoveWindow(
			offset.X, offset.Y,
			settings.Size.X, settings.Size.Y
		);
		sWindow->MoveWindowTo(offset);
		sWindow->Resize(settings.Size);
	}

	//ViewportFrame->ResizeFrame(settings.Size.X, settings.Size.Y, (EWindowMode::Type)settings.WindowMode);

	Settings = settings;
}

UManagableGameViewportClient* UManagableGameViewportClient::GetManagableViewport(bool& Success)
{
	UManagableGameViewportClient* res = Cast<UManagableGameViewportClient>(GEngine->GameViewport);
	Success = res ? true : false;
	return res;
}

void UManagableGameViewportClient::AddInputTargetWidget(UUserWidget* widget)
{
	GlobalInputProcessors->AddInputTargetWidget(widget);
}

void UManagableGameViewportClient::RemoveInputTargetWidget(UUserWidget* widget)
{
	GlobalInputProcessors->AddInputTargetWidget(widget);
}

void UManagableGameViewportClient::Tick(float DeltaTime)
{
	UGameViewportClient::Tick(DeltaTime);

	if (!Viewport) return;

	FIntPoint vps = Viewport->GetSizeXY();

	//GEngine->GameViewport->GetGameViewport()->UpdateRenderTargetSurfaceRHIToCurrentBackBuffer();

	ENQUEUE_RENDER_COMMAND(void)([this, vps](FRHICommandListImmediate& RHICmdList)
	{
		if (!Viewport) return;

		auto RhiVpTex = Viewport->GetRenderTargetTexture();
		auto RhiVp = Viewport->GetViewportRHI();

		if(!RhiVpTex && RhiVp)
		{
			RhiVpTex = RHIGetViewportBackBuffer(RhiVp);
		}
		if (!RhiVpTex) return;

		if (!ViewportCopy)
		{
			ViewportCopy = UTexture2D::CreateTransient(vps.X, vps.Y, RhiVpTex->GetFormat());
			ViewportCopy->UpdateResource();
		}

		if (
			ViewportCopy->GetSizeX() != vps.X || ViewportCopy->GetSizeY() != vps.Y ||
			ViewportCopy->GetPixelFormat() != RhiVpTex->GetFormat()
		) {
			ViewportCopy = UTexture2D::CreateTransient(vps.X, vps.Y, RhiVpTex->GetFormat());
			ViewportCopy->UpdateResource();
		}

		FRHICopyTextureInfo info;
		info.DestMipIndex = 0;
		info.DestPosition = FIntVector::ZeroValue;
		info.DestSliceIndex = 0;
		info.NumMips = 1;
		info.NumSlices = 1;
		info.Size = FIntVector(RhiVpTex->GetSizeX(), RhiVpTex->GetSizeY(), 1);
		info.SourceMipIndex = 0;
		info.SourcePosition = FIntVector::ZeroValue;
		info.SourceSliceIndex = 0;

		auto dstRes = (FTexture2DResource*)ViewportCopy->Resource;
		GDynamicRHI->RHIGetDefaultContext()->RHICopyTexture(
			RhiVpTex.GetReference(),
			dstRes->GetTexture2DRHI(),
			info
		);
	});
}

void UManagableGameViewportClient::MouseMove(FViewport* InViewport, int32 X, int32 Y)
{
	UGameViewportClient::MouseMove(InViewport, X, Y);
	FVector2D currpos = FVector2D(X, Y);
	if (FMath::IsNearlyZero(FVector2D::Distance(currpos, LastMouseLocation))) return;

	FPointerEvent resEvent = GetMouseEventWithDelta(currpos);
	GlobalInputProcessors->OnMouseMove(GetGeometry(), resEvent);

	LastMouseLocation = currpos;
}

void UManagableGameViewportClient::Activated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent)
{
	Super::Activated(InViewport, InActivateEvent);

	SetSettings(Settings, true);
}

FGeometry UManagableGameViewportClient::GetGeometry()
{
	FVector2D vpsize; GetViewportSize(vpsize);
	return FGeometry(FVector2D::ZeroVector, FVector2D::ZeroVector, vpsize, 1.0f);
}
