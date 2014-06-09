#include "Vec.h"
#include "mex.h"
#include "windows.h"
#include "MessageProcessor.h"
#include "Globals.h"

HANDLE gTermEvent = NULL;
volatile bool gRendererInit = false;
HANDLE messageLoopHandle = NULL;

HINSTANCE gDllInstance = NULL;
DWORD threadID;

std::vector<CellHullObject*> cellHullObjects;
std::vector<VolumeTextureObject*> volumeTextureObjects;

bool registerExitFunction = false;

bool checkRenderThread()
{
	DWORD waitTerm = WaitForSingleObject(messageLoopHandle, 0);
	if (waitTerm == WAIT_OBJECT_0)
	{
		CloseHandle(messageLoopHandle);
		CloseHandle(gTermEvent);
		messageLoopHandle = NULL;
		gTermEvent = NULL;
		gRendererInit = false;

		return false;
	}

	return true;
}


void startThread()
{
	if (gTermEvent==NULL)
	{
		gTermEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if ( !gTermEvent )
			mexErrMsgTxt("Could not create thread-monitor event");
	}

	if (messageLoopHandle==NULL)
	{
		messageLoopHandle = CreateThread(NULL, 0, messageLoop, (LPVOID)(NULL), 0, &threadID);
		if ( !messageLoopHandle )
		{
			mexErrMsgTxt("Could not create thread");
		}

		while (gRendererInit==false)
			Sleep(1000);

		if (!checkRenderThread())
			mexErrMsgTxt("Failed to initialized the renderer thread!");
	}
}

void termThread()
{
	if (gTermEvent!=NULL)
	{
		// Send thread a termination event
		SetEvent(gTermEvent);
		PostThreadMessage(threadID, WM_QUIT, (WPARAM)0, (LPARAM)0);
	}

	if (messageLoopHandle!=NULL)
	{
		// Resume the thread in case it's suspended
		ResumeThread(messageLoopHandle);

		// Wait for thread termination/force termination if it times out
		DWORD waitTerm = WaitForSingleObject(messageLoopHandle, 30000);
		if ( waitTerm == WAIT_TIMEOUT )
			TerminateThread(messageLoopHandle, -1000);

		CloseHandle(messageLoopHandle);
	}
	if (gTermEvent!=NULL)
		CloseHandle(gTermEvent);

	messageLoopHandle = NULL;
	gTermEvent = NULL;
	gRendererInit = false;
}

void cleanUp()
{
	for (int i=0; i<cellHullObjects.size(); ++i)
		delete cellHullObjects[i];
	cellHullObjects.clear();

	for (int i=0; i<volumeTextureObjects.size(); ++ i)
		delete volumeTextureObjects[i];
	volumeTextureObjects.clear();

	termThread();
}


BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if ( fdwReason == DLL_PROCESS_ATTACH )
	{
		gDllInstance = hInstDLL;
		registerExitFunction = TRUE;
	}

	return TRUE;
}

extern "C" void exitFunc()
{
	cleanUp();
}

void loadHulls( const mxArray* verts, const mxArray* faces, const mxArray* normals) 
{
// 
// 	size_t numVerts = mxGetM(verts);
// 	size_t numFaces = mxGetM(faces);
// 	size_t numNormals = mxGetM(normals);
// 
// 	if (numVerts<1)
// 		mexErrMsgTxt("No Verts!");
// 
// 	if (numFaces<1)
// 		mexErrMsgTxt("No faces!");
// 
// 	if (numNormals<1)
// 		mexErrMsgTxt("No norms!");
// 
// 	if (numVerts!=numNormals) mexErrMsgTxt("Number of normals does not match the number of verts!\n");
// 	if (numVerts<=0)
// 		return;
// 
// 	gVertsDebug.resize(numVerts);
// 	gFacesDebug.resize(numFaces);
// 	gNormsDebug.resize(numNormals);
// 
// 	double* vertData = (double*)mxGetData(verts);
// 	double* normData = (double*)mxGetData(normals);
// 	Vec<float> curVert, curNormal;
// 	for (int i=0; i<numVerts; ++i)
// 	{
// 		curVert.x = vertData[i];
// 		curVert.y = vertData[i+numVerts];
// 		curVert.z = vertData[i+2*numVerts];
// 
// 		curNormal.x = normData[i];
// 		curNormal.y = normData[i+numVerts];
// 		curNormal.z = normData[i+2*numVerts];
// 
// 		gVertsDebug[i] = curVert;
// 		gNormsDebug[i] = curNormal;
// 	}
// 
// 	double* faceData = (double*)mxGetData(faces);
// 	Vec<unsigned int> curFace;
// 	for (int i=0; i<numFaces; ++i)
// 	{
// 		curFace.x = faceData[i];
// 		curFace.y = faceData[i+numFaces];
// 		curFace.z = faceData[i+2*numFaces];
// 
// 		curFace = curFace - 1;
// 
// 		gFacesDebug[i] = curFace;
// 	}
}

