#include "Vec.h"
#include "mex.h"
#include "MessageQueue.h"
#include "windows.h"
#include "MessageProcessor.h"
#include "Globals.h"
#include <set>

HANDLE gTermEvent = NULL;
HANDLE mexMessageMutex = NULL;
volatile bool gRendererInit = false;
HANDLE messageLoopHandle = NULL;

HINSTANCE gDllInstance = NULL;
DWORD threadID;

MessageQueue gMexMessageQueueOut;

std::vector<GraphicObjectNode*> gGraphicObjectNodes[GraphicObjectTypes::VTend];
CellHullObject* gBorderObj = NULL;
std::vector<VolumeTextureObject*> firstVolumeTextures;
std::vector<SceneNode*> hullRootNodes;

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
			gMexMessageQueueOut.addErrorMessage("Failed to initialize the renderer thread!");
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
	try 
	{
		if (messageLoopHandle!=NULL)
		{
			gRendererOn = false;

			Sleep(1000);

			if (gRenderer!=NULL)
				gRenderer->getMutex();

			for (int i=0; i<GraphicObjectTypes::VTend; ++i)
			{
				if (gRenderer!=NULL)
				{
					for (int j=0; j<gGraphicObjectNodes[i].size(); ++j)
					{
						gGraphicObjectNodes[i][j]->releaseRenderResources();
					}
				}

				gGraphicObjectNodes[i].clear();
			}

			gBorderObj = NULL;
			firstVolumeTextures.clear();
		}

		if (mexMessageMutex!=NULL)
		{
			CloseHandle(mexMessageMutex);
			mexMessageMutex = NULL;
		}

		termThread();
	}
	catch (const std::exception& e)
	{
		mexErrMsgTxt(e.what());
	}
	catch (const std::string& e)
	{
		mexErrMsgTxt(e.c_str());
	}
	catch (...)
	{
		mexErrMsgTxt("Caught an unknown error!");	
	}
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

CellHullObject* createCellHullObject(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals,
									 Camera* camera)
{
	std::vector<Vec<unsigned int>> faces;
	std::vector<Vec<float>> verts;
	std::vector<Vec<float>> normals;

	faces.resize(numFaces);
	verts.resize(numVerts);
	normals.resize(numNormals);

	Vec<unsigned int> curFace;
	for (int i=0; i<numFaces; ++i)
	{
		curFace.x = unsigned int(faceData[i]);
		curFace.y = unsigned int(faceData[i+numFaces]);
		curFace.z = unsigned int(faceData[i+2*numFaces]);

		curFace = curFace - 1;

		faces[i] = curFace;
	}

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
	if (gRenderer==NULL) return;

	gRenderer->getMutex();

	size_t numFaces = mxGetM(widget[0]);
	size_t numVerts = mxGetM(widget[1]);
	size_t numNormals = mxGetM(widget[2]);

	if (numVerts<1)
		mexErrMsgTxt("No Verts!");

	if (numFaces<1)
		mexErrMsgTxt("No faces!");

	if (numNormals<1)
		mexErrMsgTxt("No norms!");

	double* faceData = (double*)mxGetData(widget[0]);
	double* vertData = (double*)mxGetData(widget[1]);
	double* normData = (double*)mxGetData(widget[2]);

	SceneNode* widgetScene = new SceneNode();
	gRenderer->attachToRootScene(widgetScene,Renderer::Section::Post,0);

	CellHullObject* arrowX = createCellHullObject(faceData,numFaces,vertData,numVerts,normData,numNormals,gCameraWidget);
	arrowX->setColor(Vec<float>(1.0f, 0.2f, 0.2f),1.0f);
	GraphicObjectNode* arrowXnode = new GraphicObjectNode(arrowX);
	arrowXnode->setLocalToParent(DirectX::XMMatrixRotationY(DirectX::XM_PI/2.0f));
	arrowXnode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowXnode);

	CellHullObject* arrowY = createCellHullObject(faceData,numFaces,vertData,numVerts,normData,numNormals,gCameraWidget);
	arrowY->setColor(Vec<float>(0.1f, 1.0f, 0.1f),1.0f);
	GraphicObjectNode* arrowYnode = new GraphicObjectNode(arrowY);
	arrowYnode->setLocalToParent(DirectX::XMMatrixRotationX(-DirectX::XM_PI/2.0f));
	arrowYnode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowYnode);

	CellHullObject* arrowZ = createCellHullObject(faceData,numFaces,vertData,numVerts,normData,numNormals,gCameraWidget);
	arrowZ->setColor(Vec<float>(0.4f, 0.4f, 1.0f),1.0f);
	GraphicObjectNode* arrowZnode = new GraphicObjectNode(arrowZ);
	arrowZnode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(arrowZnode);

	numFaces = mxGetM(widget[3]);
	numVerts = mxGetM(widget[4]);
	numNormals = mxGetM(widget[5]);

	if (numVerts<1)
		mexErrMsgTxt("No Verts!");

	if (numFaces<1)
		mexErrMsgTxt("No faces!");

	if (numNormals<1)
		mexErrMsgTxt("No norms!");

	faceData = (double*)mxGetData(widget[3]);
	vertData = (double*)mxGetData(widget[4]);
	normData = (double*)mxGetData(widget[5]);

	CellHullObject* sphere = createCellHullObject(faceData,numFaces,vertData,numVerts,normData,numNormals,gCameraWidget);
	sphere->setColor(Vec<float>(0.9f,0.9f,0.9f),1.0f);
	GraphicObjectNode* sphereNode = new GraphicObjectNode(sphere);
	sphereNode->attachToParentNode(widgetScene);
	gGraphicObjectNodes[GraphicObjectTypes::Widget].push_back(sphereNode);

	gRenderer->releaseMutex();
}

