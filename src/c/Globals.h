#pragma once
#include "Renderer.h"
#include "SceneNode.h"
#include "Camera.h"
#include "windows.h"

//extern unsigned int* gImage;

extern HANDLE gTermEvent;
extern HWND gWindowHandle;
extern HINSTANCE gDllInstance;
extern HFONT gFont;

extern unsigned int gWindowWidth;
extern unsigned int gWindowHeight;

//Implemented in Initialization.cpp
extern Renderer* gRenderer;
extern SceneNode* gRootSceneNode;
extern Camera* gDefaultMeshCamera;
//////////////////////These are to be deleted after testing
extern std::vector<Vec<unsigned int>> gFacesDebug;
extern std::vector<Vec<float>> gVertsDebug;
extern std::vector<Vec<float>> gNormsDebug;
