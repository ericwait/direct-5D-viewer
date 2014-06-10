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
			// TODO print out error to screen and/or log

			return E_FAIL;
		}
	}

	// Create window
	RECT rc = { 0, 0, gWindowWidth, gWindowHeight};
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	gWindowHandle = CreateWindow( szWndClassName, "LEVer 3D viewer", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL );
	if( !gWindowHandle )
	{
		DWORD errCode = GetLastError();
		// TODO error
		return E_FAIL;
	}
	//SetCapture( gWindowHandle );

	ShowWindow( gWindowHandle, nCmdShow );
	UpdateWindow(gWindowHandle);

	return S_OK;
}

HRESULT createRenderResources()
{
	Vec<float> eye = Vec<float>(0.0f,0.0f,-5.0f);
	Vec<float> look = Vec<float>(0.0f,0.0f,0.0f);
	Vec<float> up = Vec<float>(0.0f,1.0f,0.0);
	
	gRenderer = new Renderer();//delete on message loop exit
	gRenderer->init();

	gCameraDefaultMesh = new Camera(eye,look,up);//delete on message loop exit
	gCameraWidget = new OrthoCamera(eye,look,up);//delete on message loop exit

	return S_OK;
}

HRESULT windowInit(HINSTANCE hInstance, int nCmdShow)
{
	if ( FAILED(registerWindowClass(hInstance,nCmdShow)) )
		return E_FAIL;

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

	return createRenderResources();
}