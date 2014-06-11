#pragma once
#include "Renderer.h"
#include "SceneNode.h"
#include "Camera.h"
#include "windows.h"
#include "DirectXMath.h"

//Implemented in lever-3d_mex.cpp
extern HANDLE gTermEvent;
extern HWND gWindowHandle;
extern HINSTANCE gDllInstance;
extern HFONT gFont;
extern volatile bool gRendererInit;
extern CellHullObject* gBorderObj;

//Implemented in Initialization.cpp
extern unsigned int gWindowWidth;
extern unsigned int gWindowHeight;
extern Renderer* gRenderer;
extern OrthoCamera* gCameraWidget;
extern Camera* gCameraDefaultMesh;

//Implemented in MessageProcessor.cpp
extern bool gRendererOn;