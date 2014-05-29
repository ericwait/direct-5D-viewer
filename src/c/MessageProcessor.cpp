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
// 	//char buffer[255];
// 	std::string metadataFile = "";
// 

// 
// 	switch (g_state)
// 	{
// 	case WAITING:
// 		break;
// 	case OPENING:
// 		resetGlobals();
// 		while (metadataFile.compare("")==0)
// 		{
// 			HRESULT hr = openMetaFileDialog(metadataFile);
// 		}
// 
// 		if (metadataFile.compare("")==0)
// 		{
// 			return S_FALSE;
// 		}
// 
// 		gImagesTiff = new ImagesTiff(metadataFile);
// 		g_state = INIT_DX;
// 
// 		//++g_state;
// 
// 		break;
// 	case REFRESH:
// 		refresh();
// 		g_state = RENDERING;
// 		break;
// 	case INIT_DX:
// 		hr = initDX();
// 		if (FAILED(hr))
// 			break;
// 
// 		//StartHand();
// 		LeapStartHand();
// 		StartArduinoComm();
// 
// 		loadCMesh(&g_coordMesh, TEXT("widgetMesh.msh"));
// 		ShowCursor(g_LEAPmode);
// 
// 		++g_state;
// 		break;
// 	case RENDERING:
// 	default:
// 		clock_t intervalRot = clock()-g_lastTimeRot;
// 		clock_t intervalPlay = clock()-g_lastTimePlay;
// 		float playInt = (gSpinFrame) ? (gImagesTiff->getNumberOfFrames()/300.0f) : (0.20f);
// 
// 		if (gPlayMovie && (playInt<(float)intervalPlay/CLOCKS_PER_SEC))
// 		{
// 			updateTime(1);
// 			g_lastTimePlay = clock();
// 		}
// 
// 		if(g_bRender)
// 			Render();
// 
// 		if (gSpinFrame && (0.01<(float)intervalRot/CLOCKS_PER_SEC))
// 		{
// 			g_worldRotMatrix = g_worldRotMatrix*XMMatrixRotationY(-0.0209f);
// 			updateTransforms();
// 			++g_rotCount;
// 			g_lastTimeRot = clock();
// 		}
// 
// 		if (gCaptureImages)
// 		{
// 			if (gPlayMovie && g_nTime==gImagesTiff->getNumberOfFrames()-1  && !gSpinFrame)
// 			{
// 				gCaptureImages = false;
// 				g_rotCount = 0;
// 				gSpinFrame = false;
// 				gPlayMovie = false;
// 			}
// 
// 			if (gSpinFrame && g_rotCount>300)
// 			{
// 				g_rotCount = 0;
// 				gCaptureImages = false;
// 				gSpinFrame = false;
// 				g_pointerOn = true;
// 				g_fpsShow = true;
// 
// 				if (g_rotCount==0 && gCaptureImages)
// 				{
// 					resetViewCenter();
// 					resetViewScale();
// 					resetWorldRotation();
// 				}
// 			}
// 
// 			FrameRender();
// 		}
// 
// 		break;
// 	}

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

	//teardownDX();

	DestroyWindow(gWindowHandle);
	UnregisterClass(szWndClassName, gDllInstance);

	//cleanUp();

	return ((int)msg.wParam);
}