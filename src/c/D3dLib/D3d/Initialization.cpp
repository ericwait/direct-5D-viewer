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

#include "Globals.h"
#include "Initialization.h"
#include "MessageProcessor.h"
#include "Renderer.h"
#include "SceneNode.h"

HWND gWindowHandle = NULL;
const TCHAR szWndClassName[] = TEXT("LEVER 3-D");

unsigned int gWindowHeight = 720;
unsigned int gWindowWidth = 1280;

HFONT gFont = NULL;
Renderer* gRenderer = NULL;

Camera* gCameraDefaultMesh = NULL;
OrthoCamera* gCameraWidget = NULL;
TextCamera* gCameraText = NULL;

std::vector<Vec<unsigned int>> gFacesDebug;
std::vector<Vec<float>> gVertsDebug;
std::vector<Vec<float>> gNormsDebug;

HRESULT registerWindowClass(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;

	wcx.cbSize = sizeof(WNDCLASSEX);

	if ( !GetClassInfoEx(hInstance, szWndClassName, &wcx) )
	{
		wcx.style			= CS_HREDRAW | CS_VREDRAW;
		wcx.lpfnWndProc		= wndProc;
		wcx.cbClsExtra		= 0;
		wcx.cbWndExtra		= 0;
		wcx.hInstance		= hInstance;
		wcx.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
		wcx.hCursor			= LoadCursor(NULL, IDC_ARROW);
		//wcx.hCursor			= LoadCursor(hInstance, NULL);
		wcx.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcx.lpszMenuName	= NULL;
		wcx.lpszClassName	= szWndClassName;
		wcx.hIconSm			= NULL;

		if ( !RegisterClassEx( &wcx ) )
		{
			DWORD errCode = GetLastError();

			gMsgQueueToMex.addMessage("error","Unable to register window class!",errCode);

			return E_FAIL;
		}
	}

	// Create window
	RECT rc = { 0, 0, gWindowWidth, gWindowHeight};
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	gWindowHandle = CreateWindow(szWndClassName, "LEVer 3D viewer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top,NULL,NULL,hInstance,NULL);

	if( !gWindowHandle )
	{
		DWORD errCode = GetLastError();
		
		gMsgQueueToMex.addMessage("error","Unable to register window class!",errCode);

		return E_FAIL;
	}
	//SetCapture( gWindowHandle );

	ShowWindow( gWindowHandle, nCmdShow );
	UpdateWindow(gWindowHandle);

	return S_OK;
}

HRESULT createRenderResources(std::string rootDir, Vec<int> viewportSize)
{
	Vec<float> eye = Vec<float>(0.0f,0.0f,-3.0f);
	Vec<float> look = Vec<float>(0.0f,0.0f,0.0f);
	Vec<float> up = Vec<float>(0.0f,1.0f,0.0);
	
	gRenderer = new Renderer();//delete on message loop exit
	HRESULT hr = gRenderer->init(rootDir, viewportSize);

	if (FAILED(hr))
		return hr;

	gCameraDefaultMesh = new Camera(eye,look,up,viewportSize);//delete on message loop exit
	gCameraWidget = new OrthoCamera(eye,look,up,viewportSize);//delete on message loop exit
	gCameraText = new TextCamera(eye,look,up,viewportSize);

	return S_OK;
}

HRESULT windowInit(HINSTANCE hInstance,int nCmdShow,std::string rootDir)
{
	HRESULT hr = registerWindowClass(hInstance,nCmdShow);
	if ( FAILED(hr) )
		return hr;

	gFont = CreateFont(18,//Height
		0,          //Width
		0,          //Escapement
		0,          //Orientation
		FW_NORMAL,  //Weight
		false,      //Italic
		false,      //Underline
		false,      //Strikeout
		DEFAULT_CHARSET,//Charset 
		OUT_DEFAULT_PRECIS,  //Output Precision
		CLIP_DEFAULT_PRECIS, //Clipping Precision
		DEFAULT_QUALITY,     //Quality
		DEFAULT_PITCH|FF_DONTCARE, //Pitch and Family
		"Arial");


	Vec<int> viewportSize(gWindowWidth,gWindowHeight,1);
	return createRenderResources(rootDir, viewportSize);
}