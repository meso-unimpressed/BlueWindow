// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees

#pragma once

#include "CoreMinimal.h"

/**
 * Get pixel streaming infos passed in through command line arguments
 */


class BLUEWINDOW_API PixelstreamingManager
{
private:
	static bool PixelstreamingActive;
	static FString PixelstreamingIP;
	
public:
	PixelstreamingManager();
	~PixelstreamingManager();

	static bool InitPixelstreamingManager(FString IP = "");
	static bool GetPixelstreamingActive();
	static FString GetPixelStreamingIP();
};
