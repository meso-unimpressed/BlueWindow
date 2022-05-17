#include "BlueWindowEditorBPLibrary.h"

#include "Editor.h"
#include "BlueWindowBPLibrary.h"
#include "LevelEditorViewport.h"
#include "SEditorViewport.h"
#include "Widgets/SOverlay.h"

TSharedPtr<SOverlay> UBlueWindowEditorBPLibrary::GetEditorViewportOverlay()
{
	if (!GEditor || !GEditor->GetActiveViewport()) return nullptr;

	const auto ViewportClient = static_cast<FLevelEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
	if (!ViewportClient) return nullptr;

	const auto VpRootWidget = ViewportClient->GetEditorViewportWidget();
	// Gets the next overlay widget in the hierarchy
	return StaticCastSharedPtr<SOverlay>(UBlueWindowBPLibrary::GetParentWidgetOfType(VpRootWidget, "SOverlay"));
}

void UBlueWindowEditorBPLibrary::AddWidgetToLevelViewportOverlays(UWidget* Widget)
{
	const auto Overlay = GetEditorViewportOverlay();
	if (Overlay.IsValid())
		UBlueWindowBPLibrary::AddWidgetToOverlay(Overlay, Widget);
}

void UBlueWindowEditorBPLibrary::RemoveWidgetFromLevelViewportOverlays(UWidget* Widget)
{
	if (!Widget) return;

	const auto Overlay = GetEditorViewportOverlay();
	if (Overlay.IsValid())
		UBlueWindowBPLibrary::RemoveWidgetFromParentOverlay(Widget);
}

bool UBlueWindowEditorBPLibrary::GetActiveEditorViewportChanged()
{
	if (GEditor)
	{
		static bool LastResult = false;
		if (static int64 LastCheckedFrame = 0; LastCheckedFrame != GFrameCounter)
		{
			static FViewport* LastFrameActiveViewport = nullptr;
			LastResult = LastFrameActiveViewport != GEditor->GetActiveViewport();
			LastFrameActiveViewport = GEditor->GetActiveViewport();
			LastCheckedFrame = GFrameCounter;
		}
		return LastResult;
	}
	return false;
}
