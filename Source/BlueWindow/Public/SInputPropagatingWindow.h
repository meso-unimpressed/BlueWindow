// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "GenericApplication.h"
#include "SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Engine/World.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FPointerEventDelegate, FGeometry, FPointerEvent);
DECLARE_MULTICAST_DELEGATE_TwoParams(FKeyEventDelegate, FGeometry, FKeyEvent);

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

	~SInputPropagatingWindow();

private:

	FArguments Arguments;

	FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

public:

	FPointerEventDelegate OnMouseButtonDownEvent;

	FPointerEventDelegate OnMouseButtonUpEvent;

	FPointerEventDelegate OnMouseMoveEvent;

	FKeyEventDelegate OnKeyDownEvent;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	FKeyEventDelegate OnKeyUpEvent;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	FPointerEventDelegate OnTouchGestureEvent;
	virtual FReply OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent) override;

	FPointerEventDelegate OnTouchStartedEvent;
	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;

	FPointerEventDelegate OnTouchFirstMoveEvent;
	virtual FReply OnTouchFirstMove(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;

	FPointerEventDelegate OnTouchMovedEvent;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;

	FPointerEventDelegate OnTouchForceChangedEvent;
	virtual FReply OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;

	FPointerEventDelegate OnTouchEndedEvent;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
};
