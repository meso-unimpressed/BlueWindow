// Developed by MESO Digital Interiors GmbH. for HERE Technologies Inc. Confidential content only accessible to MESO Digital Interiors GmbH. employees


#include "PixelstreamingManager.h"

FString PixelstreamingManager::PixelstreamingIP = "";
bool PixelstreamingManager::PixelstreamingActive = false;

PixelstreamingManager::PixelstreamingManager()
{
}

PixelstreamingManager::~PixelstreamingManager()
{
}

bool PixelstreamingManager::InitPixelstreamingManager(FString IP)
{
	static bool Called;
	const bool InitCalled = Called;
	if (!Called)
	{
		if (IP != "")
		{
			PixelstreamingIP = IP;
			PixelstreamingActive = true;
		}
		else
		{
			PixelstreamingActive = false;
		}
		Called = true;
	}

	return !InitCalled;
}

bool PixelstreamingManager::GetPixelstreamingActive()
{
	return PixelstreamingActive;
}

FString PixelstreamingManager::GetPixelStreamingIP()
{
	return PixelstreamingIP;
}
