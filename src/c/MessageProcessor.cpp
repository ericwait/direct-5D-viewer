#include "MessageProcessor.h"
#include "Globals.h"
#include "Initialization.h"

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

HRESULT messageProcess( MSG& msg ) 
{
 	HRESULT hr = S_OK;

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

	DestroyWindow(gWindowHandle);
	UnregisterClass(szWndClassName, gDllInstance);

	//TODO cleanUp();

	return ((int)msg.wParam);
}