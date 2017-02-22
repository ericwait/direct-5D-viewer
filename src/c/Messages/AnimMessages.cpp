#include "AnimMessages.h"


bool MessageSetMovieFrame::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->setCurrentFrame(frame);

	return true;
}



bool MessageSetCapturePath::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->setCaptureFilePath(path);
	gRenderer->setCaptureFileName(filename);

	return true;
}



bool MessageCaptureWindow::process()
{
	if ( !gRenderer )
		return false;

	if ( outData != NULL )
	{
		DWORD bmpDataSize = 0;
		BITMAPINFOHEADER bmpInfo;

		//outData->data = gRenderer->captureWindow(bmpDataSize, bmpInfo);

		//outData->width = bmpInfo.biWidth;
		//outData->height = bmpInfo.biHeight;

		Vec<size_t> dims;
		outData->data = gRenderer->captureWindow(dims);

		outData->width = dims.x;
		outData->height = dims.y;
	}
	else
		gRenderer->captureWindow(NULL);

	return true;
}



bool MessageCaptureSpinMovie::process()
{
	if ( !gRenderer )
		return false;

	gRenderer->resetRootWorldTransform();
	//gCameraDefaultMesh->resetCamera();

	gCapture = true;
	gRotate = true;

	return true;
}
