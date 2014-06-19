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

enum GraphicObjectTypes //original 
{
	Widget,
	CellHulls,
	Border,
	OriginalVolume,
	ProcessedVolume,
	VTend
};

std::vector<GraphicObjectNode*> localGraphicObjectNodes[GraphicObjectTypes::VTend];
CellHullObject* gBorderObj = NULL;
std::vector<VolumeTextureObject*> firstVolumeTextures;

extern std::vector<DirectX::XMVECTOR> volumeBoundingVerts;

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
	if (messageLoopHandle!=NULL)
	{
		gRendererOn = false;

		Sleep(1000);

		gRenderer->getMutex();

		gBorderObj = NULL;
		firstVolumeTextures.clear();

		for (int i=0; i<GraphicObjectTypes::VTend; ++i)
		{
			for (int j=0; j<localGraphicObjectNodes[i].size(); ++j)
			{
				localGraphicObjectNodes[i][j]->releaseRenderResources();
			}

			localGraphicObjectNodes[i].clear();
		}
	}

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
		curFace.x = unsigned int(faceData[i]);
		curFace.y = unsigned int(faceData[i+numFaces]);
		curFace.z = unsigned int(faceData[i+2*numFaces]);

		curFace = curFace - 1;

		faces[i] = curFace;
	}

	double* vertData = (double*)mxGetData(widget[1]);
	double* normData = (double*)mxGetData(widget[2]);
	Vec<float> curVert, curNormal;
	for (int i=0; i<numVerts; ++i)
	{
		curVert.x = float(vertData[i]);
		curVert.y = float(vertData[i+numVerts]);
		curVert.z = float(vertData[i+2*numVerts]);

		curNormal.x = float(normData[i]);
		curNormal.y = float(normData[i+numVerts]);
		curNormal.z = float(normData[i+2*numVerts]);

		verts[i] = curVert;
		normals[i] = curNormal;
	}

	CellHullObject* cho = new CellHullObject(gRenderer,faces,verts,normals,camera);

	return cho;
}

void loadWidget(const mxArray* widget[])
{
	gRenderer->getMutex();

	SceneNode* widgetScene = new SceneNode();
	gRenderer->attachToRootScene(widgetScene,Renderer::Section::Post,0);

	CellHullObject* arrowX = createCellHullObject(widget,gCameraWidget);
	arrowX->setColor(Vec<float>(1.0f, 0.2f, 0.2f),1.0f);
	GraphicObjectNode* arrowXnode = new GraphicObjectNode(arrowX);
	arrowXnode->setLocalToParent(DirectX::XMMatrixRotationY(DirectX::XM_PI/2.0f));
	arrowXnode->attachToParentNode(widgetScene);
	localGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowXnode);

	CellHullObject* arrowY = createCellHullObject(widget,gCameraWidget);
	arrowY->setColor(Vec<float>(0.1f, 1.0f, 0.1f),1.0f);
	GraphicObjectNode* arrowYnode = new GraphicObjectNode(arrowY);
	arrowYnode->setLocalToParent(DirectX::XMMatrixRotationX(-DirectX::XM_PI/2.0f));
	arrowYnode->attachToParentNode(widgetScene);
	localGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowYnode);

	CellHullObject* arrowZ = createCellHullObject(widget,gCameraWidget);
	arrowZ->setColor(Vec<float>(0.4f, 0.4f, 1.0f),1.0f);
	GraphicObjectNode* arrowZnode = new GraphicObjectNode(arrowZ);
	arrowZnode->attachToParentNode(widgetScene);
	localGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowZnode);

	CellHullObject* sphere = createCellHullObject(widget+3,gCameraWidget);
	sphere->setColor(Vec<float>(0.9f,0.9f,0.9f),1.0f);
	GraphicObjectNode* sphereNode = new GraphicObjectNode(sphere);
	sphereNode->attachToParentNode(widgetScene);
	localGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(sphereNode);

	gRenderer->releaseMutex();
}

