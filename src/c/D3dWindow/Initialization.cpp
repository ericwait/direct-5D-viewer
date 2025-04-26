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

#include "Global/Globals.h"
#include "D3d/Renderer.h"
#include "D3d/SceneNode.h"

#include "Initialization.h"
#include "MessageProcessor.h"

#include "D3d/RenderInit.h"

HWND gWindowHandle = NULL;
const TCHAR szWndClassName[] = TEXT("LEVER 3-D");

const unsigned int initWindowHeight = 720;
const unsigned int initWindowWidth = 1280;

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
	RECT rc = { 0, 0, initWindowWidth, initWindowHeight};
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

HRESULT windowInit(HINSTANCE hInstance,int nCmdShow,std::string rootDir)
{
	HRESULT hr = registerWindowClass(hInstance,nCmdShow);
	if ( FAILED(hr) )
		return hr;

	Vec<int> viewportSize(initWindowWidth,initWindowHeight,1);
	return createRenderResources(rootDir, viewportSize, gWindowHandle);
}