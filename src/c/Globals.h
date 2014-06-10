#pragma once
#include "Renderer.h"
#include "SceneNode.h"
#include "Camera.h"
#include "windows.h"

extern HANDLE gTermEvent;
extern HWND gWindowHandle;
extern HINSTANCE gDllInstance;
extern HFONT gFont;
extern volatile bool gRendererInit;

//Implemented in Initialization.cpp
extern unsigned int gWindowWidth;
extern unsigned int gWindowHeight;
extern Renderer* gRenderer;
extern OrthoCamera* gCameraWidget;
extern Camera* gCameraDefaultMesh;
//////////////////////These are to be deleted after testing
// extern std::vector<Vec<unsigned int>> gFacesDebug;
// extern std::vector<Vec<float>> gVertsDebug;
// extern std::vector<Vec<float>> gNormsDebug;
