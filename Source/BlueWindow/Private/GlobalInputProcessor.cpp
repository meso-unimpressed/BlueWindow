// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobalInputProcessor.h"

#include "UserWidget.h"

#define DECL_KEYEVENT(name) \
	void UGlobalInputProcessor::##name##_Implementation(FGeometry geometry, FKeyEvent keyEvent) \
	{ \
		for (UUserWidget* tunnelto : TunnelToWidgets) \
			tunnelto->##name##(tunnelto->GetCachedGeometry(), keyEvent); \
		##name##Event.Broadcast(geometry, keyEvent); \
	}

#define DECL_TOUCHEVENT(name) \
	void UGlobalInputProcessor::##name##_Implementation(FGeometry geometry, FPointerEvent pointerEvent) \
	{ \
		for (UUserWidget* tunnelto : TunnelToWidgets) \
			tunnelto->##name##(tunnelto->GetCachedGeometry(), pointerEvent); \
		##name##Event.Broadcast(geometry, pointerEvent); \
	}

#define DECL_KEYEVENT_TUNNEL(name) \
	void UInputProcessorCollection::##name##(FGeometry geometry, FKeyEvent keyEvent) \
	{ \
		for (auto kvp : Map) \
			kvp.Value->##name##(geometry, keyEvent); \
	}

#define DECL_TOUCHEVENT_TUNNEL(name) \
	void UInputProcessorCollection::##name##(FGeometry geometry, FPointerEvent pointerEvent) \
	{ \
		for (auto kvp : Map) \
			kvp.Value->##name##(geometry, pointerEvent); \
	}

void UInputProcessorCollection::AddInputTargetWidget(UUserWidget* widget)
{
	for (auto kvp : Map)
		kvp.Value->AddTargetWidget(widget);
}

void UInputProcessorCollection::RemoveInputTargetWidget(UUserWidget* widget)
{
	for (auto kvp : Map)
		kvp.Value->AddTargetWidget(widget);
}

DECL_KEYEVENT_TUNNEL(OnKeyDown)
DECL_KEYEVENT_TUNNEL(OnKeyUp)
DECL_TOUCHEVENT_TUNNEL(OnTouchGesture)
DECL_TOUCHEVENT_TUNNEL(OnTouchStarted)
DECL_TOUCHEVENT_TUNNEL(OnTouchMoved)
DECL_TOUCHEVENT_TUNNEL(OnTouchForceChanged)
DECL_TOUCHEVENT_TUNNEL(OnTouchEnded)
DECL_TOUCHEVENT_TUNNEL(OnMouseButtonDown)
DECL_TOUCHEVENT_TUNNEL(OnMouseButtonUp)
DECL_TOUCHEVENT_TUNNEL(OnMouseMove)

DECL_KEYEVENT(OnKeyDown)
DECL_KEYEVENT(OnKeyUp)
DECL_TOUCHEVENT(OnTouchGesture)

void UGlobalInputProcessor::OnTouchStarted_Implementation(FGeometry geometry, FPointerEvent pointerEvent)
{
	for (UUserWidget* tunnelto : TunnelToWidgets)
		tunnelto->OnTouchStarted(tunnelto->GetCachedGeometry(), pointerEvent);
	OnTouchStartedEvent.Broadcast(geometry, pointerEvent);
}

//DECL_TOUCHEVENT(OnTouchStarted)
DECL_TOUCHEVENT(OnTouchMoved)
DECL_TOUCHEVENT(OnTouchForceChanged)
DECL_TOUCHEVENT(OnTouchEnded)
DECL_TOUCHEVENT(OnMouseButtonDown)
DECL_TOUCHEVENT(OnMouseButtonUp)
DECL_TOUCHEVENT(OnMouseMove)

void UGlobalInputProcessor::AddTargetWidget(UUserWidget* widget)
{
	if (TunnelToWidgets.Contains(widget)) return;
	TunnelToWidgets.Add(widget);
}

void UGlobalInputProcessor::RemoveTargetWidget(UUserWidget* widget)
{
	if (!TunnelToWidgets.Contains(widget)) return;
	TunnelToWidgets.Remove(widget);
}

