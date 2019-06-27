// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "TraceEventActor.h"

// Sets default values
ATraceEventActor::ATraceEventActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATraceEventActor::BeginPlay()
{
	AActor::BeginPlay();
	
}

// Called every frame
void ATraceEventActor::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

}

void ATraceEventActor::NotifyActorOnLineTraceHitBegin(FPointerRay Pointer)
{
	ReceiveActorOnLineTraceHitBegin(Pointer);
}

void ATraceEventActor::NotifyActorOnLineTraceHitEnter(FPointerRay Pointer)
{
	ReceiveActorOnLineTraceHitEnter(Pointer);
}

void ATraceEventActor::NotifyActorOnLineTraceHitMove(FPointerRay Pointer)
{
	ReceiveActorOnLineTraceHitMove(Pointer);
}

void ATraceEventActor::NotifyActorOnLineTraceHitLeave(FPointerRay Pointer)
{
	ReceiveActorOnLineTraceHitLeave(Pointer);
}

void ATraceEventActor::NotifyActorOnLineTraceHitEnd(FPointerRay Pointer)
{
	ReceiveActorOnLineTraceHitEnd(Pointer);
}

