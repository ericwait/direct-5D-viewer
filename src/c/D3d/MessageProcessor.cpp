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

#include "MessageProcessor.h"
#include "Initialization.h"
#include "Global/Globals.h"
#include "Global/ErrorMsg.h"
#include "Global/ModuleInfo.h"
#include "Global/WidgetData.h"
#include "Messages/LoadData.h"
#include "Messages/MessageHelpers.h"

#include <time.h>
#include <thread>

bool gUpdateShaders = true;
bool gPlay = false;
bool gRotate = false;
bool gCapture = false;

float gFramesPerSec = 5;
float numAngles = 720.0f;

const DirectX::XMMATRIX ROT_X = DirectX::XMMatrixRotationY(-1.0f/(numAngles)*DirectX::XM_2PI);

void ClientResize(HWND hWnd, int nWidth, int nHeight)
{
	RECT rcClient, rcWind;
	POINT ptDiff;
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWind);
	ptDiff.x = (rcWind.right-rcWind.left)-rcClient.right;
	ptDiff.y = (rcWind.bottom-rcWind.top)-rcClient.bottom;
	MoveWindow(hWnd, rcWind.left, rcWind.top, nWidth+ptDiff.x, nHeight+ptDiff.y, TRUE);
}

LRESULT CALLBACK wndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	if(message==WM_DESTROY)
		PostQuitMessage(0);

	// Current mouse position
	int iMouseX = (short)LOWORD(lParam);
	int iMouseY = (short)HIWORD(lParam);
	HMENU hmenu = GetMenu(hWnd);

	static bool leftButtonDown = false;
	static bool shiftDown = false;
	static bool ctrlDown = false;
	static bool altDown = false;
	static int previousMouseX = 0;
	static int previousMouseY = 0;
	static DirectX::XMMATRIX previousWorldRotation;
	static float previousPeel;
	static bool minimized = false;
	static bool hullsOn = true;
	static bool widgetOn = true;

	float alpha = (altDown) ? (0.1f) : (1.0f);

	float wheelTicks = 0.0f;

	int index;
	Vec<float> pnt;
	Vec<float> direction;

	switch(message)
	{
	case WM_NULL:
		break;
	case WM_CREATE:
		break;
	case WM_MOVE:
		break;
	case WM_SIZE:
		if(wParam == SIZE_MINIMIZED)
		{
			gRenderer->setRendering(false);
			minimized = true;
			break;
		}
		else if(wParam == SIZE_MAXIMIZED || (wParam == SIZE_RESTORED && minimized))
		{
			gRenderer->setRendering(true);
			minimized = false;
		}

		gWindowWidth = LOWORD(lParam);
		gWindowHeight = HIWORD(lParam);
		if(gRenderer != NULL)
		{
			gRenderer->resizeViewPort();
			gRenderer->forceUpdate();
		}
		break;
	case WM_MOUSEWHEEL:
		wheelTicks = ((float)GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
		if(shiftDown && ctrlDown)
		{
			float nearZ = gCameraDefaultMesh->getNearZ();
			nearZ += 0.05f * alpha * wheelTicks;

			gCameraDefaultMesh->setNearZ(nearZ);
		}
		else if (ctrlDown)
		{
			float frontClipPos = gRenderer->FrontClipPos();
			frontClipPos += 0.01f * alpha * wheelTicks;

			gRenderer->FrontClipPos(frontClipPos);
		}
		else if (shiftDown)
		{
			float backClipPos = gRenderer->BackClipPos();
			backClipPos += 0.01f * alpha * wheelTicks;

			gRenderer->BackClipPos(backClipPos);
		}
		else
		{
			gCameraDefaultMesh->move(Vec<float>(0.0f, 0.0f, 0.3f*alpha*wheelTicks));
		}
		gRenderer->forceUpdate();
		break;
	case WM_MOUSEMOVE:
		if(leftButtonDown)
		{
			DirectX::XMMATRIX rotX,rotY;
			if(shiftDown)
				rotX = DirectX::XMMatrixIdentity();
			else
				rotX = DirectX::XMMatrixRotationY(((float)(iMouseX - previousMouseX) / gWindowWidth)*DirectX::XM_2PI*alpha);

			if(ctrlDown)
				rotY = DirectX::XMMatrixIdentity();
			else
				rotY = DirectX::XMMatrixRotationX((-(float)(iMouseY - previousMouseY) / gWindowHeight)*DirectX::XM_2PI*alpha);

			gRenderer->setWorldRotation(previousWorldRotation*rotX*rotY);
			gRenderer->forceUpdate();
		}
		break;
	case WM_LBUTTONDOWN:
		leftButtonDown = true;
		previousMouseX = iMouseX;
		previousMouseY = iMouseY;
		previousWorldRotation = gRenderer->getRootWorldRotation();
		previousPeel = gRenderer->getClipChunkPercent();
		if(previousPeel>0.2f)
			gRenderer->setClipChunkPercent(0.1f);
		break;
	case WM_RBUTTONDOWN:
		gCameraDefaultMesh->getRay(iMouseX,iMouseY,pnt,direction);
		index = gRenderer->getPolygon(pnt,direction);
		if(ctrlDown)
		{
			gMsgQueueToMex.addMessage("cellSelected",index);
		}
		else
		{
			gMsgQueueToMex.addMessage("rightClick",index);
		}
		ctrlDown = false;
		gRenderer->forceUpdate();
		break;
	case WM_LBUTTONUP:
		leftButtonDown = false;
		gRenderer->setClipChunkPercent(previousPeel);
		gRenderer->forceUpdate();
		break;
	case WM_KEYDOWN:
		if(VK_LEFT==wParam)
		{
			gCameraDefaultMesh->move(Vec<float>(-0.05f, 0.0f, 0.0f));
			gRenderer->forceUpdate();
		}
		else if(VK_RIGHT==wParam)
		{
			gCameraDefaultMesh->move(Vec<float>(0.05f, 0.0f, 0.0f));
			gRenderer->forceUpdate();
		}
		else if(VK_UP==wParam)
		{
			gCameraDefaultMesh->move(Vec<float>(0.0f, -0.05f, 0.0f));
			gRenderer->forceUpdate();
		}
		else if(VK_DOWN==wParam)
		{
			gCameraDefaultMesh->move(Vec<float>(0.0f, 0.05f, 0.0f));
			gRenderer->forceUpdate();
		}
		else if(VK_PRIOR==wParam) //Page Up key
		{
			gRenderer->decrementFrame();
			gRenderer->forceUpdate();
		}
		else if(VK_NEXT==wParam) //Page Down key
		{
			gRenderer->incrementFrame();
			gRenderer->forceUpdate();
		}
		else if(VK_HOME==wParam)
		{
			gRenderer->setCurrentFrame(0);
			gRenderer->forceUpdate();
		}
		else if(VK_END==wParam)
		{
			gRenderer->setCurrentFrame(gRenderer->getNumberOfFrames() - 1);
			gRenderer->forceUpdate();
		}
		else if(VK_SPACE==wParam)
		{
			gPlay = !gPlay;
			gMsgQueueToMex.addMessage("play",gPlay);
		}
		else if(VK_SHIFT==wParam)
		{
			shiftDown = true;
			gMsgQueueToMex.addMessage("keyDown","shift");
		}
		else if(VK_CONTROL==wParam)
		{
			ctrlDown = true;
			gMsgQueueToMex.addMessage("keyDown","ctrl");
		}
		else if(VK_MENU == wParam)
		{
			altDown = true;
			gMsgQueueToMex.addMessage("keyDown","alt");
		}
		else if('B' == wParam)
		{
			if(ctrlDown)
				gRenderer->togglescaleText();
			else
				gRenderer->togglescaleBar();

			gRenderer->forceUpdate();
		}
		else if('C' == wParam)
		{
			gRenderer->FrontClipPos(-Renderer::cornerVolumeDist);
			gRenderer->BackClipPos(Renderer::cornerVolumeDist);
			gCameraDefaultMesh->resetCamera();
			gRenderer->forceUpdate();
		}
		else if('F' == wParam)
		{
			if(ctrlDown && !altDown)
				gRenderer->togglefps();
			else if(ctrlDown && altDown)
				gRenderer->togglestats();
			else
				gRenderer->toggleFrameNum();

			gRenderer->forceUpdate();
		}
		else if('H' == wParam)
		{
			hullsOn = !hullsOn;
			gMsgQueueToMex.addMessage("togglePolygons",double(hullsOn));

			for ( auto& it : gRenderer->allSceneObjects(GraphicObjectTypes::Polygons) )
			{
				it.second->setRenderable(hullsOn);
			}

			gRenderer->forceUpdate();
		}
		else if('L' == wParam)
		{
			gRenderer->toggleLabels();
			gMsgQueueToMex.addMessage("toggleLabels",0.0);
			gRenderer->forceUpdate();
		}
		else if('P' == wParam)
		{
			if(ctrlDown)
			{
				gRenderer->captureWindow(NULL);
				gRenderer->forceUpdate();
			}
		}
		else if('R' == wParam)
		{
			gRenderer->resetRootWorldTransform();
			gRenderer->forceUpdate();
		}
		else if('S' == wParam)
		{
			gRotate = !gRotate;
			gMsgQueueToMex.addMessage("rotate",gRotate);
		}
		else if('W' == wParam)
		{
			widgetOn = !widgetOn;
			setObjectTypeVisibility(GraphicObjectTypes::Widget, widgetOn);

			gRenderer->forceUpdate();
		}
		else if('X' == wParam)
		{
			gMsgQueueToMex.addMessage("centerSelectedPolygon",1.0);
			gRenderer->forceUpdate();
		}
		else if('1' == wParam || VK_NUMPAD1 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a 1080p movie
				ClientResize(gWindowHandle, 1920, 1080);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a 4k movie (UHD = 3840 x 2160)
				ClientResize(gWindowHandle, 3840, 2160);
			}
			{
				gMsgQueueToMex.addMessage("keyDown","number",1.0);
			}
		}
		else if('2' == wParam || VK_NUMPAD2 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a two panel 1080p movie
				ClientResize(gWindowHandle, 1920/2, 1080);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a two panel 4k movie (UHD = 3840 x 2160)
				ClientResize(gWindowHandle, 3840/2, 2160);
			}
			{
				gMsgQueueToMex.addMessage("keyDown","number",2.0);
			}
		}
		else if('3' == wParam || VK_NUMPAD3 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a three panel 1080p movie
				ClientResize(gWindowHandle, 1920/3, 1080);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a three panel 4k movie (UHD = 3840 x 2160)
				ClientResize(gWindowHandle, 3840/3, 2160);
			}
			{
				gMsgQueueToMex.addMessage("keyDown","number",3.0);
			}
		}
		else if('4' == wParam || VK_NUMPAD4 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a four panel 1080p movie
				ClientResize(gWindowHandle, 1920/2, 1080/2);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a four panel 4k movie (UHD = 3840 x 2160)
				ClientResize(gWindowHandle, 3840/2, 2160/2);
			}
			{
				gMsgQueueToMex.addMessage("keyDown","number",4.0);
			}
		}
		else if('5' == wParam || VK_NUMPAD5 == wParam)
		{
			gMsgQueueToMex.addMessage("keyDown","number",5.0);
		}
		else if('6' == wParam || VK_NUMPAD6 == wParam)
		{
			gMsgQueueToMex.addMessage("keyDown","number",6.0);
		}
		else if('7' == wParam || VK_NUMPAD7 == wParam)
		{
			gMsgQueueToMex.addMessage("keyDown","number",7.0);
		}
		else if('8' == wParam || VK_NUMPAD8 == wParam)
		{
			gMsgQueueToMex.addMessage("keyDown","number",8.0);
		}
		else if('9' == wParam || VK_NUMPAD9 == wParam)
		{
			gMsgQueueToMex.addMessage("keyDown","number",9.0);
		}
		else if('0' == wParam || VK_NUMPAD0 == wParam)
		{
			gMsgQueueToMex.addMessage("keyDown","number",0.0);
		}
		break;
	case WM_SYSKEYDOWN:
		if(VK_MENU==wParam)
		{
			altDown = true;
			gMsgQueueToMex.addMessage("keyDown", "alt");
		}
		break;
	case WM_KEYUP:
		if(VK_SHIFT==wParam)
		{
			shiftDown = false;
			gMsgQueueToMex.addMessage("keyUp","shift");
		}
		else if(VK_CONTROL==wParam)
		{
			ctrlDown = false;
			gMsgQueueToMex.addMessage("keyUp","ctrl");
		}
		else if(VK_MENU==wParam)
		{
			altDown = false;
			gMsgQueueToMex.addMessage("keyUp","alt");
		}
		break;
	case WM_SYSKEYUP:
		if(VK_MENU==wParam)
		{
			altDown = false;
			gMsgQueueToMex.addMessage("keyDown", "alt");
		}
		break;

	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}