void loadHulls(const mxArray* hulls)
{
	if (gRenderer==NULL) return;

	if (!gGraphicObjectNodes[GraphicObjectTypes::CellHulls].empty())
	{
		gRenderer->getMutex();
		for (int j=0; j<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->releaseRenderResources();
			delete gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j];
		}

		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].clear();

		gRenderer->updateRenderList();
		gRenderer->releaseMutex();
	}

	if (hullRootNodes.size()!=gRenderer->getNumberOfFrames())
	{
		for (int i=0; i<hullRootNodes.size(); ++i)
			delete hullRootNodes[i];
	}

	hullRootNodes.resize(gRenderer->getNumberOfFrames());
	for (unsigned int i=0; i<gRenderer->getNumberOfFrames(); ++i)
		hullRootNodes[i] = new SceneNode();

	size_t numHulls = mxGetNumberOfElements(hulls);
	for (size_t i=0; i<numHulls; ++i)
	{
		mxArray* mxFaces = mxGetField(hulls,i,"faces");
		mxArray* mxVerts = mxGetField(hulls,i,"verts");
		mxArray* mxNorms = mxGetField(hulls,i,"norms");
		mxArray* mxColor = mxGetField(hulls,i,"color");
		mxArray* mxFrame = mxGetField(hulls,i,"frame");
		mxArray* mxLabel = mxGetField(hulls,i,"label");
		mxArray* mxTrack = mxGetField(hulls,i,"track");

		size_t numFaces = mxGetM(mxFaces);
		size_t numVerts = mxGetM(mxVerts);
		size_t numNormals = mxGetM(mxNorms);

		if (numVerts<1)
			mexErrMsgTxt("No Verts!");

		if (numFaces<1)
			mexErrMsgTxt("No faces!");

		if (numNormals<1)
			mexErrMsgTxt("No norms!");

		if (numNormals!=numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
		int frame = int(mxGetScalar(mxFrame))-1;

		gRenderer->getMutex();

		CellHullObject* curHullObj = createCellHullObject(faceData,numFaces,vertData,numVerts,normData,numNormals,gCameraDefaultMesh);
		curHullObj->setColor(Vec<float>((float)colorData[0],(float)colorData[1],(float)colorData[2]),1.0f);
		curHullObj->setLabel((int)mxGetScalar(mxLabel));
		curHullObj->setTrack((int)mxGetScalar(mxTrack));
		GraphicObjectNode* curHullNode = new GraphicObjectNode(curHullObj);
		curHullNode->setWireframe(true);
		curHullNode->attachToParentNode(hullRootNodes[frame]);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].push_back(curHullNode);

		gRenderer->releaseMutex();
	}

	gRenderer->getMutex();

	for (unsigned int i=0; i<gRenderer->getNumberOfFrames(); ++i)
		gRenderer->attachToRootScene(hullRootNodes[i],Renderer::Section::Main,i);

	gRenderer->releaseMutex();
}

