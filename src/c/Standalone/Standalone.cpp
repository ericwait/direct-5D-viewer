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
	const int numChan = 2;
	const Vec<size_t> dims(128,128,50);

	unsigned char* pixelVals = createRandomVolume(numChan, dims);

	MessageLoadTexture* loadMsg = new MessageLoadTexture(numChan, 1, dims, pixelVals);
	loadMsg->setTextureType(GraphicObjectTypes::OriginalVolume);
	loadMsg->setPhysSize(Vec<float>(1.0f, 1.0f, 1.0f)*Vec<float>(dims));

	gMsgQueueToDirectX.pushMessage(loadMsg);
	gMsgQueueToDirectX.pushMessage(new MessageShowObjectType(GraphicObjectTypes::OriginalVolume,true));

	SetProcessDPIAware();
	messageLoop(pRootDir);

	// This is sort of a leak, but we can't delete the image data until application close.
	delete[] pixelVals;

	return 0;
}
