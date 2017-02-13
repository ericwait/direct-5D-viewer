#include "Global/Globals.h"
#include "Global/ModuleInfo.h"
#include "D3d/MessageProcessor.h"
#include "Messages/LoadMessages.h"
#include "Messages/ViewMessages.h"

#include <random>

unsigned char* createRandomVolume(int numChannels, Vec<size_t> dims)
{
	std::mt19937 mtRNG;
	std::uniform_int_distribution<int> unifDist(0,255);

	unsigned char* pixelVals = new unsigned char[numChannels*dims.product()];
	for ( int c = 0; c < numChannels; ++c )
	{
		for ( int i=0; i < dims.product(); ++i )
		{
			pixelVals[i + c*dims.product()] = unifDist(mtRNG);
		}
	}

	return pixelVals;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ModuleInfo::setModuleHandle(hInstance);
	ModuleInfo::initModuleInfo();

	std::string* pRootDir = new std::string(".");

	gUpdateShaders = true;

	// Queue up a random volume to render
	const bool columnMajor = false;
	const int numChan = 2;
	const Vec<size_t> dims(128,128,50);
	const Vec<float> physSize = 1.0f * dims;

	unsigned char* pixelVals = createRandomVolume(numChan, dims);

	// Setup demo noise volume
	gMsgQueueToDirectX.pushMessage(new MessageInitVolume(1, numChan, dims, physSize, columnMajor));
	gMsgQueueToDirectX.pushMessage(new MessageLoadTexture(GraphicObjectTypes::OriginalVolume, pixelVals));
	gMsgQueueToDirectX.pushMessage(new MessageUpdateRender());

	SetProcessDPIAware();
	messageLoop(pRootDir);

	// This is sort of a leak, but we can't delete the image data until application close.
	delete[] pixelVals;

	return 0;
}
