#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "Image.h"
#include "TransferObj.h"
#include "TextureLightingObj.h"

HANDLE messageLoopHandle = NULL;
DWORD threadID;
HANDLE mexMessageMutex = NULL;
bool registerExitFunction = false;
DataQueue* dataQueue = new DataQueue();

void pollCommand(int nlhs, mxArray** plhs)
{
	if (nlhs != 1) mexErrMsgTxt("Wrong number of return arguments");

	std::vector<OldMessage> curMsgs = gMexMessageQueueOut.flushQueue();

	const char* fields[] = { "command", "message", "val" };
	plhs[0] = mxCreateStructMatrix(curMsgs.size(), 1, 3, fields);

	for (int i = 0; i < curMsgs.size(); ++i)
	{
		mxArray* cmd = mxCreateString(curMsgs[i].command.c_str());
		mxArray* msg = mxCreateString(curMsgs[i].message.c_str());
		mxArray* val = mxCreateDoubleScalar(curMsgs[i].val);
		mxSetField(plhs[0], i, fields[0], cmd);
		mxSetField(plhs[0], i, fields[1], msg);
		mxSetField(plhs[0], i, fields[2], val);
	}
}

void initCommand(int nrhs, const mxArray** prhs)
{
	// Not enough args
	if (nrhs < 7)
		mexErrMsgTxt("Not enough input arguments to initialize Lever 3-d.  Did you forget the widget?");

	if (!messageLoopHandle)
	{
		startThread();

		if (registerExitFunction)
		{
			mexAtExit(exitFunc);
			registerExitFunction = FALSE;
		}

		loadWidget(prhs + 1);
		gRendererOn = true;
	}
}

void closeCommand()
{
	dataQueue->writeMessage("close", NULL);
	cleanUp();
	gMexMessageQueueOut.clear();
}

void loadTextureCommand(const mxArray** prhs, int nrhs)
{
	size_t numDims = mxGetNumberOfDimensions(prhs[1]);
	if (numDims<3)
		mexErrMsgTxt("Image must have at least three dimensions!");

	const mwSize* DIMS = mxGetDimensions(prhs[1]);

	// Dimensions 1-5
	Vec<size_t> dims = Vec<size_t>(DIMS[0], DIMS[1], DIMS[2]);
	int numChannels = 1;
	int numFrames = 1;


	if (numDims>3)
		numChannels = int(DIMS[3]);

	if (numDims > 4)
		numFrames = int(DIMS[4]);

	// ptr to image data
	unsigned char* image = (unsigned char*)mxGetData(prhs[1]);

	/**/
	char buff[96];
	mxGetString(prhs[3],buff,96);

	Image* img = new Image(numChannels,numFrames,dims,buff);
	img->setPixels(image);

	if (nrhs > 2)
	{
		// Physical dimensions
		double* physDims = (double*)mxGetData(prhs[2]);
		img->setPhysicalDim(Vec<float>(physDims[0],physDims[1],physDims[2]));
	}

	if (nrhs > 3)
	{
		char buff[96];
		mxGetString(prhs[3], buff, 96);

		if (_strcmpi("original", buff) == 0)
			img->setNumArgs(0);
		else if (_strcmpi("processed", buff) == 0)
			img->setNumArgs(1);
	}
	std::string s = "loadTexture";
	dataQueue->writeMessage(s,(void*)img);
	int x = 0;
	x = x + 30;
	/*if (dataQueue->getNumMessages() > 0){
		Message m = dataQueue->getNextMessage();
		Image* returnedImg = (Image*)m.data;
		dims = returnedImg->getDimensions();
		image = returnedImg->getPixels();
		numChannels = returnedImg->getNumChannels();
		numFrames = returnedImg->getNumFrames();

		Vec<float> scale(dims);
		scale = scale / scale.maxValue();
		if (nrhs > 2)
		{
			// Physical dimensions
			double* physDims = (double*)mxGetData(prhs[2]);
			scale.y *= float(physDims[1] / physDims[0]);
			scale.z *= float(physDims[2] / physDims[0]);
		}

	}*/

	/**/

	/*GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;
	if (nrhs > 3)
	{
		char buff[96];
		mxGetString(prhs[3], buff, 96);

		if (_strcmpi("original", buff) == 0)
			textureType = GraphicObjectTypes::OriginalVolume;
		else if (_strcmpi("processed", buff) == 0)
			textureType = GraphicObjectTypes::ProcessedVolume;
	}

	if (gGraphicObjectNodes[GraphicObjectTypes::Border].empty())
	{
		HRESULT hr = createBorder(scale);
		if (FAILED(hr))
			mexErrMsgTxt("Could not create border!");
	}

	HRESULT hr = loadVolumeTexture(image, dims, numChannels, numFrames, scale, textureType);
	if (FAILED(hr))
		mexErrMsgTxt("Could not load texture!");

	setCurrentTexture(textureType);*/
}

void peelUpdateCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("not the right arguments for peelUpdate!");

	float x = (float)mxGetScalar(prhs[1]);
	float* ptr = &x;
	dataQueue->writeMessage("peelUpdate", (void*)ptr);
}

void textureLightingUpdateCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("not the right arguments for lightingUpdate!");

	if (mxGetScalar(prhs[1]) > 0.0)
	{
		for (int i = 0; i < firstVolumeTextures.size(); ++i)
		{
			if (NULL != firstVolumeTextures[i])
			{
				TextureLightingObj* textureLightObj = new TextureLightingObj(true, i);
				dataQueue->writeMessage("textureLightingUpdate", (void*)textureLightObj);
				//firstVolumeTextures[i]->setLightOn(true);
			}
		}
	}
	else
	{
		for (int i = 0; i < firstVolumeTextures.size(); ++i)
		{
			if (NULL != firstVolumeTextures[i])
			{
				TextureLightingObj* textureLightObj = new TextureLightingObj(false, i);
				dataQueue->writeMessage("textureLightingUpdate", (void*)textureLightObj);
				//firstVolumeTextures[i]->setLightOn(false);
			}
		}
	}
}

void textureAttenUpdateCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("not the right arguments for attenuationUpdate!");

	if (mxGetScalar(prhs[1]) > 0.0)
	{
		for (int i = 0; i < firstVolumeTextures.size(); ++i)
		{
			if (NULL != firstVolumeTextures[i])
			{
				//firstVolumeTextures[i]->setAttenuationOn(true);
				TextureLightingObj* textureLightObj = new TextureLightingObj(true, i);
				dataQueue->writeMessage("textureAttenUpdate", (void*)textureLightObj);
			}
		}
	}
	else
	{
		for (int i = 0; i < firstVolumeTextures.size(); ++i)
		{
			if (NULL != firstVolumeTextures[i])
			{
				//firstVolumeTextures[i]->setAttenuationOn(false);
				TextureLightingObj* textureLightObj = new TextureLightingObj(false, i);
				dataQueue->writeMessage("textureAttenUpdate", (void*)textureLightObj);
			}
		}
	}
}

void segmentationLightingCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for segmentationLighting!");

	double onD = mxGetScalar(prhs[1]);
	double* onD2 = new double;
	*onD2 = onD;
	dataQueue->writeMessage("showLabels", (void*)onD2);
}

void playCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for play!");

	double onD = mxGetScalar(prhs[1]);
	double* onD2 = new double;
	*onD2 = onD;
	dataQueue->writeMessage("play", (void*)onD2);
}

void rotateCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for rotate!");

	double onD = mxGetScalar(prhs[1]);
	double* onD2 = new double;
	*onD2 = onD;
	dataQueue->writeMessage("rotate", (void*)onD2);
}

void showLabelsCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for showLabels!");

	double onD = mxGetScalar(prhs[1]);
	double* onD2 = new double;
	*onD2 = onD;
	dataQueue->writeMessage("showLabels", (void*)onD2);
}

void resetViewCommand()
{
	std::string s = "resetView";
	dataQueue->writeMessage(s, NULL);
}

void captureSpinMovieCommand()
{
	dataQueue->writeMessage("captureSpinMovie", (void*)NULL);
}

void transferUpdateCommand(int nrhs, int nlhs, const mxArray** prhs)
{
	if (2 > nrhs || 3<nrhs) mexErrMsgTxt("This is not the right number of input arguments for transferUpdate!");

	char buff[96];
	if (nrhs > 2)
	{
		mxGetString(prhs[2], buff, 96);
	}

	size_t numElem = mxGetNumberOfElements(prhs[1]);

	for (int chan = 0; chan < numElem; ++chan)
	{
		// TODO Pull these out.. eventually 
		Vec<float> transferFunction(0.0f, 0.0f, 0.0f);
		Vec<float> ranges;
		Vec<float> color;
		float alphaMod;

		mxArray* mxColorPt = mxGetField(prhs[1], chan, "color");
		double* mxColor = (double*)mxGetData(mxColorPt);
		color = Vec<float>((float)(mxColor[0]), (float)(mxColor[1]), (float)(mxColor[2]));

		mxArray* mxAPt = mxGetField(prhs[1], chan, "a");
		mxArray* mxBPt = mxGetField(prhs[1], chan, "b");
		mxArray* mxCPt = mxGetField(prhs[1], chan, "c");
		double a = mxGetScalar(mxAPt);
		double b = mxGetScalar(mxBPt);
		double c = mxGetScalar(mxCPt);
		transferFunction = Vec<float>((float)a, (float)b, (float)c);

		mxArray* mxMin = mxGetField(prhs[1], chan, "minVal");
		mxArray* mxMax = mxGetField(prhs[1], chan, "maxVal");
		ranges = Vec<float>((float)mxGetScalar(mxMin), (float)mxGetScalar(mxMax), 1.0f);

		mxArray* mxAlphaPt = mxGetField(prhs[1], chan, "alphaMod");
		mxArray* mxOnPt = mxGetField(prhs[1], chan, "visible");
		if (mxGetScalar(mxOnPt) != 0)
			alphaMod = (float)mxGetScalar(mxAlphaPt);
		else
			alphaMod = 0.0f;

		TransferObj* transferObj = new TransferObj(transferFunction, ranges, color, alphaMod, chan, buff, numElem);
		dataQueue->writeMessage("transferUpdate", (void*)transferObj);
	}
}

void viewTextureCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for viewTexture!");

	char buff[96];
	mxGetString(prhs[1], buff, 96);



	GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;

	if (_strcmpi("original", buff) == 0)
		textureType = GraphicObjectTypes::OriginalVolume;
	else if (_strcmpi("processed", buff) == 0)
		textureType = GraphicObjectTypes::ProcessedVolume;

	setCurrentTexture(textureType);
}

void viewSegmentationCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for viewSegmentation!");

	double onD = mxGetScalar(prhs[1]);
	double* onD2 = new double;
	*onD2 = onD;
	dataQueue->writeMessage("viewSegmentation", (void*)onD2);
}

void wireframeSegmentationCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for wireframeSegmentation!");

	double onD = mxGetScalar(prhs[1]);
	double* onD2 = new double;
	*onD2 = onD;
	dataQueue->writeMessage("wireframeSegmentation", (void*)onD2);
}

void loadHullsCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for loadHulls!");

	const mxArray* hulls = prhs[1];
	if (hulls == NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

	HRESULT hr = loadHulls(hulls);
	if (FAILED(hr))
		mexErrMsgTxt("Could not load hulls!");
}

void removeHullCommand(int nrhs, const mxArray** prhs){
	double d = mxGetScalar(prhs[1]);
	int* label = new int;
	*label = d;
	dataQueue->writeMessage("removeHull", (void*)label);
}

void displayHullsCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for displayHulls!");

	double* hullList = (double*)mxGetData(prhs[1]);
	size_t numHulls = mxGetNumberOfElements(prhs[1]);

	std::set<int>* hullset = new std::set<int>;
	for (size_t i = 0; i < numHulls; ++i)
		hullset->insert((int)(hullList[i]));

	dataQueue->writeMessage("displayHulls", (void*)hullset);
}

void setFrameCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for setFrame!");

	int curFrame = (int)mxGetScalar(prhs[1]);
	int* onD2 = new int;
	*onD2 = curFrame;
	dataQueue->writeMessage("setFrame", (void*)onD2);
}

void setViewOriginCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for setViewOrigin!");

	double* origin = (double*)mxGetData(prhs[1]);
	size_t numDims = mxGetNumberOfElements(prhs[1]);

	if (numDims != 3) mexErrMsgTxt("There needs to be three doubles for the view origin!");

	double* originMsg = new double[3];
	originMsg[0] = origin[0];
	originMsg[1] = origin[1];
	originMsg[2] = origin[2];

	dataQueue->writeMessage("setViewOrigin", (void*)originMsg);
}

void updateHullsCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for loadHulls!");

	const mxArray* hulls = prhs[1];
	if (hulls == NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

	HRESULT hr = updateHulls(hulls);
	if (FAILED(hr))
		mexErrMsgTxt("Could not update hulls!");
}

void addHullsCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 2) mexErrMsgTxt("Not the right arguments for loadHulls!");

	const mxArray* hulls = prhs[1];
	if (hulls == NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

	HRESULT hr = addHulls(hulls);
	if (FAILED(hr))
		mexErrMsgTxt("Could not add hulls!");
}

void setCapturePathCommand(int nrhs, const mxArray** prhs)
{
	if (nrhs != 3) mexErrMsgTxt("Not the right arguments for setCapturePath!");

	char filePath[512];
	char fileName[255];
	mxGetString(prhs[1], filePath, 512);
	mxGetString(prhs[2], fileName, 255);

	gRenderer->setCaptureFilePath(filePath);
	gRenderer->setCaptureFileName(fileName);
}

void takeControlCommand()
{
	//gRenderer->getMutex();
	gRendererOn = false;
	//gRenderer->releaseMutex();
}

void releaseControlCommand()
{
	//gRenderer->getMutex();
	gRendererOn = true;
	//gRenderer->releaseMutex();
}

void captureImageCommand(int nlhs, int nrhs, const mxArray** prhs, mxArray** plhs)
{
	if (gRendererOn == true) mexErrMsgTxt("MATLAB does not have exclusive control over the renderer! Call takeControl before using this command");

	//if (nrhs != 3) mexErrMsgTxt("Usage is lever_3d('captureImage',folderPath,fileNamePrefix); ");
	if (nlhs != 1) mexErrMsgTxt("There must be one output argument to hold the file path/name that was captured!");

	std::string fileNameOut;
	HRESULT hr;

	if (nrhs > 2)
	{
		char dirBuff[512];
		char filePreBuff[256];

		mxGetString(prhs[1], dirBuff, 512);
		mxGetString(prhs[2], dirBuff, 256);

		hr = gRenderer->captureWindow(dirBuff, filePreBuff, fileNameOut);
	}
	else
	{
		hr = gRenderer->captureWindow(&fileNameOut);
	}

	if (FAILED(hr)) mexErrMsgTxt("Unable to capture the screen!");

	plhs[0] = mxCreateString(fileNameOut.c_str());
}

extern "C" void exitFunc()
{
	cleanUp();
}