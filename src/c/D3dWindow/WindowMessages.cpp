#include "Globals.h"



bool MessageSetWindowSize::process()
{
	ClientResize(gWindowHandle, width, height);

	return true;
}
