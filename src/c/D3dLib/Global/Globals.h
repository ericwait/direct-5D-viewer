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

#pragma once
#include "Global/Color.h"
#include "Global/Vec.h"
#include "D3d/Renderer.h"
#include "D3d/Camera.h"
#include "D3d/SceneNode.h"

#include "Global/Defines.h"
#include "Messages/MessageQueue.h"
#include "Messages/ReturnQueue.h"

#include <Windows.h>

#undef max
#undef min

//Implemented in lever-3d_mex.cpp
extern HANDLE gTermEvent;
extern volatile bool gRendererInit;

//Implemented in Renderer.cpp
extern Renderer* gRenderer;
extern Camera* gCameraDefaultMesh;
extern OrthoCamera* gCameraWidget;
extern TextCamera* gCameraText;

//Implemented in Renderer.cpp
extern bool gUpdateShaders;
extern bool gPlay;
extern bool gRotate;
extern bool gCapture;
extern float gFramesPerSec;

// Global Queues
extern MessageQueue gMsgQueueToDirectX;
extern ReturnQueue gMsgQueueToMex;