CellHullObject* createCellHullObject(const mxArray** widget, Camera* camera)
{
	size_t numFaces = mxGetM(widget[0]);
	size_t numVerts = mxGetM(widget[1]);
	size_t numNormals = mxGetM(widget[2]);

	if (numVerts<1)
		mexErrMsgTxt("No Verts!");

	if (numFaces<1)
		mexErrMsgTxt("No faces!");

	if (numNormals<1)
		mexErrMsgTxt("No norms!");

	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> verts;
	std::vector<Vec<float>> normals;

	faces.resize(numFaces);
	verts.resize(numVerts);
	normals.resize(numNormals);

	double* faceData = (double*)mxGetData(widget[0]);
	Vec<unsigned int> curFace;
	for (int i=0; i<numFaces; ++i)
	{
		curFace.x = faceData[i];
		curFace.y = faceData[i+numFaces];
		curFace.z = faceData[i+2*numFaces];

		curFace = curFace - 1;

		faces[i] = curFace;
	}

	double* vertData = (double*)mxGetData(widget[1]);
	double* normData = (double*)mxGetData(widget[2]);
	Vec<float> curVert, curNormal;
	for (int i=0; i<numVerts; ++i)
	{
		curVert.x = vertData[i];
		curVert.y = vertData[i+numVerts];
		curVert.z = vertData[i+2*numVerts];

		curNormal.x = normData[i];
		curNormal.y = normData[i+numVerts];
		curNormal.z = normData[i+2*numVerts];

		verts[i] = curVert;
		normals[i] = curNormal;
	}

	CellHullObject* cho = new CellHullObject(gRenderer,faces,verts,normals,camera);
	cellHullObjects.push_back(cho);

	return cho;
}

void loadWidget(const mxArray* widget[])
{
	CellHullObject* arrowX = createCellHullObject(widget,gCameraWidget);
	arrowX->setColor(Vec<float>(1.0f, 0.2f, 0.2f),1.0f);
	GraphicObjectNode* arrowXnode = new GraphicObjectNode(arrowX);
	arrowXnode->setLocalToParent(DirectX::XMMatrixRotationY(DirectX::XM_PI/2.0f));

	CellHullObject* arrowY = createCellHullObject(widget,gCameraWidget);
	arrowY->setColor(Vec<float>(0.1f, 1.0f, 0.1f),1.0f);
	GraphicObjectNode* arrowYnode = new GraphicObjectNode(arrowY);
	arrowYnode->setLocalToParent(DirectX::XMMatrixRotationX(-DirectX::XM_PI/2.0f));

	CellHullObject* arrowZ = createCellHullObject(widget,gCameraWidget);
	arrowZ->setColor(Vec<float>(0.4f, 0.4f, 1.0f),1.0f);
	GraphicObjectNode* arrowZnode = new GraphicObjectNode(arrowZ);

	CellHullObject* sphere = createCellHullObject(widget+3,gCameraWidget);
	sphere->setColor(Vec<float>(0.9f,0.9f,0.9f),1.0f);
	GraphicObjectNode* sphereNode = new GraphicObjectNode(sphere);

	SceneNode* widgetScene = new SceneNode();
	arrowXnode->attachToParentNode(widgetScene);
	arrowYnode->attachToParentNode(widgetScene);
	arrowZnode->attachToParentNode(widgetScene);
	sphereNode->attachToParentNode(widgetScene);

	gRenderer->attachToRootScene(widgetScene,Renderer::Section::Post,0);
}

void loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scale)
{ 
	//TODO keep objects around somewhere for better cleanup
	unsigned char* shaderConstMemory = NULL;

	for (int i=0; i<numFrames; ++i)
	{
		VolumeTextureObject* volumeTexture = new VolumeTextureObject(gRenderer,dims,numChannel,image+i*numChannel*dims.product(),scale,
			gCameraDefaultMesh,shaderConstMemory);
		volumeTextureObjects.push_back(volumeTexture);

		shaderConstMemory = volumeTexture->getShaderConstMemory();
		GraphicObjectNode* volumeTextureNode = new GraphicObjectNode(volumeTexture);

		gRenderer->attachToRootScene(volumeTextureNode,Renderer::Section::Main,i);
	}

	std::vector<Vec<float>> vertices;
	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> normals;

	faces.resize(12);
	vertices.resize(24);
	normals.resize(24);

	for (int i=0; i<6; ++i)
	{
		faces[2*i] = VolumeTextureObject::triIndices[0] + 4*i;
		faces[2*i+1] = VolumeTextureObject::triIndices[1] + 4*i;
	}

	DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(scale.x,scale.y,scale.z);
	DirectX::XMMATRIX xRot = DirectX::XMMatrixRotationX(DirectX::XM_PI/2.0f);
	DirectX::XMMATRIX start = DirectX::XMMatrixIdentity();

	for (int i=0; i<16; ++i)
	{
		DirectX::XMFLOAT4 curF4(VolumeTextureObject::triVertices[i%4].x,VolumeTextureObject::triVertices[i%4].y,
			VolumeTextureObject::triVertices[i%4].z,1.0f);

		DirectX::XMVECTOR curVec = DirectX::XMLoadFloat4(&curF4);
		DirectX::XMVECTOR rotVec = DirectX::XMVector3TransformCoord(DirectX::XMVector3TransformCoord(curVec, start),scl);
		vertices[i] = Vec<float>(DirectX::XMVectorGetX(rotVec),DirectX::XMVectorGetY(rotVec),DirectX::XMVectorGetZ(rotVec));
		if (i%4==3)
			start = start*xRot;
	}

	DirectX::XMMATRIX yRot1 = DirectX::XMMatrixRotationY(DirectX::XM_PI/2.0f);
	DirectX::XMMATRIX yRot2 = DirectX::XMMatrixRotationY(3*DirectX::XM_PI/2.0f);

	for (int i=16; i<20; ++i)
	{
		DirectX::XMFLOAT4 curF4(VolumeTextureObject::triVertices[i%4].x,VolumeTextureObject::triVertices[i%4].y,
			VolumeTextureObject::triVertices[i%4].z,1.0f);

		DirectX::XMVECTOR curVec = DirectX::XMLoadFloat4(&curF4);
		DirectX::XMVECTOR rotVec = DirectX::XMVector3TransformCoord(DirectX::XMVector3TransformCoord(curVec, yRot1),scl);
		vertices[i] = Vec<float>(DirectX::XMVectorGetX(rotVec),DirectX::XMVectorGetY(rotVec),DirectX::XMVectorGetZ(rotVec));
	}

	for (int i=20; i<24; ++i)
	{
		DirectX::XMFLOAT4 curF4(VolumeTextureObject::triVertices[i%4].x,VolumeTextureObject::triVertices[i%4].y,
			VolumeTextureObject::triVertices[i%4].z,1.0f);

		DirectX::XMVECTOR curVec = DirectX::XMLoadFloat4(&curF4);
		DirectX::XMVECTOR rotVec = DirectX::XMVector3TransformCoord(DirectX::XMVector3TransformCoord(curVec, yRot2),scl);
		vertices[i] = Vec<float>(DirectX::XMVectorGetX(rotVec),DirectX::XMVectorGetY(rotVec),DirectX::XMVectorGetZ(rotVec));
	}

	for (int i=0; i<6; ++i)
	{
		Vec<float> edge1, edge2;
		Vec<double> norm;

		edge1 = vertices[faces[2*i].y] - vertices[faces[2*i].x];
		edge2 = vertices[faces[2*i].z] - vertices[faces[2*i].x];

		Vec<float> triDir = edge1.cross(edge2);

		norm = triDir.norm();

		for (int j=0; j<4 ; ++j)
			normals[faces[2*i].x+j] = norm;
	}

	CellHullObject* borderObj = new CellHullObject(gRenderer,faces,vertices,normals,gCameraDefaultMesh);
	cellHullObjects.push_back(borderObj);

	GraphicObjectNode* borderNode = new GraphicObjectNode(borderObj);
	borderObj->setColor(Vec<float>(0.0f,0.0f,0.0f), 1.0f);

	gRenderer->attachToRootScene(borderNode,Renderer::Pre,0);
}

