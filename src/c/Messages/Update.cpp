#include "Global/Vec.h"
#include "Messages/MessageQueue.h"
#include "Global/Globals.h"
#include "Messages/QueuePolygon.h"

#include "windows.h"

#include <set>
#include <vector>

using std::vector;

std::vector<SceneNode*> hullRootNodes;

void setCurrentTexture(GraphicObjectTypes textureType)
{
	if (gRenderer == NULL) return;

	for (int i = GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::VTend; ++i)
	{
		bool render = (i == textureType);
		for (auto itr : gGraphicObjectNodes[i])
			itr.second->setRenderable(render);
	}
}

void togglePolygons(bool on)
{
	if (gRenderer == NULL) return;

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	for (auto itr : gGraphicObjectNodes[cellType])
		itr.second->setRenderable(on);
}

void toggleWidget(bool on)
{
	if(gRenderer==NULL) return;

	const GraphicObjectTypes widgetType = GraphicObjectTypes::Widget;
	for(auto itr:gGraphicObjectNodes[widgetType])
		itr.second->setRenderable(on);
}

void togglePolygonWireframe(bool wireframe)
{
	if (gRenderer == NULL) return;

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	std::map<int, GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[cellType].begin();
	for ( ; objectIter != gGraphicObjectNodes[cellType].end(); ++objectIter )
		objectIter->second->setWireframe(wireframe);
}

void togglePolygonLighting(bool lighting)
{
	if (gRenderer == NULL) return;

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	for(auto itr:gGraphicObjectNodes[cellType])
		itr.second->setLightOn(lighting);
}

void toggleSelectedPolygon(std::set<int> labels)
{
	if (gRenderer == NULL) return;

	const GraphicObjectTypes cellType = GraphicObjectTypes::Polygons;
	for(auto itr:gGraphicObjectNodes[cellType])
	{
		GraphicObjectNode* node = itr.second;
		if ( labels.count(node->getIndex()) > 0 )
			node->setRenderable(true);
		else
			node->setRenderable(false);
	}
}


