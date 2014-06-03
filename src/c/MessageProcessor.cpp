#include "Globals.h"
#include "MessageProcessor.h"
#include "Initialization.h"

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message==WM_DESTROY)
		PostQuitMessage(0);

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

	SAFE_DELETE(gRootSceneNode);//TODO make sure this cleans up all the children too
	SAFE_DELETE(gRenderer);

	DestroyWindow(gWindowHandle);
	UnregisterClass(szWndClassName, gDllInstance);

	//TODO cleanUp();

	return ((int)msg.wParam);
}