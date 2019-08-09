// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "SInputPropagator.h"
#include "SlateOptMacros.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#define IS_EVENT_ENABLED(event) if(!Arguments._##event) return FReply::Unhandled()

#define GET_PLAYERCONTROLLER \
	if(!Arguments._TargetWorld) return FReply::Unhandled(); \
	APlayerController* player = UGameplayStatics::GetPlayerController(Arguments._TargetWorld, 0)

#define HANDLE_EVENT return Arguments._HandleEvents ? FReply::Handled() : FReply::Unhandled()

void SInputPropagator::Construct(const FArguments& InArgs)
{
	Arguments = InArgs;
	SBox::FArguments boxargs = InArgs._BoxArgs;
	SBox::Construct(boxargs.Content()[InArgs._Content.Widget]);
	bCanSupportFocus = true;

	//TODO: Keyboard doesn't get captured
}

FReply SInputPropagator::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	IS_EVENT_ENABLED(PropagateKeys);
	GET_PLAYERCONTROLLER;
	player->InputKey(InKeyEvent.GetKey(), EInputEvent::IE_Pressed, 1.0f, false);
	HANDLE_EVENT;
}

FReply SInputPropagator::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	IS_EVENT_ENABLED(PropagateKeys);
	GET_PLAYERCONTROLLER;
	player->InputKey(InKeyEvent.GetKey(), EInputEvent::IE_Released, 1.0f, false);
	HANDLE_EVENT;
}

FReply SInputPropagator::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	IS_EVENT_ENABLED(PropagateMouse);
	GET_PLAYERCONTROLLER;
	player->InputKey(MouseEvent.GetEffectingButton(), EInputEvent::IE_Pressed, 1.0f, false);
	HANDLE_EVENT;
}

FReply SInputPropagator::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	IS_EVENT_ENABLED(PropagateMouse);
	GET_PLAYERCONTROLLER;
	player->InputKey(MouseEvent.GetEffectingButton(), EInputEvent::IE_Released, 1.0f, false);
	HANDLE_EVENT;
}

FReply SInputPropagator::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//TODO
	
	//IS_EVENT_ENABLED(PropagateMouse);
	//GET_PLAYERCONTROLLER;
	//player->InputAxis( /* ? */, EInputEvent::IE_Pressed, 1.0f, false);
	//HANDLE_EVENT;
	return FReply::Unhandled();
}

FReply SInputPropagator::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//TODO

	//IS_EVENT_ENABLED(PropagateMouse);
	//GET_PLAYERCONTROLLER;
	//player->InputAxis( /* ? */, EInputEvent::IE_Pressed, 1.0f, false);
	//HANDLE_EVENT;
	return FReply::Unhandled();
}

FReply SInputPropagator::OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent)
{
	IS_EVENT_ENABLED(PropagateTouchGestures);
	GET_PLAYERCONTROLLER;
	player->InputKey(GestureEvent.GetEffectingButton(), EInputEvent::IE_Pressed, 1.0f, false);
	player->InputAxis(GestureEvent.GetEffectingButton(), GestureEvent.GetGestureDelta().X, 0.0166f, 1, false);
	HANDLE_EVENT;
}

FReply SInputPropagator::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
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

FReply SInputPropagator::OnTouchFirstMove(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
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

FReply SInputPropagator::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
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

FReply SInputPropagator::OnTouchForceChanged(const FGeometry& MyGeometry, const FPointerEvent& TouchEvent)
{
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

FReply SInputPropagator::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
{
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