void updateTime()
{
	static clock_t lastTimeUpdate = clock();
	static clock_t lastRotateUpdate = clock();
	static clock_t lastShaderUpdate = clock();

	if ( gPlay )
	{
		float timeFromLast = (float)(clock() - lastTimeUpdate) / CLOCKS_PER_SEC;
		if ( timeFromLast > 1.0f/gFramesPerSec )
		{
			lastTimeUpdate = clock();

			gRenderer->incrementFrame();

			gRenderer->forceUpdate();
		}
	}

	if ( gRotate )
	{
		float timeFromLast = (float)(clock() - lastRotateUpdate) / CLOCKS_PER_SEC;
		if ( timeFromLast > 2.5f/numAngles )
		{
			lastRotateUpdate = clock();

			DirectX::XMMATRIX previousWorldRotation = gRenderer->getRootWorldRotation();
			gRenderer->setWorldRotation(previousWorldRotation*ROT_X);

			gRenderer->forceUpdate();
		}
	}

	if ( gUpdateShaders )
	{
		float shaderUpdateElapsed = (float)(clock() - lastShaderUpdate) / CLOCKS_PER_SEC;
		if ( shaderUpdateElapsed > 1.0f )
		{
			lastShaderUpdate = clock();

			bool update = gRenderer->updateRegisteredShaders();
			if ( update )
				gRenderer->forceUpdate();
		}
	}
}