// This is the entry point from Matlab
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs<1 || !mxIsChar(prhs[0])) mexErrMsgTxt("Usage:\n");// TODO make this useful

	if (messageLoopHandle)
	{
		checkRenderThread();
	}

	char* command = mxArrayToString(prhs[0]);

	if (_strcmpi("init",command)==0)
	{
		if (nrhs<7)
			mexErrMsgTxt("Not enough input arguments to initialize Lever 3-d.  Did you forget the widget?");

		if ( !messageLoopHandle )
			startThread();

		if ( registerExitFunction )
		{
			mexAtExit(exitFunc);
			registerExitFunction = FALSE;
		}

		loadWidget(prhs+1);
	}

	else if (_strcmpi("close",command)==0)
	{
		termThread();
		cleanUp();
	}

	else if (messageLoopHandle!=NULL)
	{
		if (_strcmpi("loadTexture",command)==0)
		{
			size_t numDims = mxGetNumberOfDimensions(prhs[1]);
			if (numDims<3)
				mexErrMsgTxt("Image must have at least three dimensions!");

			const mwSize* DIMS = mxGetDimensions(prhs[1]);
			Vec<size_t> dims = Vec<size_t>(DIMS[0],DIMS[1],DIMS[2]);
			int numChannels = 1;
			int numFrames = 1;
			if (numDims>3)
				numChannels = DIMS[3];

			if (numDims>4)
				numFrames = DIMS[4];

			unsigned char* image = (unsigned char*)mxGetData(prhs[1]);
			Vec<float> scale(dims);
			scale = scale / scale.maxValue();

			loadVolumeTexture(image,dims,numChannels,numFrames,scale);
		}

		else if (_strcmpi("getData",command)==0)
		{
			;
		}

		else if (_strcmpi("peelUpdate",command)==0)
		{
			if (nrhs!=2) mexErrMsgTxt("not the right arguments for peelUpdate!");

			//g_peelFactor = (float)mxGetScalar(prhs[1]);
		}

		else if (_strcmpi("lightingUpdate",command)==0)
		{
			if (nrhs!=2) mexErrMsgTxt("not the right arguments for lightingUpdate!");

			//g_lightingOn = (float)mxGetScalar(prhs[1]);
		}

		else if (_strcmpi("transferUpdate",command)==0)
		{
			if (nrhs!=19) mexErrMsgTxt("not the right arguments for transferUpdate!");

			// 		gTransferFunctions[0].x = (float)mxGetScalar(prhs[1]);
			// 		gTransferFunctions[0].y = (float)mxGetScalar(prhs[2]);
			// 		gTransferFunctions[0].z = (float)mxGetScalar(prhs[3]);
			// 
			// 		gTransferFunctions[1].x = (float)mxGetScalar(prhs[4]);
			// 		gTransferFunctions[1].y = (float)mxGetScalar(prhs[5]);
			// 		gTransferFunctions[1].z = (float)mxGetScalar(prhs[6]);
			// 
			// 		gTransferFunctions[2].x = (float)mxGetScalar(prhs[7]);
			// 		gTransferFunctions[2].y = (float)mxGetScalar(prhs[8]);
			// 		gTransferFunctions[2].z = (float)mxGetScalar(prhs[9]);
			// 
			// 		gTransferFunctions[3].x = (float)mxGetScalar(prhs[10]);
			// 		gTransferFunctions[3].y = (float)mxGetScalar(prhs[11]);
			// 		gTransferFunctions[3].z = (float)mxGetScalar(prhs[12]);
			// 
			// 		gTransferFunctions[4].x = (float)mxGetScalar(prhs[13]);
			// 		gTransferFunctions[4].y = (float)mxGetScalar(prhs[14]);
			// 		gTransferFunctions[4].z = (float)mxGetScalar(prhs[15]);
			// 
			// 		gTransferFunctions[5].x = (float)mxGetScalar(prhs[16]);
			// 		gTransferFunctions[5].y = (float)mxGetScalar(prhs[17]);
			// 		gTransferFunctions[5].z = (float)mxGetScalar(prhs[18]);

		}

		else if (_strcmpi("rangesUpdate",command)==0)
		{
			if (nrhs!=19) mexErrMsgTxt("Wrong input arguments for rangesUpdate");

			// 		gRanges[0].x = (float)mxGetScalar(prhs[1]);
			// 		gRanges[0].y = (float)mxGetScalar(prhs[2]);
			// 		gRanges[0].z = (float)mxGetScalar(prhs[3]);
			// 
			// 		gRanges[1].x = (float)mxGetScalar(prhs[4]);
			// 		gRanges[1].y = (float)mxGetScalar(prhs[5]);
			// 		gRanges[1].z = (float)mxGetScalar(prhs[6]);
			// 
			// 		gRanges[2].x = (float)mxGetScalar(prhs[7]);
			// 		gRanges[2].y = (float)mxGetScalar(prhs[8]);
			// 		gRanges[2].z = (float)mxGetScalar(prhs[9]);
			// 
			// 		gRanges[3].x = (float)mxGetScalar(prhs[10]);
			// 		gRanges[3].y = (float)mxGetScalar(prhs[11]);
			// 		gRanges[3].z = (float)mxGetScalar(prhs[12]);
			// 
			// 		gRanges[4].x = (float)mxGetScalar(prhs[13]);
			// 		gRanges[4].y = (float)mxGetScalar(prhs[14]);
			// 		gRanges[4].z = (float)mxGetScalar(prhs[15]);
			// 
			// 		gRanges[5].x = (float)mxGetScalar(prhs[16]);
			// 		gRanges[5].y = (float)mxGetScalar(prhs[17]);
			// 		gRanges[5].z = (float)mxGetScalar(prhs[18]);
		}

		else if (_strcmpi("colorsUpdate",command)==0)
		{
			if (nrhs!=19) mexErrMsgTxt("not the right arguments for transferUpdate!");

			// 		gChannelColor[0].x = (float)mxGetScalar(prhs[1]);
			// 		gChannelColor[0].y = (float)mxGetScalar(prhs[2]);
			// 		gChannelColor[0].z = (float)mxGetScalar(prhs[3]);
			// 
			// 		gChannelColor[1].x = (float)mxGetScalar(prhs[4]);
			// 		gChannelColor[1].y = (float)mxGetScalar(prhs[5]);
			// 		gChannelColor[1].z = (float)mxGetScalar(prhs[6]);
			// 
			// 		gChannelColor[2].x = (float)mxGetScalar(prhs[7]);
			// 		gChannelColor[2].y = (float)mxGetScalar(prhs[8]);
			// 		gChannelColor[2].z = (float)mxGetScalar(prhs[9]);
			// 
			// 		gChannelColor[3].x = (float)mxGetScalar(prhs[10]);
			// 		gChannelColor[3].y = (float)mxGetScalar(prhs[11]);
			// 		gChannelColor[3].z = (float)mxGetScalar(prhs[12]);
			// 
			// 		gChannelColor[4].x = (float)mxGetScalar(prhs[13]);
			// 		gChannelColor[4].y = (float)mxGetScalar(prhs[14]);
			// 		gChannelColor[4].z = (float)mxGetScalar(prhs[15]);
			// 
			// 		gChannelColor[5].x = (float)mxGetScalar(prhs[16]);
			// 		gChannelColor[5].y = (float)mxGetScalar(prhs[17]);
			// 		gChannelColor[5].z = (float)mxGetScalar(prhs[18]);

		}

		else if (_strcmpi("poll",command)==0)
		{
			if (nlhs!=1) mexErrMsgTxt("Wrong number of return arguments");
		}

		else if (_strcmpi("load hulls",command)==0)
		{
			const mxArray* hulls = prhs[1];
			if (hulls==NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

			//loadHulls(hulls);
		}

		else
		{
			char buff[255];
			sprintf_s(buff,"%s is not a valid command!\n",command);
			mexErrMsgTxt(buff);
		}
	}

	mxFree(command);
}
