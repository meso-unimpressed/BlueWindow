
#include "PointerRay.h"

void FPointerRay::Age(float delta)
{
	AgeInFrames++;
	AgeInSeconds += delta;
}