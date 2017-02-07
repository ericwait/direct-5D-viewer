#include "MessageHelpers.h"

#include "Global/Globals.h"

// Called from mex messages and from window proc
void setObjectTypeVisibility(GraphicObjectTypes type, bool visible)
{
	for ( auto itr : gGraphicObjectNodes[type] )
		itr.second->setRenderable(visible);
}

void setObjectTypeColor(GraphicObjectTypes type, Vec<float> color, float alpha)
{
	for ( auto itr : gGraphicObjectNodes[type] )
		itr.second->setColor(color, alpha);
}