void createBorder(Vec<float> &scale)
{
	gRenderer->getMutex();

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

	gBorderObj = new CellHullObject(gRenderer,faces,vertices,normals,gCameraDefaultMesh);

	GraphicObjectNode* borderNode = new GraphicObjectNode(gBorderObj);
	gBorderObj->setColor(Vec<float>(0.0f,0.0f,0.0f), 1.0f);
	gRenderer->attachToRootScene(borderNode,Renderer::Pre,0);

	localGraphicObjectNodes[GraphicObjectTypes::Border].push_back(borderNode);

	gRenderer->releaseMutex();
}

void loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ)
{ 
	gRenderer->getMutex();

	unsigned char* shaderConstMemory = NULL;

	int fvtIdx = typ - GraphicObjectTypes::OriginalVolume;

	if (!firstVolumeTextures.empty() && fvtIdx<firstVolumeTextures.size() && NULL!=firstVolumeTextures[fvtIdx])
	{
		for (int i=0; i<localGraphicObjectNodes[typ].size(); ++i)
		{
			delete localGraphicObjectNodes[typ][i];
		}

		localGraphicObjectNodes[typ].clear();
	}

	for (int i=0; i<numFrames; ++i)
	{
		VolumeTextureObject* volumeTexture = new VolumeTextureObject(gRenderer,dims,numChannel,image+i*numChannel*dims.product(),scales,
			gCameraDefaultMesh,shaderConstMemory);
		shaderConstMemory = volumeTexture->getShaderConstMemory();

		GraphicObjectNode* volumeTextureNode = new GraphicObjectNode(volumeTexture);
		gRenderer->attachToRootScene(volumeTextureNode,Renderer::Section::Main,i);

		localGraphicObjectNodes[typ].push_back(volumeTextureNode);

		if (0==i)
		{
			if (fvtIdx+1>firstVolumeTextures.size())
				firstVolumeTextures.resize(fvtIdx+1);

			firstVolumeTextures[fvtIdx] = volumeTexture;
		}
	}

	gRenderer->releaseMutex();
}

