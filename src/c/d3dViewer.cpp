////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "Global\Vec.h"
#include "mex.h"
#include "Messages\MessageQueue.h"
#include "windows.h"
#include "Global\Globals.h"
#include "Mex\MexFunctions.h"
#include <set>
#include "Messages\MexErrorMsg.h"

HANDLE gTermEvent = NULL;
volatile bool gRendererInit = false;

HINSTANCE gDllInstance = NULL;

MessageQueue gMsgQueueToMex;

std::map<int, GraphicObjectNode*> gGraphicObjectNodes[GraphicObjectTypes::VTend];
PolygonObject* gBorderObj = NULL;

extern std::vector<DirectX::XMVECTOR> volumeBoundingVerts;

// A few of global helper functions for managing the graphic object node list
void insertGlobalGraphicsObject(GraphicObjectTypes objType, GraphicObjectNode* node, int forceLabel /* = -1*/)
{
	int uniqueID = forceLabel;
	if ( forceLabel < 0 )
		uniqueID = node->getPolygonLabel();

	if (gGraphicObjectNodes[objType].count(uniqueID) > 0)
	{
		sendErrMessage("You can't add a hull that already exists!");
		return;
	}

	std::pair<int, GraphicObjectNode*> newPolygon(uniqueID, node);
	gGraphicObjectNodes[objType].insert(newPolygon);
}

void removeGlobalGraphicsObject(GraphicObjectTypes objType, unsigned int uniqueID)
{
	if (gGraphicObjectNodes[objType].count(uniqueID) == 0)
		return;

	gGraphicObjectNodes[objType].erase(uniqueID);
}

GraphicObjectNode* getGlobalGraphicsObject(GraphicObjectTypes objType, unsigned int uniqueID)
{
	if (gGraphicObjectNodes[objType].count(uniqueID) == 0)
		return NULL;

	return gGraphicObjectNodes[objType][uniqueID];
}

