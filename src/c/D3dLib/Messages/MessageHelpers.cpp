#include "MessageHelpers.h"

#include "Globals.h"

// Called from mex messages and from window proc
void setObjectTypeVisibility(GraphicObjectTypes type, bool visible)
{
	for ( auto& itr : gRenderer->allSceneObjects(type) )
		itr.second->setRenderable(visible);
}

void setObjectTypeColor(GraphicObjectTypes type, Vec<float> color, float alpha)
{
	for ( auto& itr : gRenderer->allSceneObjects(type) )
		itr.second->setColor(color, alpha);
}

void setObjectWireframe(GraphicObjectTypes type, bool wireframe)
{
	for ( auto& itr : gRenderer->allSceneObjects(type) )
		itr.second->setWireframe(wireframe);
}

void setObjectLighting(GraphicObjectTypes type, bool lightingOn)
{
	for ( auto& itr : gRenderer->allSceneObjects(type) )
		itr.second->setLightOn(lightingOn);
}
