#include "Globals.h"
#include "MessageProcessor.h"
#include "Initialization.h"
#include <time.h>

bool gRendererOn = false;
bool gPlay = false;
float gFramesPerSec = 5;

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message==WM_DESTROY)
		PostQuitMessage(0);

	// Current mouse position
	int iMouseX = (short)LOWORD( lParam );
	int iMouseY = (short)HIWORD( lParam );
	HMENU hmenu = GetMenu(hWnd);

	static bool leftButtonDown = false;
	static bool shiftDown = false;
	static bool ctrlDown = false;
	static bool altDown = false;
	static int previousMouseX = 0;
	static int previousMouseY = 0;
	static DirectX::XMMATRIX previousWorldRotation;

	int label;
	Vec<float> pnt;
	Vec<float> direction;

	switch (message)
	{
	case WM_NULL:
		break;
	case WM_CREATE:
		break;
	case WM_MOVE:
		break;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			gRendererOn = false;
			break;
		}else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			gRendererOn = true;

		gWindowWidth = LOWORD( lParam );
		gWindowHeight = HIWORD( lParam );
		if (gRenderer!=NULL)
			gRenderer->resizeViewPort();
		break;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam)>0)
			gCameraDefaultMesh->zoomIncrement();
		else
			gCameraDefaultMesh->zoomDecrement();
		break;
	case WM_MOUSEMOVE:
		if (leftButtonDown)
		{
			DirectX::XMMATRIX rotX,rotY;
			rotX=DirectX::XMMatrixRotationY(((float)(iMouseX-previousMouseX)/gWindowWidth)*DirectX::XM_2PI);
			rotY=DirectX::XMMatrixRotationX((-(float)(iMouseY-previousMouseY)/gWindowHeight)*DirectX::XM_2PI);

			gRenderer->setWorldRotation(previousWorldRotation*rotX*rotY);
		}
		break;
	case WM_LBUTTONDOWN:
		leftButtonDown = true;
		previousMouseX = iMouseX;
		previousMouseY = iMouseY;
		previousWorldRotation = gRenderer->getRootWorldRotation();
		break;
	case WM_RBUTTONDOWN:
		gCameraDefaultMesh->getRay(iMouseX,iMouseY,pnt,direction);
		label = gRenderer->getHull(pnt,direction);
		gMexMessageQueueOut.addMessage("cellSelected",label);
		break;
	case WM_LBUTTONUP:
		leftButtonDown = false;
		break;
	case WM_KEYDOWN:
		if (VK_LEFT==wParam)
			gCameraDefaultMesh->moveLeft();
		else if (VK_RIGHT==wParam)
			gCameraDefaultMesh->moveRight();
		else if (VK_UP==wParam)
			gCameraDefaultMesh->moveUp();
		else if (VK_DOWN==wParam)
			gCameraDefaultMesh->moveDown();
		else if (VK_PRIOR==wParam) //Page Up key
			gRenderer->incrementFrame();
		else if (VK_NEXT==wParam) //Page Down key
			gRenderer->decrementFrame();
		else if (VK_HOME==wParam)
			gRenderer->setCurrentFrame(0);
		else if (VK_END==wParam)
			gRenderer->setCurrentFrame(gRenderer->getNumberOfFrames()-1);
		else if (VK_SPACE==wParam)
			gPlay = !gPlay;
		else if (VK_SHIFT==wParam)
			shiftDown = true;
		else if (VK_CONTROL==wParam)
			ctrlDown = true;
		else if (VK_MENU==wParam)
			altDown = true;
		else if ('C'==wParam)
			gCameraDefaultMesh->resetCamera();
		else if ('R'==wParam)
			gRenderer->resetRootWorldTransform();
		else if ('X'==wParam)
		{
			gMexMessageQueueOut.addMessage("centerSelectedCell",1.0);
		}
		break;
	case WM_KEYUP:
		if (VK_SHIFT==wParam)
			shiftDown = false;
		else if (VK_CONTROL==wParam)
			ctrlDown = false;
		else if (VK_MENU)
			altDown = false;
		break;
	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}

HRESULT messageProcess( MSG& msg ) 
{
 	HRESULT hr = S_OK;
	static clock_t lastTimeupdate = clock();

	if (msg.message==WM_QUIT)
		return S_FALSE;

	if (gRendererOn)
	{
		if (gPlay)
		{
			float timeFromLast = (float)(clock() - lastTimeupdate) / CLOCKS_PER_SEC;
			if (timeFromLast > 1.0f/gFramesPerSec)
			{
				lastTimeupdate = clock();
				gRenderer->incrementFrame();
			}
		}

		gRenderer->renderAll();
	}

	return hr;
}

DWORD WINAPI messageLoop(LPVOID lpParam)
{
	HRESULT hr;
	try 
	{
		hr = windowInit(gDllInstance,true);
	}
	catch (const std::exception& e)
	{
		gMexMessageQueueOut.addErrorMessage(e.what());
	}
	catch (const std::string& e)
	{
		gMexMessageQueueOut.addErrorMessage(e);
	}
	catch (...)
	{
		gMexMessageQueueOut.addErrorMessage("Caught an unknown error!");	
	}
	
	MSG msg = {0};

	if (hr==S_OK)
	{
		gRendererInit = true;

		DWORD termWait = WaitForSingleObject(gTermEvent, 0);

		try
		{
			while ( termWait != WAIT_OBJECT_0 && hr==S_OK)
			{
				if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				hr = messageProcess(msg);

				termWait = WaitForSingleObject(gTermEvent, 0);
			}
		}
		catch (const std::exception& e)
		{
			gMexMessageQueueOut.addErrorMessage(e.what());
		}
		catch (const std::string& e)
		{
			gMexMessageQueueOut.addErrorMessage(e);
		}
		catch (...)
		{
			gMexMessageQueueOut.addErrorMessage("Caught an unknown error!");	
		}
	}
	else
	{
		gMexMessageQueueOut.addErrorMessage(hr);
	}

	try
	{
		SAFE_DELETE(gRenderer);
		SAFE_DELETE(gCameraDefaultMesh);
		SAFE_DELETE(gCameraWidget);

		DestroyWindow(gWindowHandle);
		UnregisterClass(szWndClassName, gDllInstance);

		gRendererInit = true;

		gMexMessageQueueOut.addMessage("close",1.0);
	}
	catch (const std::exception& e)
	{
		gMexMessageQueueOut.addErrorMessage(e.what());
	}
	catch (const std::string& e)
	{
		gMexMessageQueueOut.addErrorMessage(e);
	}
	catch (...)
	{
		gMexMessageQueueOut.addErrorMessage("Caught an unknown error!");	
	}

	return ((int)msg.wParam);
}