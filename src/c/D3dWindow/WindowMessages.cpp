#include "Global/Globals.h"



bool MessageSetWindowSize::process()
{
	ClientResize(gWindowHandle, width, height);

	return true;
}