void setCurrentTexture(GraphicObjectTypes textureType)
{
	int fvtIdx = textureType - GraphicObjectTypes::OriginalVolume;

	gRenderer->getMutex();

	for (int i=0; i<firstVolumeTextures.size(); ++i)
	{
		int idx = GraphicObjectTypes::OriginalVolume + i;	
		if (idx==GraphicObjectTypes::VTend)
			break;

		bool render = i==fvtIdx;
		for (int j=0; j<localGraphicObjectNodes[idx].size(); ++j)
			localGraphicObjectNodes[idx][j]->setRenderable(render,true);
	}

	localGraphicObjectNodes[GraphicObjectTypes::OriginalVolume][0]->setRenderable(textureType==GraphicObjectTypes::OriginalVolume,false);

	gRenderer->releaseMutex();
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
		gRendererOn = true;
	}

	else if (_strcmpi("close",command)==0)
	{
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
				numChannels = int(DIMS[3]);

			if (numDims>4)
				numFrames = int(DIMS[4]);

			unsigned char* image = (unsigned char*)mxGetData(prhs[1]);

			Vec<float> scale(dims);
			scale = scale / scale.maxValue();
			if (nrhs>2)
			{
				double* physDims = (double*)mxGetData(prhs[2]);
				scale.y *= float(physDims[1]/physDims[0]);
				scale.z *= float(physDims[2]/physDims[0]);
			}

			GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;
			if (nrhs>3)
			{
				char buff[96];
				mxGetString(prhs[3],buff,96);

				if (_strcmpi("original",buff)==0)
					textureType = GraphicObjectTypes::OriginalVolume;
				else if (_strcmpi("processed",buff)==0)
					textureType = GraphicObjectTypes::ProcessedVolume;
			}

			loadVolumeTexture(image,dims,numChannels,numFrames,scale,textureType);
			setCurrentTexture(textureType);

			if (localGraphicObjectNodes[GraphicObjectTypes::Border].empty())
				createBorder(scale);
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

		else if (_strcmpi("textureLightingUpdate",command)==0)
		{
			if (nrhs!=2) mexErrMsgTxt("not the right arguments for lightingUpdate!");

			if (mxGetScalar(prhs[1])>0.0)
				for (int i=0; i<firstVolumeTextures.size(); ++i)
					if (NULL!=firstVolumeTextures[i])
						firstVolumeTextures[i]->setLightOn(true);
			else
				for (int i=0; i<firstVolumeTextures.size(); ++i)
					if (NULL!=firstVolumeTextures[i])
						firstVolumeTextures[i]->setLightOn(false);
		}

		else if (_strcmpi("transferUpdate",command)==0)
		{
			if (2>nrhs || 3<nlhs) mexErrMsgTxt("This is not the right number of input arguments for transferUpdate!");

			GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;
			if (nrhs>2)
			{
				char buff[96];
				mxGetString(prhs[2],buff,96);

				if (_strcmpi("original",buff)==0)
					textureType = GraphicObjectTypes::OriginalVolume;
				else if (_strcmpi("processed",buff)==0)
					textureType = GraphicObjectTypes::ProcessedVolume;
			}

			int fvtIdx = textureType - GraphicObjectTypes::OriginalVolume;

			size_t numElem = mxGetNumberOfElements(prhs[1]);
			
			if (firstVolumeTextures.size()-1<fvtIdx || NULL==firstVolumeTextures[fvtIdx] || numElem!=firstVolumeTextures[fvtIdx]->getNumberOfChannels())
				mexErrMsgTxt("Number of elements passed in do not match the number of channels in the image data!");

			for (int chan=0; chan<firstVolumeTextures[fvtIdx]->getNumberOfChannels(); ++chan)
			{
				Vec<float> transferFunction(0.0f,0.0f,0.0f);
				Vec<float> ranges;
				Vec<float> color;
				float alphaMod;

				mxArray* mxColorPt = mxGetField(prhs[1],chan,"color");
				double* mxColor = (double*)mxGetData(mxColorPt);
				color = Vec<float>((float)(mxColor[0]),(float)(mxColor[1]),(float)(mxColor[2]));

					mxArray* mxAPt = mxGetField(prhs[1],chan,"a");
					mxArray* mxBPt = mxGetField(prhs[1],chan,"b");
					mxArray* mxCPt = mxGetField(prhs[1],chan,"c");
					double a = mxGetScalar(mxAPt);
					double b = mxGetScalar(mxBPt);
					double c = mxGetScalar(mxCPt);
					transferFunction = Vec<float>((float)a,(float)b,(float)c);

				mxArray* mxMin = mxGetField(prhs[1],chan,"minVal");
				mxArray* mxMax = mxGetField(prhs[1],chan,"maxVal");
				ranges = Vec<float>((float)mxGetScalar(mxMin),(float)mxGetScalar(mxMax),1.0f);

				mxArray* mxAlphaPt = mxGetField(prhs[1],chan,"alphaMod");
				mxArray* mxOnPt = mxGetField(prhs[1],chan,"visible");
				if (mxGetScalar(mxOnPt)!=0)
					alphaMod = (float)mxGetScalar(mxAlphaPt);
				else
					alphaMod = 0.0f;

				firstVolumeTextures[fvtIdx]->setTransferFunction(chan,transferFunction);
				firstVolumeTextures[fvtIdx]->setRange(chan,ranges);
				firstVolumeTextures[fvtIdx]->setColor(chan,color,alphaMod);
			}
		}

		else if (_strcmpi("viewTexture",command)==0)
		{
			if (nrhs!=2) mexErrMsgTxt("not the right arguments for viewTexture!");

			char buff[96];
			mxGetString(prhs[1],buff,96);

			GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;

			if (_strcmpi("original",buff)==0)
				textureType = GraphicObjectTypes::OriginalVolume;
			else if (_strcmpi("processed",buff)==0)
				textureType = GraphicObjectTypes::ProcessedVolume;

			setCurrentTexture(textureType);

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