void createBorder(Vec<float> &scale)
{
	if (gRenderer==NULL) return;

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

	gGraphicObjectNodes[GraphicObjectTypes::Border].push_back(borderNode);

	gRenderer->releaseMutex();
}

void loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ)
{ 
	if (gRenderer==NULL) return;

	gRenderer->getMutex();

	unsigned char* shaderConstMemory = NULL;

	int fvtIdx = typ - GraphicObjectTypes::OriginalVolume;

	if (!firstVolumeTextures.empty() && fvtIdx<firstVolumeTextures.size() && NULL!=firstVolumeTextures[fvtIdx])
	{
		for (int i=0; i<gGraphicObjectNodes[typ].size(); ++i)
		{
			delete gGraphicObjectNodes[typ][i];
		}

		gGraphicObjectNodes[typ].clear();
	}

	for (int i=0; i<numFrames; ++i)
	{
		VolumeTextureObject* volumeTexture = new VolumeTextureObject(gRenderer,dims,numChannel,image+i*numChannel*dims.product(),scales,
			gCameraDefaultMesh,shaderConstMemory);
		shaderConstMemory = volumeTexture->getShaderConstMemory();

		GraphicObjectNode* volumeTextureNode = new GraphicObjectNode(volumeTexture);
		gRenderer->attachToRootScene(volumeTextureNode,Renderer::Section::Main,i);

		gGraphicObjectNodes[typ].push_back(volumeTextureNode);

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
	if (gRenderer==NULL) return;

	int fvtIdx = textureType - GraphicObjectTypes::OriginalVolume;

	gRenderer->getMutex();

	for (int i=0; i<firstVolumeTextures.size(); ++i)
	{
		int idx = GraphicObjectTypes::OriginalVolume + i;	
		if (idx==GraphicObjectTypes::VTend)
			break;

		bool render = i==fvtIdx;
		for (int j=0; j<gGraphicObjectNodes[idx].size(); ++j)
			gGraphicObjectNodes[idx][j]->setRenderable(render,true);
	}

	gGraphicObjectNodes[GraphicObjectTypes::OriginalVolume][0]->setRenderable(textureType==GraphicObjectTypes::OriginalVolume,false);

	gRenderer->releaseMutex();
}

void toggleSegmentationResults(bool on)
{
	if (gRenderer==NULL) return;

	gRenderer->getMutex();

	for (int i=0; i<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setRenderable(on,true);

	gGraphicObjectNodes[GraphicObjectTypes::CellHulls][0]->setRenderable(on,false);

	gRenderer->releaseMutex();
}

void toggleSegmentaionWireframe(bool wireframe)
{
	if (gRenderer==NULL) return;

	gRenderer->getMutex();

	for (int i=0; i<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setWireframe(wireframe);

	gRenderer->releaseMutex();
}

void toggleSegmentaionLighting(bool lighting)
{
	if (gRenderer==NULL) return;

	gRenderer->getMutex();

	for (int i=0; i<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setLightOn(lighting);

	gRenderer->releaseMutex();
}

void toggleSelectedCell(std::set<int> labels)
{
	if (gRenderer==NULL) return;

	gRenderer->getMutex();

	for (int i=0; i<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++i)
	{
		bool delay = true;

		if (i==gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size()-1)
			delay = false;

		if (labels.count(gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->getHullLabel())>0)
			gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setRenderable(true,delay);
		else
			gGraphicObjectNodes[GraphicObjectTypes::CellHulls][i]->setRenderable(false,delay);
	}

	gRenderer->releaseMutex();
}

void updateHulls(const mxArray* hulls)
{
	if (gRenderer==NULL) return;

	size_t numHulls = mxGetNumberOfElements(hulls);
	for (size_t i=0; i<numHulls; ++i)
	{
		mxArray* mxFaces = mxGetField(hulls,i,"faces");
		mxArray* mxVerts = mxGetField(hulls,i,"verts");
		mxArray* mxNorms = mxGetField(hulls,i,"norms");
		mxArray* mxColor = mxGetField(hulls,i,"color");
		mxArray* mxFrame = mxGetField(hulls,i,"frame");
		mxArray* mxLabel = mxGetField(hulls,i,"label");
		mxArray* mxTrack = mxGetField(hulls,i,"track");

		size_t numFaces = mxGetM(mxFaces);
		size_t numVerts = mxGetM(mxVerts);
		size_t numNormals = mxGetM(mxNorms);

		if (numVerts<1)
			mexErrMsgTxt("No Verts!");

		if (numFaces<1)
			mexErrMsgTxt("No faces!");

		if (numNormals<1)
			mexErrMsgTxt("No norms!");

		if (numNormals!=numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
		int frame = int(mxGetScalar(mxFrame))-1;

		int hullIdx = -1;
		for (int j=0; j<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			int label = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->getHullLabel();
			if (label==(int)mxGetScalar(mxLabel))
			{
				hullIdx = j;
				break;
			}
		}

		gRenderer->getMutex();

		SceneNode* parentSceneNode = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->getParentNode();
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->releaseRenderResources();
		delete gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx];

		CellHullObject* curHullObj = createCellHullObject(faceData,numFaces,vertData,numVerts,normData,numNormals,gCameraDefaultMesh);
		curHullObj->setColor(Vec<float>((float)colorData[0],(float)colorData[1],(float)colorData[2]),1.0f);
		curHullObj->setLabel((int)mxGetScalar(mxLabel));
		curHullObj->setTrack((int)mxGetScalar(mxTrack));
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx] = new GraphicObjectNode(curHullObj);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->setWireframe(true);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls][hullIdx]->attachToParentNode(parentSceneNode);

		gRenderer->releaseMutex();
	}
}

void addHulls(const mxArray* hulls)
{
	if (gRenderer==NULL) return;

	size_t numHulls = mxGetNumberOfElements(hulls);
	for (size_t i=0; i<numHulls; ++i)
	{
		mxArray* mxFaces = mxGetField(hulls,i,"faces");
		mxArray* mxVerts = mxGetField(hulls,i,"verts");
		mxArray* mxNorms = mxGetField(hulls,i,"norms");
		mxArray* mxColor = mxGetField(hulls,i,"color");
		mxArray* mxFrame = mxGetField(hulls,i,"frame");
		mxArray* mxLabel = mxGetField(hulls,i,"label");
		mxArray* mxTrack = mxGetField(hulls,i,"track");

		size_t numFaces = mxGetM(mxFaces);
		size_t numVerts = mxGetM(mxVerts);
		size_t numNormals = mxGetM(mxNorms);

		if (numVerts<1)
			mexErrMsgTxt("No Verts!");

		if (numFaces<1)
			mexErrMsgTxt("No faces!");

		if (numNormals<1)
			mexErrMsgTxt("No norms!");

		if (numNormals!=numVerts)
			mexErrMsgTxt("Number of verts does not match the number of normals!");

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
		int frame = int(mxGetScalar(mxFrame))-1;

		int hullIdx = -1;
		for (int j=0; j<gGraphicObjectNodes[GraphicObjectTypes::CellHulls].size(); ++j)
		{
			int label = gGraphicObjectNodes[GraphicObjectTypes::CellHulls][j]->getHullLabel();
			if (label==(int)mxGetScalar(mxLabel))
			{
				hullIdx = j;
				break;
			}
		}

		gRenderer->getMutex();

		CellHullObject* curHullObj = createCellHullObject(faceData,numFaces,vertData,numVerts,normData,numNormals,gCameraDefaultMesh);
		curHullObj->setColor(Vec<float>((float)colorData[0],(float)colorData[1],(float)colorData[2]),1.0f);
		curHullObj->setLabel((int)mxGetScalar(mxLabel));
		curHullObj->setTrack((int)mxGetScalar(mxTrack));
		GraphicObjectNode* curHullNode = new GraphicObjectNode(curHullObj);
		curHullNode->setWireframe(true);
		curHullNode->attachToParentNode(hullRootNodes[frame]);
		gGraphicObjectNodes[GraphicObjectTypes::CellHulls].push_back(curHullNode);

		gRenderer->releaseMutex();
	}
}

// This is the entry point from Matlab
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs<1 || !mxIsChar(prhs[0])) mexErrMsgTxt("Usage:\n");// TODO make this useful

	if (mexMessageMutex==NULL)
		mexMessageMutex = CreateMutex(NULL,FALSE,NULL);

	DWORD waitTerm = WaitForSingleObject(mexMessageMutex,360000);
	if (waitTerm==WAIT_TIMEOUT)
	{
		cleanUp();
		mexErrMsgTxt("Closed down Lever 3-D because it was not responding for over 6 min!");
		return;
	}

	// 	if (messageLoopHandle)
	// 	{
	// 		checkRenderThread();
	// 	}

	char* command = mxArrayToString(prhs[0]);

	try
	{
		if (_strcmpi("init",command)==0)
		{
			if (nrhs<7)
				mexErrMsgTxt("Not enough input arguments to initialize Lever 3-d.  Did you forget the widget?");

			if ( !messageLoopHandle )
			{
				startThread();

				if ( registerExitFunction )
				{
					mexAtExit(exitFunc);
					registerExitFunction = FALSE;
				}

				loadWidget(prhs+1);
				gRendererOn = true;
			}
		}

		else if (_strcmpi("close",command)==0)
		{
			cleanUp();

			gMexMessageQueueOut.clear();
		}

		else if (_strcmpi("poll",command)==0)
		{
			if (nlhs!=1) mexErrMsgTxt("Wrong number of return arguments");

			std::vector<Message> curMsgs = gMexMessageQueueOut.flushQueue();

			const char* fields[] = {"command","message","val"};
			plhs[0] = mxCreateStructMatrix(curMsgs.size(),1,3,fields);

			for (int i=0; i<curMsgs.size(); ++i)
			{
				mxArray* cmd = mxCreateString(curMsgs[i].command.c_str());
				mxArray* msg = mxCreateString(curMsgs[i].message.c_str());
				mxArray* val = mxCreateDoubleScalar(curMsgs[i].val);
				mxSetField(plhs[0],i,fields[0],cmd);
				mxSetField(plhs[0],i,fields[1],msg);
				mxSetField(plhs[0],i,fields[2],val);
			}
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

				if (firstVolumeTextures.empty())
				{					
					RECT desktop;
					const HWND hDesktop = GetDesktopWindow();
					GetWindowRect(hDesktop, &desktop);
					int horizontal = desktop.right *0.9;
					int vertical = desktop.bottom *0.9;

					if (vertical>horizontal)
					{
						gWindowHeight = (horizontal*dims.y)/dims.x;
						gWindowWidth = horizontal;
					}
					else
					{
						gWindowWidth = (vertical*dims.x)/dims.y;
						gWindowHeight = vertical;
					}

					SetWindowPos(gWindowHandle,0,0,0,gWindowWidth,gWindowHeight,SWP_NOZORDER|SWP_NOACTIVATE);
				}

				loadVolumeTexture(image,dims,numChannels,numFrames,scale,textureType);
				setCurrentTexture(textureType);

				if (gGraphicObjectNodes[GraphicObjectTypes::Border].empty())
					createBorder(scale);
			}

			else if (_strcmpi("getData",command)==0)
			{
				;
			}

			else if (_strcmpi("peelUpdate",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("not the right arguments for peelUpdate!");

				gRenderer->setClipChunkPercent((float)mxGetScalar(prhs[1]));
			}

			else if (_strcmpi("textureLightingUpdate",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("not the right arguments for lightingUpdate!");

				if (mxGetScalar(prhs[1])>0.0)
				{
					for (int i=0; i<firstVolumeTextures.size(); ++i)
					{
						if (NULL!=firstVolumeTextures[i])
						{
							firstVolumeTextures[i]->setLightOn(true);
						}
					}
				}
				else
				{
					for (int i=0; i<firstVolumeTextures.size(); ++i)
					{
						if (NULL!=firstVolumeTextures[i])
						{
							firstVolumeTextures[i]->setLightOn(false);
						}
					}
				}
			}

			else if (_strcmpi("segmentationLighting",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for segmentationLighting!");

				double onD = mxGetScalar(prhs[1]);
				bool on = onD>0;

				toggleSegmentaionLighting(on);
			}

			else if (_strcmpi("play",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for play!");

				double onD = mxGetScalar(prhs[1]);
				bool on = onD>0;

				gPlay = on;
			}

			else if (_strcmpi("rotate",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for rotate!");

				double onD = mxGetScalar(prhs[1]);
				bool on = onD>0;

				gRotate = on;
			}

			else if (_strcmpi("showLabels",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for showLabels!");

				double onD = mxGetScalar(prhs[1]);
				bool on = onD>0;

				gRenderer->setLabels(on);
			}

			else if (_strcmpi("resetView",command)==0)
			{
				gRenderer->resetRootWorldTransform();
				gCameraDefaultMesh->resetCamera();
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
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for viewTexture!");

				char buff[96];
				mxGetString(prhs[1],buff,96);

				GraphicObjectTypes textureType = GraphicObjectTypes::OriginalVolume;

				if (_strcmpi("original",buff)==0)
					textureType = GraphicObjectTypes::OriginalVolume;
				else if (_strcmpi("processed",buff)==0)
					textureType = GraphicObjectTypes::ProcessedVolume;

				setCurrentTexture(textureType);
			}

			else if (_strcmpi("viewSegmentation",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for viewSegmentation!");

				double onD = mxGetScalar(prhs[1]);
				bool on = onD>0;

				toggleSegmentationResults(on);
			}

			else if (_strcmpi("wireframeSegmentation",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for wireframeSegmentation!");

				double onD = mxGetScalar(prhs[1]);
				bool on = onD>0;

				toggleSegmentaionWireframe(on);
			}

			else if (_strcmpi("loadHulls",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for loadHulls!");

				const mxArray* hulls = prhs[1];
				if (hulls==NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

				loadHulls(hulls);
			}

			else if (_strcmpi("displayHulls",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for displayHulls!");

				double* hullList = (double*)mxGetData(prhs[1]);
				int numHulls = mxGetNumberOfElements(prhs[1]);

				std::set<int> hullset;
				for (int i=0; i<numHulls; ++i)
					hullset.insert((int)(hullList[i]));

				toggleSelectedCell(hullset);
			}

			else if (_strcmpi("setFrame",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for setFrame!");

				int curFrame = (int)mxGetScalar(prhs[1]);
				gRenderer->setCurrentFrame(curFrame);
			}

			else if (_strcmpi("setViewOrigin",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for setViewOrigin!");

				double* origin = (double*)mxGetData(prhs[1]);
				int numDims = mxGetNumberOfElements(prhs[1]);

				if (numDims!=3) mexErrMsgTxt("There needs to be three doubles for the view origin!");

				gRenderer->setWorldOrigin(Vec<float>((float)(origin[0]),(float)(origin[1]),(float)(origin[2])));
			}

			else if (_strcmpi("updateHulls",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for loadHulls!");

				const mxArray* hulls = prhs[1];
				if (hulls==NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

				updateHulls(hulls);
			}

			else if (_strcmpi("addHulls",command)==0)
			{
				if (nrhs!=2) mexErrMsgTxt("Not the right arguments for loadHulls!");

				const mxArray* hulls = prhs[1];
				if (hulls==NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

				addHulls(hulls);
			}

			else
			{
				char buff[255];
				sprintf_s(buff,"%s is not a valid command!\n",command);
				mexErrMsgTxt(buff);
			}
		}
	}
	catch (const std::exception& e)
	{
		mxFree(command);
		ReleaseMutex(mexMessageMutex);
		mexErrMsgTxt(e.what());
	}
	catch (const std::string& e) 
	{
		mxFree(command);
		ReleaseMutex(mexMessageMutex);
		mexErrMsgTxt(e.c_str());
	}
	catch (...)
	{
		mxFree(command);
		ReleaseMutex(mexMessageMutex);
		mexErrMsgTxt("Caught an unknown error!");	
	}

	mxFree(command);

	ReleaseMutex(mexMessageMutex);
}