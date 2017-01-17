#include "Global/Globals.h"
#include "Global/ModuleInfo.h"
#include "D3d/MessageProcessor.h"
#include "Messages/Image.h"

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

	std::string* pRootDir = new std::string("./");

	gRendererOn = true;

	// Queue up a random volume to render
	const int numChan = 2;
	const Vec<size_t> dims(128,128,50);

	unsigned char* pixelVals = createRandomVolume(numChan, dims);

	Image* testIm = new Image(numChan, 1, dims);
	testIm->setPixels(pixelVals);
	testIm->setTextureType(GraphicObjectTypes::OriginalVolume);
	testIm->setPhysicalDim(Vec<float>(1.0f,1.0f,1.0f));

	std::string s = "loadTexture";
	gMsgQueueToDirectX.writeMessage(s, (void*)testIm);

	GraphicObjectTypes* textureType = new GraphicObjectTypes(GraphicObjectTypes::OriginalVolume);
	gMsgQueueToDirectX.writeMessage("ViewTexture", (void*)textureType);

	SetProcessDPIAware();
	messageLoop(pRootDir);

	// This is sort of a leak, but we can't delete the image data until application close.
	delete[] pixelVals;

	return 0;
}
