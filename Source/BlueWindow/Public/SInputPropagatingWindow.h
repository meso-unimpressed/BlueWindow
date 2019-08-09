// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "GenericApplication.h"
#include "SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Engine/World.h"

/**
 * Widget which tunnels its events to the currently active PlayerController
 */
class BLUEWINDOW_API SInputPropagatingWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SInputPropagatingWindow)
		: _PropagateKeys(true)
		, _PropagateTouches(false)
		, _PropagateTouchGestures(false)
		, _HandleEvents(false)
		, _TargetWorld(nullptr)
	{}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ARGUMENT(SWindow::FArguments, WindowArgs)
		SLATE_ARGUMENT(bool, PropagateKeys)
		SLATE_ARGUMENT(bool, PropagateTouches)
		SLATE_ARGUMENT(bool, PropagateTouchGestures)
		SLATE_ARGUMENT(bool, HandleEvents)
		SLATE_ARGUMENT(UWorld*, TargetWorld)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	FArguments Arguments;

public:

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent) override;

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply OnTouchFirstMove(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
};
