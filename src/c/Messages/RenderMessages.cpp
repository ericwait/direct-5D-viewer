#include "RenderMessages.h"

#include "Global/Globals.h"

bool MessageUpdateRender::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->forceUpdate();

	return true;
}