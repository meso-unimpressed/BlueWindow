// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BlueWindowEditor.h"


#include "BlueWindowEditorBPLibrary.h"
#include "Misc/CommandLine.h"

#define LOCTEXT_NAMESPACE "FBlueWindowEditorModule"

void FBlueWindowEditorModule::StartupModule()
{
	// Initialize static fields inside the function
	UBlueWindowEditorBPLibrary::GetActiveEditorViewportChanged();
}

void FBlueWindowEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueWindowEditorModule, BlueWindowEditor)