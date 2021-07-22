// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlueWindow.h"


#include "Misc/CommandLine.h"
#include "PixelstreamingManager.h"

#define LOCTEXT_NAMESPACE "FBlueWindowModule"

void FBlueWindowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PixelStreamingIP = "";
	if (FParse::Value(FCommandLine::Get(), TEXT("PixelstreamingIP"), PixelStreamingIP))
	{
		PixelStreamingIP = PixelStreamingIP.Replace(TEXT("="), TEXT(""));
	}
	PixelstreamingManager::InitPixelstreamingManager(PixelStreamingIP);
}

void FBlueWindowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueWindowModule, BlueWindow)