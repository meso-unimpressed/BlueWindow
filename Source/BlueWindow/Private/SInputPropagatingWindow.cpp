// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "SInputPropagatingWindow.h"
#include "SlateOptMacros.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#define IS_EVENT_ENABLED(event) if(!Arguments._##event) return FReply::Unhandled()

#define GET_PLAYERCONTROLLER \
	if(!Arguments._TargetWorld) return FReply::Unhandled(); \
	APlayerController* player = UGameplayStatics::GetPlayerController(Arguments._TargetWorld, 0)

#define HANDLE_EVENT return Arguments._HandleEvents ? FReply::Handled() : FReply::Unhandled()

void SInputPropagatingWindow::Construct(const FArguments& InArgs)
{
	Arguments = InArgs;
	SWindow::Construct(InArgs._WindowArgs);
}

FReply SInputPropagatingWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	SWindow::OnKeyDown(MyGeometry, InKeyEvent);
	IS_EVENT_ENABLED(PropagateKeys);
	GET_PLAYERCONTROLLER;
	player->InputKey(InKeyEvent.GetKey(), EInputEvent::IE_Pressed, 1.0f, false);
	HANDLE_EVENT;
}

FReply SInputPropagatingWindow::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	SWindow::OnKeyUp(MyGeometry, InKeyEvent);
	IS_EVENT_ENABLED(PropagateKeys);
	GET_PLAYERCONTROLLER;
	player->InputKey(InKeyEvent.GetKey(), EInputEvent::IE_Released, 1.0f, false);
	HANDLE_EVENT;
}

FReply SInputPropagatingWindow::OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent)
{
	SWindow::OnTouchGesture(MyGeometry, GestureEvent);
	IS_EVENT_ENABLED(PropagateTouchGestures);
	GET_PLAYERCONTROLLER;
	player->InputKey(GestureEvent.GetEffectingButton(), EInputEvent::IE_Pressed, 1.0f, false);
	player->InputAxis(GestureEvent.GetEffectingButton(), GestureEvent.GetGestureDelta().X, 0.0166f, 1, false);
	HANDLE_EVENT;
}

FReply SInputPropagatingWindow::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	SWindow::OnTouchStarted(MyGeometry, InTouchEvent);
	IS_EVENT_ENABLED(PropagateTouches);
	GET_PLAYERCONTROLLER;
	player->InputTouch(
		InTouchEvent.GetPointerIndex(),
		ETouchType::Began,
		InTouchEvent.GetScreenSpacePosition(),
		InTouchEvent.GetTouchForce(),
		FDateTime::Now(),
		InTouchEvent.GetTouchpadIndex()
	);
	HANDLE_EVENT;
}

FReply SInputPropagatingWindow::OnTouchFirstMove(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	SWindow::OnTouchFirstMove(MyGeometry, TouchEvent);
	IS_EVENT_ENABLED(PropagateTouches);
	GET_PLAYERCONTROLLER;
	player->InputTouch(
		TouchEvent.GetPointerIndex(),
		ETouchType::FirstMove,
		TouchEvent.GetScreenSpacePosition(),
		TouchEvent.GetTouchForce(),
		FDateTime::Now(),
		TouchEvent.GetTouchpadIndex()
	);
	HANDLE_EVENT;
}

FReply SInputPropagatingWindow::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	SWindow::OnTouchMoved(MyGeometry, InTouchEvent);
	IS_EVENT_ENABLED(PropagateTouches);
	GET_PLAYERCONTROLLER;
	player->InputTouch(
		InTouchEvent.GetPointerIndex(),
		ETouchType::Moved,
		InTouchEvent.GetScreenSpacePosition(),
		InTouchEvent.GetTouchForce(),
		FDateTime::Now(),
		InTouchEvent.GetTouchpadIndex()
	);
	HANDLE_EVENT;
}

FReply SInputPropagatingWindow::OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
	SWindow::OnTouchForceChanged(MyGeometry, TouchEvent);
	IS_EVENT_ENABLED(PropagateTouches);
	GET_PLAYERCONTROLLER;
	player->InputTouch(
		TouchEvent.GetPointerIndex(),
		ETouchType::ForceChanged,
		TouchEvent.GetScreenSpacePosition(),
		TouchEvent.GetTouchForce(),
		FDateTime::Now(),
		TouchEvent.GetTouchpadIndex()
	);
	HANDLE_EVENT;
}

FReply SInputPropagatingWindow::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
	SWindow::OnTouchEnded(MyGeometry, InTouchEvent);
	IS_EVENT_ENABLED(PropagateTouches);
	GET_PLAYERCONTROLLER;
	player->InputTouch(
		InTouchEvent.GetPointerIndex(),
		ETouchType::Ended,
		InTouchEvent.GetScreenSpacePosition(),
		InTouchEvent.GetTouchForce(),
		FDateTime::Now(),
		InTouchEvent.GetTouchpadIndex()
	);
	HANDLE_EVENT;
}
