#include "RenderInit.h"
#include "D3d/Renderer.h"
#include "Global/Globals.h"

#include <Windows.h>

long createRenderResources(std::string rootDir, Vec<int> viewportSize, void* hwnd)
{
	Vec<float> eye = Vec<float>(0.0f,0.0f,-3.0f);
	Vec<float> look = Vec<float>(0.0f,0.0f,0.0f);
	Vec<float> up = Vec<float>(0.0f,1.0f,0.0);
	
	gRenderer = new Renderer();//delete on message loop exit
	HRESULT hr = gRenderer->init(rootDir, viewportSize, (HWND)hwnd);

	if (FAILED(hr))
		return hr;

	gCameraDefaultMesh = new Camera(eye,look,up,viewportSize);//delete on message loop exit
	gCameraWidget = new OrthoCamera(eye,look,up,viewportSize);//delete on message loop exit
	gCameraText = new TextCamera(eye,look,up,viewportSize);

	return S_OK;
}
