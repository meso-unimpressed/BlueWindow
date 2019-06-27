// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractiveSceneCapture2D.h"

#include "TraceEventActor.generated.h"

//class AInteractiveSceneCapture2D;
//struct FPointerRay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorPointerRayEventSignature, FPointerRay, Pointer);

UCLASS()
class BLUEWINDOW_API ATraceEventActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATraceEventActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	/**
	 * Event when a Pointer Ray intersecting this actor moves
	 */
	virtual void NotifyActorOnLineTraceHitBegin(FPointerRay Pointer);

	/**
	 * Event when a Pointer Ray intersects this actor begins on this actor
	 */
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|TraceEventActor")
		FActorPointerRayEventSignature OnLineTraceHitBegin;

	/**
	 * Event when a Pointer Ray intersects this actor begins on this actor
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "BeginLineTraceHit"), Category = "BlueWindow|TraceEventActor")
		void ReceiveActorOnLineTraceHitBegin(FPointerRay Pointer);


	/**
	 * Event when a Pointer Ray intersecting this actor moves
	 */
	virtual void NotifyActorOnLineTraceHitEnter(FPointerRay Pointer);

	/**
	 * Event when a Pointer Ray which began somewhere else intersects this actor
	 */
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|TraceEventActor")
		FActorPointerRayEventSignature OnLineTraceHitEnter;

	/**
	 * Event when a Pointer Ray which began somewhere else intersects this actor
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EnterLineTraceHit"), Category = "BlueWindow|TraceEventActor")
		void ReceiveActorOnLineTraceHitEnter(FPointerRay Pointer);


	/**
	 * Event when a Pointer Ray intersecting this actor moves
	 */
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|TraceEventActor")
		FActorPointerRayEventSignature OnLineTraceHitMove;

	/**
	 * Event when a Pointer Ray intersecting this actor moves
	 */
	virtual void NotifyActorOnLineTraceHitMove(FPointerRay Pointer);

	/**
	 * Event when a Pointer Ray intersecting this actor moves
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "MoveLineTraceHit"), Category = "BlueWindow|TraceEventActor")
		void ReceiveActorOnLineTraceHitMove(FPointerRay Pointer);


	/**
	 * Event when a Pointer Ray intersecting this actor leaves this actor
	 */
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|TraceEventActor")
		FActorPointerRayEventSignature OnLineTraceHitLeave;

	/**
	 * Event when a Pointer Ray intersecting this actor leaves this actor
	 */
	virtual void NotifyActorOnLineTraceHitLeave(FPointerRay Pointer);

	/**
	 * Event when a Pointer Ray intersecting this actor leaves this actor
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "LeaveLineTraceHit"), Category = "BlueWindow|TraceEventActor")
		void ReceiveActorOnLineTraceHitLeave(FPointerRay Pointer);


	/**
	 * Event when a Pointer Ray intersecting this actor dies on this actor
	 */
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|TraceEventActor")
		FActorPointerRayEventSignature OnLineTraceHitEnd;

	/**
	 * Event when a Pointer Ray intersecting this actor dies on this actor
	 */
	virtual void NotifyActorOnLineTraceHitEnd(FPointerRay Pointer);

	/**
	 * Event when a Pointer Ray intersecting this actor dies on this actor
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EndLineTraceHit"), Category = "BlueWindow|TraceEventActor")
		void ReceiveActorOnLineTraceHitEnd(FPointerRay Pointer);


	/**
	 * Event when a Pointer Ray intersecting this actor dies on this actor
	 */
	UPROPERTY(BlueprintAssignable, Category = "BlueWindow|TraceEventActor")
		FActorPointerRayEventSignature OnOriginatedLineTraceEnd;

	/**
	 * Event when a Pointer Ray intersecting this actor dies on this actor
	 */
	virtual void NotifyActorOnOriginatedLineTraceEnd(FPointerRay Pointer);

	/**
	 * Event when a Pointer Ray intersecting this actor dies on this actor
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EndOriginatedLineTrace"), Category = "BlueWindow|TraceEventActor")
		void ReceiveActorOnOriginatedLineTraceEnd(FPointerRay Pointer);
};