void updateCapture()
{
	static int curAngle = 0;

	if ( !gCapture )
		return;

	gRenderer->captureWindow(NULL);

	if ( !gRotate )
		return;

	++curAngle;
	if ( curAngle == numAngles )
	{
		gCapture = false;
		gRotate = false;
		curAngle = 0;
	}
}

void startThreadQueue()
{
	gMsgQueueToDirectX.open();
}

void stopThreadQueue()
{
	gMsgQueueToDirectX.close();
}

DWORD WINAPI messageLoop(LPVOID lpParam)
{
	std::string* rootDir = (std::string*)lpParam;
	HRESULT hr = E_FAIL;
	try
	{
		hr = windowInit(ModuleInfo::getInstance(),true,*rootDir);
		attachWidget(arrowFaces, numArrowFaces/3, arrowVerts, numArrowVerts/3, arrowNorms, numArrowNorms/3,
					sphereFaces, numSphereFaces/3, sphereVerts, numSphereVerts/3, sphereNorms, numSphereNorms/3);
	}
	catch(const std::exception& e)
	{
		sendErrMessage(e.what());
	}
	catch(const std::string& e)
	{
		sendErrMessage(e);
	}
	catch(...)
	{
		sendErrMessage("Caught an unknown error!");
	}

	delete rootDir;

	MSG msg ={0};

	if(hr==S_OK)
	{
		startThreadQueue();
		gRendererInit = true;

		DWORD termWait = WaitForSingleObject(gTermEvent,0);

		try
		{
			// DirectX thread
			while(termWait != WAIT_OBJECT_0 && SUCCEEDED(hr))
			{
				if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// Start tearing down the thread in the case of WM_QUIT
				if ( msg.message == WM_QUIT )
				{
					hr = S_FALSE;
					break;
				}

				// Check messages from the MEX thread
				hr = checkMessage();
				if ( FAILED(hr) )
					break;

				// Update any time-related parts of renderer
				updateTime();
				updateCapture();

				// This function checks if renderer needs update then calls renderall()
				gRenderer->renderUpdate();


				termWait = WaitForSingleObject(gTermEvent,0);
			}
		}
		catch(const std::exception& e)
		{
			sendErrMessage(e.what());
		}
		catch(const std::string& e)
		{
			sendErrMessage(e);
		}
		catch(...)
		{
			sendErrMessage("Caught an unknown error!");
		}

		gRenderer->setRendering(false);
		stopThreadQueue();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		// TODO: Just let this cleanup in the renderer destructor?
		if ( gRenderer != NULL )
		{
			for ( int i = 0; i < GraphicObjectTypes::NumGO; ++i )
			{
				gRenderer->removeSceneObjects((GraphicObjectTypes)i);
			}
		}
	}
	else
	{
		sendHrErrMessage(hr);
	}

	try
	{
		SAFE_DELETE(gRenderer);
		SAFE_DELETE(gCameraDefaultMesh);
		SAFE_DELETE(gCameraWidget);

		DestroyWindow(gWindowHandle);
		UnregisterClass(szWndClassName,ModuleInfo::getInstance());

		gRendererInit = false;

		gMsgQueueToMex.addMessage("loadDone", 0);
		gMsgQueueToMex.addMessage("close",1.0);
	}
	catch(const std::exception& e)
	{
		sendErrMessage(e.what());
	}
	catch(const std::string& e)
	{
		sendErrMessage(e);
	}
	catch(...)
	{
		sendErrMessage("Caught an unknown error!");
	}

	return ((int)msg.wParam);
}

HRESULT checkMessage()
{
	if ( gMsgQueueToDirectX.getNumMessages() == 0 )
		return S_OK;

	bool success = gMsgQueueToDirectX.processNext();

	return ((success) ? (S_OK) : (S_FALSE));
}
