// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "GenericApplication.h"
#include "SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Engine/World.h"

/**
 * Widget which tunnels its events to the currently active PlayerController
 */
class BLUEWINDOW_API SInputPropagator : public SBox
{

public:
	SLATE_BEGIN_ARGS(SInputPropagator)
		: _PropagateKeys(true)
		, _PropagateMouse(false)
		, _PropagateTouches(false)
		//, _PropagateAnalog(true)
		, _HandleEvents(false)
		, _TargetWorld(nullptr)
	{}
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ARGUMENT(SBox::FArguments, BoxArgs)
		SLATE_ARGUMENT(bool, PropagateKeys)
		SLATE_ARGUMENT(bool, PropagateMouse)
		SLATE_ARGUMENT(bool, PropagateTouches)
		SLATE_ARGUMENT(bool, PropagateTouchGestures)
		//SLATE_ARGUMENT(bool, PropagateAnalog)
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

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent) override;

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply OnTouchFirstMove(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual FReply OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
};
