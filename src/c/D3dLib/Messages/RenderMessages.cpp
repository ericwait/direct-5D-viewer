#include "RenderMessages.h"

#include "Globals.h"

bool MessageUpdateRender::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->forceUpdate();

	return true;
}