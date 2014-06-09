#include "Globals.h"
#include "MessageProcessor.h"
#include "Initialization.h"

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message==WM_DESTROY)
		PostQuitMessage(0);

	// Current mouse position
	int iMouseX = (short)LOWORD( lParam );
	int iMouseY = (short)HIWORD( lParam );
	HMENU hmenu = GetMenu(hWnd);

	static bool leftButtonDown = false;
	static int previousMouseX = 0;
	static int previousMouseY = 0;
	static DirectX::XMMATRIX previousWorldRotation;

	switch (message)
	{
	case WM_NULL:
		break;
	case WM_CREATE:
		break;
	case WM_MOVE:
		break;
	case WM_SIZE:
		gWindowWidth = LOWORD( lParam );
		gWindowHeight = HIWORD( lParam );
		if (gRenderer!=NULL)
			gRenderer->resizeViewPort();

		break;
	case WM_MOUSEMOVE:
		if (leftButtonDown)
		{
			DirectX::XMMATRIX rotX,rotY;
			rotX=DirectX::XMMatrixRotationY(((float)(iMouseX-previousMouseX)/gWindowWidth)*DirectX::XM_2PI);
			rotY=DirectX::XMMatrixRotationX((-(float)(iMouseY-previousMouseY)/gWindowHeight)*DirectX::XM_2PI);

			gRenderer->setRootWorldTransform(previousWorldRotation*rotX*rotY);
		}
		break;
	case WM_LBUTTONDOWN:
		leftButtonDown = true;
		previousMouseX = iMouseX;
		previousMouseY = iMouseY;
		previousWorldRotation = gRenderer->getRootWorldTransorm();
		break;
	case WM_LBUTTONUP:
		leftButtonDown = false;
		break;
	case WM_KEYDOWN:
		if (VK_PRIOR==wParam) //Page Up key
			gRenderer->incrementFrame();
		if (VK_NEXT==wParam) //Page Down key
			gRenderer->decrementFrame();
		if (VK_HOME==wParam)
			gRenderer->setCurrentFrame(0);
		if (VK_END==wParam)
			gRenderer->setCurrentFrame(gRenderer->getLastFrame());
		break;
	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}

HRESULT messageProcess( MSG& msg ) 
{
 	HRESULT hr = S_OK;

	if (msg.message==WM_QUIT)
		return S_FALSE;

	gRenderer->renderAll();

	return hr;
}

DWORD WINAPI messageLoop(LPVOID lpParam)
{
	HRESULT hr = windowInit(gDllInstance,true);
	
	MSG msg = {0};

	if (hr==S_OK)
	{
		gRendererInit = true;

		DWORD termWait = WaitForSingleObject(gTermEvent, 0);

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

	SAFE_DELETE(gRenderer);

	DestroyWindow(gWindowHandle);
	UnregisterClass(szWndClassName, gDllInstance);

	//TODO cleanUp();

	gRendererInit = true;

	return ((int)msg.wParam);
}