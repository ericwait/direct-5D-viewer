////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "Renderer.h"
#include "MeshPrimitive.h"
#include "Material.h"
#include "MaterialParams.h"
#include "Camera.h"
#include "Timer.h"
#include "RenderTarget.h"
#include "DepthTarget.h"
#include "VolumeInfo.h"
#include "TextRenderer.h"

#include "Global/Defines.h"
#include "Global/Globals.h"

#include "Global/ErrorMsg.h"

#include <d3dcompiler.h>
#include <Windows.h>

#include <set>
#include <map>
#include <regex>
#include <string>
#include <fstream>

const std::string SHADER_DIR = "Shaders";
const float Renderer::cornerVolumeDist = 1.75f;

Renderer::Renderer()
{
	textRenderer = NULL;
	volInfo = NULL;

	fallbackPS = NULL;

	isDirty = true;
	isRendering = false;
	backgroundColor = Vec<float>(0.25f, 0.25f, 0.25f);
	currentFrame = 0;
	clipChunkPercent = 1.0f;
	FrontClipPos(-cornerVolumeDist);
	BackClipPos(cornerVolumeDist);
	numPlanes = 0;
	labelsOn = true;
	frameNumOn = true;
	scaleTextOn = true;
	scaleBarOn = true;
	fpsOn = false;
	statsOn = false;
	captureFilePath = "./";
	captureFileName = "";
	dllRoot = "";

	for(int i = 0; i<NUM_TIMES; ++i)
	{
		frameTimes[i] = 0;
		startTimes[i] = 0;
		preTimes  [i] = 0;
		mainTimes [i] = 0;
		postTimes [i] = 0;
		gdiTimes  [i] = 0;
		endTimes  [i] = 0;
	}

	curTimeIdx = 0;
}

Renderer::~Renderer()
{
	SAFE_DELETE(volInfo);
	SAFE_DELETE(rootScene);
	SAFE_DELETE(textRenderer);

	clearVertexShaderList();
	clearPixelShaderList();

	clearFallbackShaders();

	SAFE_RELEASE(vertexShaderConstBuffer);
	SAFE_RELEASE(blendState);

	releaseMaterialStates();
	releaseDepthStencils();
	releaseRenderTargets();
	releaseDevice();
}

HRESULT Renderer::init(std::string rootDir, Vec<int> viewportSizeIn)
{
	dllRoot = rootDir;

	for ( int i=0; i < TargetChains::NumTC; ++i )
		viewportSize[i] = viewportSizeIn;

	HRESULT hr = initDevice();
	if (FAILED(hr))
		return hr;

	hr = initRenderTargets();
	if (FAILED(hr))
		return hr;

	hr = initDepthStencils();
	if (FAILED(hr))
		return hr;

	createBlendState();

	hr = createConstantBuffer(sizeof(VertexShaderConstBuffer),&vertexShaderConstBuffer);
	if (FAILED(hr))
		return hr;

	rootScene = new RootSceneNode();

	resetRootWorldTransform();

	initFallbackShaders();

	resetViewPort(TargetChains::Screen);

	// Make sure this is initialized after fallback renderers and viewports
	textRenderer = new TextRenderer(this, gWindowHandle, "Consolas", 20);

	setRendering(true);

	return hr;
}

HRESULT Renderer::initDevice()
{
	//WaitForSingleObject(mutexDevice,INFINITE);

	HRESULT hr = E_FAIL;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =	{D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =	{D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for(UINT driverTypeIndex=0; driverTypeIndex<numDriverTypes; ++driverTypeIndex)
	{
		D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
								&renderDevice, &renderFeatureLevel, &renderContext);

		if( SUCCEEDED(hr) )
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get a hold of the IDXGIFactory2 factory that creates our devices, etc. Necessary for making new swap chains.
	IDXGIDevice2* pDXGIDevice2;
	hr = renderDevice->QueryInterface(__uuidof(IDXGIDevice2), (void **)&pDXGIDevice2);
	if ( FAILED(hr) )
		return hr;

	IDXGIAdapter* pDXGIAdapter;
	hr = pDXGIDevice2->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	if ( FAILED(hr) )
		return hr;

	hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void **)&renderFactory);
	if ( FAILED(hr) )
		return hr;

	//ReleaseMutex(mutexDevice);

	return hr;
}

void Renderer::releaseDevice()
{
	flushContext();

	SAFE_RELEASE(renderDevice);
	SAFE_RELEASE(renderContext);
}

HRESULT Renderer::initRenderTargets()
{
	Vec<int> size = viewportSize[TargetChains::Screen];
	renderChains[TargetChains::Screen][RenderTargetTypes::DefaultRT] = std::make_shared<SwapChainTarget>(this, gWindowHandle, size.x, size.y);
	renderChains[TargetChains::Capture][RenderTargetTypes::DefaultRT] = std::make_shared<ReadbackRenderTarget>(this, size.x, size.y);

	return S_OK;
}

void Renderer::releaseRenderTargets()
{
	// Assign null (these are shared pointers, so it'll clear the render targets)
	for ( int i=0; i < TargetChains::NumTC; ++i )
	{
		for ( int j=0; j < RenderTargetTypes::NumRT; ++j )
			renderChains[i][j] = nullptr;
	}
}

HRESULT Renderer::initDepthStencils()
{
	Vec<int> size = viewportSize[TargetChains::Screen];
	depthChains[TargetChains::Screen][DepthTargetTypes::DefaultDT] = std::make_shared<RenderDepthTarget>(this, size.x, size.y);
	depthChains[TargetChains::Capture][DepthTargetTypes::DefaultDT] = std::make_shared<RenderDepthTarget>(this, size.x, size.y);


	return S_OK;
}

void Renderer::releaseDepthStencils()
{
	// Assign null (these are shared pointers, so it'll clear the depth targets)
	for ( int i=0; i < TargetChains::NumTC; ++i )
	{
		for ( int j=0; j < DepthTargetTypes::NumDT; ++j )
			depthChains[i][j] = nullptr;
	}
}

void Renderer::releaseMaterialStates()
{
	std::map<unsigned int,ID3D11DepthStencilState*>::iterator dsIter;

	for ( dsIter = depthStencilStates.begin(); dsIter != depthStencilStates.end(); ++dsIter )
		SAFE_RELEASE(dsIter->second);

	depthStencilStates.clear();

	std::map<unsigned int,ID3D11RasterizerState*>::iterator rasterIter;

	for ( rasterIter = rasterStates.begin(); rasterIter != rasterStates.end(); ++rasterIter )
		SAFE_RELEASE(rasterIter->second);

	rasterStates.clear();
}

HRESULT Renderer::createVertexBuffer(UINT accessFlags, D3D11_USAGE usage, size_t bufferSize, const void* initData, ID3D11Buffer** vertexBufferOut)
{
	//WaitForSingleObject(mutexDevice,INFINITE);

	if ( bufferSize == 0 )
		return E_FAIL;

	D3D11_BUFFER_DESC vertBufferDesc;
	D3D11_SUBRESOURCE_DATA vertData;

	vertBufferDesc.Usage = usage;
	vertBufferDesc.ByteWidth = (unsigned int)bufferSize;
	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.CPUAccessFlags = accessFlags;
	vertBufferDesc.MiscFlags = 0;
	vertBufferDesc.StructureByteStride = 0;

	vertData.pSysMem = initData;
	vertData.SysMemPitch = 0;
	vertData.SysMemSlicePitch = 0;

	HRESULT result = renderDevice->CreateBuffer(&vertBufferDesc, &vertData, vertexBufferOut);
	if ( FAILED(result) )
		*vertexBufferOut = NULL;

	//ReleaseMutex(mutexDevice);

	return result;
}

HRESULT Renderer::createIndexBuffer(UINT accessFlags, D3D11_USAGE usage, const std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut)
{
	//WaitForSingleObject(mutexDevice,INFINITE);
	if (faces.size()==0)
		return E_FAIL;

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexBufferDesc.Usage = usage;
	indexBufferDesc.ByteWidth = (unsigned int)(sizeof(Vec<unsigned int>) * faces.size());
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = accessFlags;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = faces.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	HRESULT result = renderDevice->CreateBuffer(&indexBufferDesc, &indexData, indexBufferOut);
	if ( FAILED(result) )
		*indexBufferOut = NULL;

	//ReleaseMutex(mutexDevice);

	return result;
}

HRESULT Renderer::createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut)
{
	//WaitForSingleObject(mutexDevice,INFINITE);
	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (unsigned int)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	HRESULT hr = renderDevice->CreateBuffer(&bufferDesc, NULL, constBufferOut);

	//ReleaseMutex(mutexDevice);

	return hr;
}

HRESULT Renderer::lockBuffer(ID3D11Buffer* buffer, D3D11_MAP mapType, D3D11_MAPPED_SUBRESOURCE& outResource)
{
	return renderContext->Map(buffer, 0, mapType, 0, &outResource);
}

void Renderer::releaseBuffer(ID3D11Buffer* buffer)
{
	renderContext->Unmap(buffer, 0);
}

void Renderer::updateShaderParams(const void* params, ID3D11Buffer* buffer)
{
	renderContext->UpdateSubresource(buffer,0,NULL,params,0,0);
}

void Renderer::togglestats()
{
	statsOn = !statsOn;
	if(!fpsOn)
		fpsOn = true;
}

std::string makeVariableString(const std::map<std::string, std::string>& variables)
{
	std::string strOut;
	for (auto varIter=variables.begin(); varIter != variables.end(); ++varIter)
	{
		strOut += varIter->first + "=" + varIter->second + ",";
	}

	return strOut;
}

FILETIME getFileUpdateTime(const std::string& filePath)
{
	FILETIME lastUpdate = {0};

	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if ( hFile == INVALID_HANDLE_VALUE )
		return lastUpdate;

	GetFileTime(hFile, NULL, NULL, &lastUpdate);

	CloseHandle(hFile);

	return lastUpdate;
}

int Renderer::registerVertexShader(const std::string& filename, const std::string& entrypoint,
		VertexLayout layoutInfo, const std::map<std::string, std::string>& variables)
{
	std::string rootDir = getDllDir();

	std::string paramStr = makeVariableString(variables);

	std::string shaderLookupName = filename + ":" + entrypoint + "_" + layoutInfo.getName() + ":" + paramStr;
	if ( 0 != vertexShaderMap.count(shaderLookupName) )
	{
		return vertexShaderMap[shaderLookupName];
	}

	std::string shaderPath = rootDir + "/" + SHADER_DIR + "/" + filename + ".hlsl";

	// Add a new shader entry regardless of error state (build error entry by default)
	int entryIdx = (int)vertexShaderRegistry.size();

	VertexShaderEntry newEntry;
	newEntry.filename = filename;
	newEntry.entryFunc = entrypoint;
	newEntry.vars = variables;
	newEntry.update = getFileUpdateTime(shaderPath);
	newEntry.layoutInfo = layoutInfo;

	// Assume an error case
	newEntry.error = true;
	newEntry.shader = fallbackVSs[layoutInfo.getType()];
	newEntry.layout = fallbackLayouts[layoutInfo.getType()];

	vertexShaderRegistry.push_back(newEntry);
	vertexShaderMap.insert(std::pair<std::string,int>(shaderLookupName,entryIdx));

	ID3D11VertexShader* newShader;
	ID3D11InputLayout* newLayout;
	if ( FAILED(compileVertexShader(shaderPath, entrypoint, variables, layoutInfo.getLayoutDesc(), &newShader, &newLayout)) )
	{
		return entryIdx;
	}

	vertexShaderRegistry[entryIdx].error = false;
	vertexShaderRegistry[entryIdx].shader = newShader;
	vertexShaderRegistry[entryIdx].layout = newLayout;

	return entryIdx;
}

int Renderer::registerPixelShader(const std::string& filename, const std::string& entrypoint, const std::map<std::string, std::string>& variables)
{
	std::string rootDir = getDllDir();

	std::string paramStr = makeVariableString(variables);

	std::string shaderLookupName = filename + ":" + entrypoint + ":" + paramStr;
	if ( 0 != pixelShaderMap.count(shaderLookupName) )
	{
		return pixelShaderMap[shaderLookupName];
	}

	std::string shaderPath = rootDir + "/" + SHADER_DIR + "/" + filename + ".hlsl";

	// Add a new shader entry regardless of error state (build error entry by default)
	int entryIdx = (int)pixelShaderRegistry.size();

	PixelShaderEntry newEntry;
	newEntry.filename = filename;
	newEntry.entryFunc = entrypoint;
	newEntry.vars = variables;
	newEntry.error = true;
	newEntry.update = getFileUpdateTime(shaderPath);

	// Assume an error case
	newEntry.error = true;
	newEntry.shader = fallbackPS;

	pixelShaderRegistry.push_back(newEntry);
	pixelShaderMap.insert(std::pair<std::string,int>(shaderLookupName,entryIdx));

	ID3D11PixelShader* newShader;
	if ( FAILED(compilePixelShader(shaderPath, entrypoint, variables, &newShader)) )
	{
		return entryIdx;
	}

	pixelShaderRegistry[entryIdx].error = false;
	pixelShaderRegistry[entryIdx].shader = newShader;

	return entryIdx;
}

void Renderer::initFallbackShaders()
{
	std::string rootDir = getDllDir();

	if ( !pixelShaderRegistry.empty() )
		clearVertexShaderList();

	if ( !vertexShaderRegistry.empty() )
		clearPixelShaderList();


	std::map<std::string, std::string> emptyVar;

	std::string vertexFuncPrefix = "FallbackVS";
	std::string shaderPath = rootDir + "/" + SHADER_DIR + "/Fallback.hlsl";

	for ( const auto& it : VertexLayout::layoutInfo )
	{
		std::string vertexFunc = vertexFuncPrefix + "_" + it.second.name;

		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc = it.second.inputLayout;

		ID3D11VertexShader* newVertShader;
		ID3D11InputLayout* newVertLayout;
		if ( FAILED(compileVertexShader(shaderPath, vertexFunc, emptyVar, layoutDesc, &newVertShader, &newVertLayout)) )
		{
			throw std::runtime_error("Unable to create default fallback vertex shader!");
		}


		fallbackVSs.insert(std::make_pair(it.first,newVertShader));
		fallbackLayouts.insert(std::make_pair(it.first, newVertLayout));
	}
	
	std::string pixelFunc = "FallbackPS";

	ID3D11PixelShader* newPixelShader;
	if ( FAILED(compilePixelShader(shaderPath, pixelFunc, emptyVar, &newPixelShader)) )
	{
		throw std::runtime_error("Unable to create default fallback pixel shader!");
	}

	fallbackPS = newPixelShader;
}

int Renderer::updateRegisteredShaders()
{
	std::string rootDir = getDllDir();

	int updatedShaders = FALSE;

	// Check for updated file times in vertex shader files
	for ( int i = 0; i < vertexShaderRegistry.size(); ++i )
	{
		std::string shaderPath = rootDir + "/" + SHADER_DIR + "/" + vertexShaderRegistry[i].filename + ".hlsl";
		FILETIME updateTime = getFileUpdateTime(shaderPath);

		if ( CompareFileTime(&vertexShaderRegistry[i].update,&updateTime) >= 0 )
			continue;

		updateVertexShader(i);
		vertexShaderRegistry[i].update = updateTime;

		updatedShaders = TRUE;
	}

	// Check for updated file times in pixel shader files
	for ( int i = 0; i < pixelShaderRegistry.size(); ++i )
	{
		std::string shaderPath = rootDir + "/" + SHADER_DIR + "/" + pixelShaderRegistry[i].filename + ".hlsl";
		FILETIME updateTime = getFileUpdateTime(shaderPath);

		if ( CompareFileTime(&pixelShaderRegistry[i].update,&updateTime) >= 0 )
			continue;

		updatePixelShader(i);
		pixelShaderRegistry[i].update = updateTime;

		updatedShaders = TRUE;
	}

	return updatedShaders;
}

void Renderer::updateVertexShader(int entryIdx)
{
	VertexShaderEntry& vsEntry = vertexShaderRegistry[entryIdx];

	std::string rootDir = getDllDir();
	std::string shaderPath = rootDir + "/" + SHADER_DIR + "/" + vsEntry.filename + ".hlsl";

	bool error = vsEntry.error;

	// Clean out the old shader and layout entries
	if ( !error )
	{
		SAFE_RELEASE(vsEntry.shader);
		SAFE_RELEASE(vsEntry.layout);
	}

	// Set up shader entry for the error case
	vsEntry.error = true;
	vsEntry.shader = fallbackVSs[vsEntry.layoutInfo.getType()];
	vsEntry.layout = fallbackLayouts[vsEntry.layoutInfo.getType()];

	ID3D11VertexShader* newShader;
	ID3D11InputLayout* newLayout;
	if ( FAILED(compileVertexShader(shaderPath, vsEntry.entryFunc, vsEntry.vars, vsEntry.layoutInfo.getLayoutDesc(), &newShader, &newLayout)) )
	{
		return;
	}

	vsEntry.error = false;
	vsEntry.shader = newShader;
	vsEntry.layout = newLayout;
}

void Renderer::updatePixelShader(int entryIdx)
{
	PixelShaderEntry& psEntry = pixelShaderRegistry[entryIdx];

	std::string rootDir = getDllDir();
	std::string shaderPath = rootDir + "/" + SHADER_DIR + "/" + psEntry.filename + ".hlsl";

	bool error = psEntry.error;

	// Clean out the old shader and layout entries
	if ( !error )
	{
		SAFE_RELEASE(psEntry.shader);
	}

	// Set up shader entry for the error case
	psEntry.error = true;
	psEntry.shader = fallbackPS;

	ID3D11PixelShader* newShader;
	if ( FAILED(compilePixelShader(shaderPath, psEntry.entryFunc, psEntry.vars, &newShader)) )
	{
		return;
	}

	psEntry.error = false;
	psEntry.shader = newShader;
}

void Renderer::clearVertexShaderList()
{
	for ( int i = 0; i<vertexShaderRegistry.size(); ++i )
	{
		// Don't delete fallbacks (handled separately)
		if ( vertexShaderRegistry[i].error )
			continue;

		SAFE_RELEASE(vertexShaderRegistry[i].shader);
		SAFE_RELEASE(vertexShaderRegistry[i].layout);
	}

	vertexShaderRegistry.clear();
	vertexShaderMap.clear();
}

void Renderer::clearPixelShaderList()
{
	for ( int i = 0; i<pixelShaderRegistry.size(); ++i )
	{
		// Don't delete fallbacks (handled separately)
		if ( pixelShaderRegistry[i].error )
			continue;

		SAFE_RELEASE(pixelShaderRegistry[i].shader);
	}

	pixelShaderRegistry.clear();
	pixelShaderMap.clear();
}

void Renderer::clearFallbackShaders()
{
	for ( auto& it : fallbackVSs )
		SAFE_RELEASE(it.second);

	for ( auto& it : fallbackLayouts )
		SAFE_RELEASE(it.second);

	fallbackVSs.clear();
	fallbackLayouts.clear();

	SAFE_RELEASE(fallbackPS);
}

float Renderer::FrontClipPos() const
{
	return frontClipPos;
}

void Renderer::FrontClipPos(float val)
{
	frontClipPos = val;
}

float Renderer::BackClipPos() const
{
	return backClipPos;
}

void Renderer::BackClipPos(float val)
{
	backClipPos = val;
}

void Renderer::renderUpdate()
{
	if ( needsUpdate() )
	{
		renderAll(TargetChains::Screen);

		isDirty = false;
	}
}

void Renderer::renderAll(TargetChains renderChain)
{
	static int counter = 0;

	UINT64 frameTime = GetTimeMs64();
	//rootScene->updateRenderableList();

	UINT64 startTime = GetTimeMs64();
	startRender(renderChain);
	startTimes[curTimeIdx] = GetTimeMs64()-startTime;

	UINT64 preTime = GetTimeMs64();
	renderBackground(renderChain);
	preTimes[curTimeIdx] = GetTimeMs64()-preTime;

	// Clear depth target before rendering polygons and volume data
	clearDepthTarget(renderChain, DepthTargetTypes::DefaultDT, 1.0f);

	UINT64 mainTime = GetTimeMs64();
	renderPolygons(renderChain);
	renderVolume(renderChain);
	mainTimes[curTimeIdx] = GetTimeMs64()-mainTime;

	// And again before rendering the widget
	clearDepthTarget(renderChain, DepthTargetTypes::DefaultDT, 1.0f);

	UINT64 postTime = GetTimeMs64();
	renderWidget(renderChain);
	postTimes[curTimeIdx] = GetTimeMs64()-postTime;

	UINT64 gdiTime = GetTimeMs64();
	renderTextOverlays(renderChain);
	gdiTimes[curTimeIdx] = GetTimeMs64()-gdiTime;

	UINT64 endTime = GetTimeMs64();
	endRender(renderChain);
	endTimes[curTimeIdx] = GetTimeMs64()-endTime;

	frameTimes[curTimeIdx] = GetTimeMs64()-frameTime;
	++curTimeIdx;
	if ( curTimeIdx >= NUM_TIMES )
		curTimeIdx = 0;
}

void Renderer::attachToRootScene(SceneNode* sceneIn, Section section,int frame)
{
	sceneIn->attachToParentNode(rootScene->getRenderSectionNode(section,frame));
}

void Renderer::removeSceneObjects(GraphicObjectTypes type)
{
	rootScene->removeRegisteredObjects(type);
}

GraphicObjectNode* Renderer::findSceneObject(GraphicObjectTypes type, int index)
{
	std::map<int, GraphicObjectNode*>& sceneObjects = rootScene->sceneObjectRegistry(type);

	if ( sceneObjects.count(index) == 0 )
		return NULL;

	return sceneObjects[index];
}

std::map<int, GraphicObjectNode*>& Renderer::allSceneObjects(GraphicObjectTypes type)
{
	return rootScene->sceneObjectRegistry(type);
}


void Renderer::attachTargets(TargetChains chain, RenderTargetTypes rt, DepthTargetTypes dt)
{
	int numRT = 0;
	ID3D11RenderTargetView*	renderTarget = NULL;
	ID3D11DepthStencilView*	depthTarget = NULL;
	if ( rt > RenderTargetTypes::NoneRT )
	{
		numRT = 1;
		renderTarget = renderChains[chain][rt]->getRenderTarget();
	}
	
	if ( dt > DepthTargetTypes::NoneDT )
		depthTarget = depthChains[chain][dt]->getDepthTarget();

	renderContext->OMSetRenderTargets(numRT, &renderTarget, depthTarget);
}

void Renderer::detachTargets()
{
	renderContext->OMSetRenderTargets(0, NULL, NULL);
}

void Renderer::clearRenderTarget(TargetChains chain, RenderTargetTypes rt, Vec<float> clearColor)
{
	ID3D11RenderTargetView*	renderTarget = renderChains[chain][rt]->getRenderTarget();
	float color[4] = {clearColor.x, clearColor.y, clearColor.z, 1.0f};

	renderContext->ClearRenderTargetView(renderTarget, color);
}

void Renderer::clearDepthTarget(TargetChains chain, DepthTargetTypes dt, float clearDepth)
{
	ID3D11DepthStencilView*	depthTarget = depthChains[chain][dt]->getDepthTarget();

	renderContext->ClearDepthStencilView(depthTarget, D3D11_CLEAR_DEPTH, clearDepth, 0);
}


void Renderer::renderBackground(TargetChains chain)
{
	SceneNode* preRoot = rootScene->getRenderSectionNode(Renderer::Section::Pre, 0);
	if ( !preRoot )
		return;

	RenderFilter renderFilter(preRoot, GraphicObjectTypes::Border);

	GraphicObjectNode* node = renderFilter.first();
	for ( ; node != NULL; node = renderFilter.next() )
		renderNode(gCameraDefaultMesh, node);
}

void Renderer::renderPolygons(TargetChains chain)
{
	SceneNode* mainRoot = rootScene->getRenderSectionNode(Renderer::Section::Main, currentFrame);
	if ( !mainRoot )
		return;

	RenderFilter renderFilter(mainRoot, GraphicObjectTypes::Polygons);

	GraphicObjectNode* node = renderFilter.first();
	for ( ; node != NULL; node = renderFilter.next() )
		renderNode(gCameraDefaultMesh, node, FrontClipPos(), BackClipPos());
}

void Renderer::renderVolume(TargetChains chain)
{
	SceneNode* mainRoot = rootScene->getRenderSectionNode(Renderer::Section::Main, currentFrame);
	if ( !mainRoot )
		return;

	for ( int i = GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::NumGO; ++i )
	{
		RenderFilter filt(mainRoot, (GraphicObjectTypes)i);
		for (GraphicObjectNode* node = filt.first() ; node != NULL; node = filt.next() )
			renderNode(gCameraDefaultMesh, node, FrontClipPos(), BackClipPos());
	}
}

void Renderer::renderWidget(TargetChains chain)
{
	SceneNode* postRoot = rootScene->getRenderSectionNode(Renderer::Section::Post, 0);
	if ( !postRoot )
		return;

	RenderFilter renderFilter(postRoot, GraphicObjectTypes::Widget);

	GraphicObjectNode* node = renderFilter.first();
	for ( ; node != NULL; node = renderFilter.next() )
		renderNode(gCameraWidget, node);
}

void Renderer::renderTextOverlays(TargetChains chain)
{
	if ( labelsOn )
	{
		SceneNode* mainRoot = rootScene->getRenderSectionNode(Renderer::Section::Main, currentFrame);
		if ( !mainRoot )
			return;

		RenderFilter renderFilter(mainRoot, GraphicObjectTypes::Polygons);

		GraphicObjectNode* node = renderFilter.first();
		for ( ; node != NULL; node = renderFilter.next() )
			renderLabel(chain, gCameraDefaultMesh, node);
	}

	if ( scaleBarOn )
		renderScaleValue(chain, gCameraDefaultMesh);

	if ( frameNumOn )
		renderFrameNum(chain);

	if ( fpsOn )
		renderFPS(chain);

	textRenderer->render();
}

void Renderer::startRender(TargetChains chain)
{
	if ( lastChain != chain )
		resetViewPort(chain);

	// Set up the camera projections
	gCameraText->setViewportSize(viewportSize[chain]);
	gCameraWidget->setViewportSize(viewportSize[chain]);
	gCameraDefaultMesh->setViewportSize(viewportSize[chain]);

	attachTargets(chain, RenderTargetTypes::DefaultRT, DepthTargetTypes::DefaultDT);

	clearRenderTarget(chain, RenderTargetTypes::DefaultRT, backgroundColor);
	clearDepthTarget(chain, DepthTargetTypes::DefaultDT, 1.0f);
}

void Renderer::endRender(TargetChains chain)
{
	if ( chain == TargetChains::Screen )
		getSwapChain()->present(0,0);

	detachTargets();
}

HRESULT Renderer::compileVertexShader(const std::string& filename, const std::string& functionName, 
	const std::map<std::string,std::string>& variables, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
	ID3D11VertexShader** vertexShaderOut, ID3D11InputLayout** vertexLayoutOut)
{
	HRESULT hr = E_FAIL;
	ID3DBlob* shaderBlob = compileShaderFile(filename, functionName, "vs_4_0", variables);

	if ( shaderBlob == NULL )
	{
		return hr;
	}

	hr = renderDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, vertexShaderOut);
	if (FAILED(hr))
	{
		shaderBlob->Release();
		return hr;
	}

	hr = renderDevice->CreateInputLayout(layoutDesc.data(), layoutDesc.size(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), vertexLayoutOut);

	shaderBlob->Release();
	return hr;
}

HRESULT Renderer::compilePixelShader(const std::string& filename, const std::string& functionName, const std::map<std::string, std::string>& variables, ID3D11PixelShader** pixelShaderOut)
{
	HRESULT hr = E_FAIL;
	ID3DBlob* shaderBlob = compileShaderFile(filename, functionName, "ps_4_0", variables);

	if (shaderBlob == NULL )
	{
		return hr;
	}

	hr = renderDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, pixelShaderOut);

	shaderBlob->Release();
	return hr;
}

std::string Renderer::preprocessShader(const std::string& shader, const std::map<std::string, std::string>& repVars)
{
	std::string outShader = shader;

	// Match shader vars
	std::regex var_match("\\$(\\w+)");
	std::sregex_token_iterator regexMatcher(outShader.begin(), outShader.end(), var_match, 1);
	std::sregex_token_iterator doneMatch = std::sregex_token_iterator();

	std::set<std::string> shaderVars;
	for (std::sregex_token_iterator matches=regexMatcher; matches != doneMatch; ++matches)
	{
		std::string varName = matches->str();
		std::transform(varName.begin(), varName.end(), varName.begin(), ::toupper);

		shaderVars.insert(varName);
	}

	// Check for variable references missing definitions
	std::string missingDef;
	for (auto varIter = shaderVars.begin(); varIter != shaderVars.end(); ++varIter)
	{
		if (repVars.count(*varIter) > 0)
			continue;

		missingDef += *varIter + std::string(", ");
	}

	if (missingDef.size() > 0)
	{
		std::string err = std::string("Missing definition for ") + missingDef;
		std::runtime_error(err.c_str());

		return "";
	}

	// Replace each $ref with values
	for (auto varIter = shaderVars.begin(); varIter != shaderVars.end(); ++varIter)
	{
		std::string matchStr = std::string("(\\$") + *varIter + std::string(")");
		std::regex var_rep(matchStr, std::regex_constants::icase);

		outShader = std::regex_replace(outShader, var_rep, repVars.at(*varIter));
	}

	return outShader;
}

ID3DBlob* Renderer::compileShaderFile(const std::string& filename, const std::string& entryFunction, const std::string& shaderModel, const std::map<std::string, std::string>& repVars)
{
	// Just use a file stream to read the whole shader at once
	std::ifstream shaderFile(filename);
	std::string shader((std::istreambuf_iterator<char>(shaderFile)), (std::istreambuf_iterator<char>()));

	// Replace any $vars that the material has passed down for this shader
	std::string finalShader = preprocessShader(shader, repVars);

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* errBlob = NULL;
	ID3DBlob* shaderBlob = NULL;
	HRESULT hr = D3DCompile(finalShader.c_str(), finalShader.length(), NULL, NULL, NULL, entryFunction.c_str(), shaderModel.c_str(), dwShaderFlags, 0, &shaderBlob, &errBlob);

	if (FAILED(hr))
	{
		if (errBlob != NULL)
		{
			std::string errTitle = "Shader Compile Error: " + filename;
			MessageBoxA(NULL, (LPCSTR)errBlob->GetBufferPointer(), errTitle.c_str(), MB_OK | MB_ICONERROR);
		}

		SAFE_RELEASE(errBlob);

		return NULL;
	}

	SAFE_RELEASE(errBlob);

	return shaderBlob;
}

void Renderer::renderNode(const Camera* camera, const GraphicObjectNode* node, float frontClip, float backClip)
{
	static ID3D11VertexShader* previousVertexShader = NULL;
	static ID3D11PixelShader* previousPixelShader = NULL;

	const VertexShaderEntry& vsEntry = vertexShaderRegistry[node->mesh->vertShaderIdx];
	const PixelShaderEntry& psEntry = pixelShaderRegistry[node->material->shaderIdx];

	ID3D11InputLayout* vertLayout = vsEntry.layout;
	ID3D11VertexShader* vertShader = vsEntry.shader;
	ID3D11PixelShader* pixShader = psEntry.shader;

	// Don't try to use a good pixel-shader with a bad vertex shader
	if ( vsEntry.error )
		pixShader = fallbackPS;

	//Vertex Shader setup
	VertexShaderConstBuffer vcb;

	vcb.projectionTransform = DirectX::XMMatrixTranspose(camera->getProjectionTransform());
	vcb.viewTransform = DirectX::XMMatrixTranspose(camera->getViewTransform());
	vcb.worldTransform = DirectX::XMMatrixTranspose(node->getLocalToWorld());
	vcb.depthPeelPlanes.x = frontClip;
	vcb.depthPeelPlanes.y = backClip;
	updateShaderParams(&vcb,vertexShaderConstBuffer);

	if ( previousVertexShader != vertShader )
	{
		setVertexShader(vertShader, vertLayout);
		previousVertexShader = vertShader;
	}

	setGeometry(node->mesh->layout, node->mesh->vertexBuffer, node->mesh->indexBuffer);


	// Material and pixel shader setup

	//Pixel Shader setup
	node->material->updateTransformParams(node->getLocalToWorldTransform(), camera->getViewTransform(), camera->getProjectionTransform());
	node->material->getParams()->updateParams(); //can be sped up by doing this differently
	
	node->material->bindTextures();
	node->material->bindConstants(); //TODO this needs tweeking
	
	if (previousPixelShader != pixShader)
	{
		setPixelShader(pixShader);
		previousPixelShader = pixShader;
	}
	
	setRasterizerState(node->material->rasterState);
	setDepthStencilState(node->material->depthStencilState);
	
	drawTriangles(node->mesh->numFaces);
}


void Renderer::renderLabel(TargetChains chain, const Camera* camera, const GraphicObjectNode* node)
{
	if ( node->getLabel().length() == 0 )
		return;

	DirectX::XMVECTOR v2D={0.,0.,0.,0.};
	Vec<int> pos(0,0,0);

	DirectX::XMFLOAT4 boxColor = node->material->getColor();
	Color bgColor(boxColor.x, boxColor.y, boxColor.z, 1.0f);
	Color fgColor(1.0f,1.0f,1.0f,1.0f);

	if ( bgColor.r + bgColor.g + bgColor.b > 1.5f )
		fgColor = Color(0.0f,0.0f,0.0f,1.0f);

	Vec<float> centerOfmassVec = node->mesh->getCenterOfMass();
	DirectX::XMFLOAT3 centerOfmass(centerOfmassVec.x,centerOfmassVec.y,centerOfmassVec.z);
	DirectX::XMVECTOR com = DirectX::XMLoadFloat3(&centerOfmass);

	const Vec<int>& size = viewportSize[chain];

	v2D = DirectX::XMVector3Project(com,0.0f,0.0f,(float)size.x,(float)size.y,0.0f,1.0f,
		camera->getProjectionTransform(),camera->getViewTransform(),node->getLocalToWorld());

	pos.x = (int)DirectX::XMVectorGetX(v2D);
	pos.y = (int)DirectX::XMVectorGetY(v2D);

	textRenderer->drawString(node->getLabel(), pos, fgColor, bgColor);
}

float clamp(float x, float minVal, float maxVal)
{
	return (x > minVal) ? ((x < maxVal) ? (x) : (maxVal)) : (minVal);
}

void Renderer::renderScaleValue(TargetChains chain, const Camera* camera)
{
	if ( !volInfo )
		return;

	Vec<int>& viewSize = viewportSize[chain];

	float sz = camera->getVolUnitsPerPix();

	Vec<size_t> dims = volInfo->getDims();
	Vec<float> volSize = volInfo->getPhysSize();
	sz *= volSize.maxValue()/2.0f;

	const float maxBar = 120.0f;
	const float bestBar = 100.0f;
	const float minBar = 40.0f;

	const float checkUnits[] = {1.0f,2.0f,3.0f,4.0f,5.0f};
	const int numCheck = ARRAY_SIZE(checkUnits);

	float maxUnits = maxBar * sz;
	float bestUnits = bestBar * sz;
	float minUnits = minBar * sz;

	const float infty = std::numeric_limits<float>::infinity();

	int minPow = (int)floor(log10f(minUnits));
	int maxPow = (int)floor(log10f(maxUnits));

	float bestDist = infty;
	float closestUnits = 0.0f;
	for ( int i=minPow; i <= maxPow; ++i )
	{
		for ( int j=0; j < numCheck; ++j )
		{
			float units = checkUnits[j] * pow(10.0f,i);
			float dist = abs(units - bestUnits);

			if ( dist < bestDist )
			{
				closestUnits = units;
				bestDist = dist;
			}
		}
	}

	if(scaleTextOn)
	{
		char buff[36];
		if( closestUnits >= 1.0f )
			sprintf(buff, "%.0f%cm", closestUnits, 0xb5);
		else if( closestUnits >= 0.1f)
			sprintf(buff, "%.1f%cm", closestUnits, 0xb5);
		else
			sprintf(buff, "%.2f%cm", closestUnits, 0xb5);

		Vec<int> textPos(viewSize.x-100, viewSize.y-25,0);
		textRenderer->drawString(buff, textPos);
	}

	int barWidth = round(closestUnits / sz);

	Vec<int> tl(viewSize.x-85-barWidth/2, viewSize.y-35, 0);
	Vec<int> size(barWidth, 10, 0);

	textRenderer->drawRect(tl, size, Color(1.0f,1.0f,1.0f,1.0f));
}

void Renderer::renderFrameNum(TargetChains chain)
{
	char buff[36];
	sprintf(buff, "Frame:%d", currentFrame+1);

	Vec<int>& viewSize = viewportSize[chain];
	Vec<int> screenPos(viewSize.x-120, 10, 0);
	textRenderer->drawString(buff, screenPos);
}

void Renderer::renderFPS(TargetChains chain)
{
	UINT64 totalTimes = 0;
	UINT64 startTime  = 0;
	UINT64 preTime	  = 0;
	UINT64 mainTime   = 0;
	UINT64 postTime  = 0;
	UINT64 gdiTime   = 0;
	UINT64 endTime   = 0;

	double avgFrame = 0;
	double avgStart = 0;
	double avgPre = 0;
	double avgMain = 0;
	double avgPost = 0;
	double avgGdi = 0;
	double avgEnd = 0;

	for(int i = 0; i<NUM_TIMES; ++i)
		totalTimes += frameTimes[i];

	avgFrame = double(totalTimes)/NUM_TIMES;

	if(statsOn)
	{
		for(int i = 0; i<NUM_TIMES; ++i)
		{
			startTime += startTimes[i];
			preTime += preTimes[i];
			mainTime += mainTimes[i];
			postTime += postTimes[i];
			gdiTime += gdiTimes[i];
			endTime += endTimes[i];
		}

		avgStart = double(startTime)/NUM_TIMES;
		avgPre = double(preTime)/NUM_TIMES;
		avgMain = double(mainTime)/NUM_TIMES;
		avgPost = double(postTime)/NUM_TIMES;
		avgGdi = double(gdiTime)/NUM_TIMES;
		avgEnd = double(endTime)/NUM_TIMES;
	}

	char buff[36];
	double fps = 1000.0/avgFrame;
	if (0.01<fps)
		sprintf(buff, "FPS:%0.3f", fps);
	else if(0.1<fps)
		sprintf(buff, "FPS:%0.2f", fps);
	else if(10.0<fps)
		sprintf(buff, "FPS:%0.1f", fps);
	else
		sprintf(buff, "FPS:%.0f", fps);

	textRenderer->drawString(buff, Vec<int>(20,10,0));

	if(statsOn)
	{
		sprintf(buff, "Start:   %.3fms", avgStart);
		textRenderer->drawString(buff, Vec<int>(20, 40, 0));

		sprintf(buff, "Pre:     %.3fms", avgPre);
		textRenderer->drawString(buff, Vec<int>(20, 60, 0));

		sprintf(buff, "Main:    %.3fms", avgMain);
		textRenderer->drawString(buff, Vec<int>(20, 80, 0));

		sprintf(buff, "Post:    %.3fms", avgPost);
		textRenderer->drawString(buff, Vec<int>(20, 100, 0));

		sprintf(buff, "Overlay: %.3fms", avgGdi);
		textRenderer->drawString(buff, Vec<int>(20, 120, 0));

		sprintf(buff, "Full:    %.3fms", avgFrame);
		textRenderer->drawString(buff, Vec<int>(20, 140, 0));

		sprintf(buff, "Present: %.3fms", avgEnd);
		textRenderer->drawString(buff, Vec<int>(20, 165, 0));
	}
}

const SwapChainTarget* Renderer::getSwapChain() const
{
	return static_cast<SwapChainTarget*>(renderChains[TargetChains::Screen][RenderTargetTypes::DefaultRT].get());
}

void Renderer::setVertexShader(ID3D11VertexShader* shader, ID3D11InputLayout* layout)
{
	renderContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	renderContext->VSSetShader(shader,NULL,0);
	renderContext->IASetInputLayout(layout);
	renderContext->VSSetConstantBuffers(0,1,&vertexShaderConstBuffer);
}

void Renderer::setRasterizerState(ID3D11RasterizerState* rasterState)
{
	renderContext->RSSetState(rasterState);
}

void Renderer::setPixelShader(ID3D11PixelShader* shader)
{
	renderContext->PSSetShader(shader,NULL,0);
}

void Renderer::setDepthStencilState(ID3D11DepthStencilState* depthStencilState)
{
	renderContext->OMSetDepthStencilState(depthStencilState,NULL);
}

void Renderer::setGeometry(VertexLayout layout, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer)
{
	UINT stride = layout.getVertSize();
	UINT offset = 0;

	renderContext->IASetIndexBuffer(indexBuffer,DXGI_FORMAT_R32_UINT,0);
	renderContext->IASetVertexBuffers(0,1,&vertexBuffer,&stride,&offset);
}

void Renderer::drawTriangles(size_t numFaces)
{
	renderContext->DrawIndexed(unsigned int(3*numFaces),0,0);
}

void Renderer::setPixelShaderConsts(ID3D11Buffer* buffer)
{
	renderContext->PSSetConstantBuffers(1,1,&buffer);
}

IDXGISwapChain1* Renderer::createSwapChain(HWND hWnd, Vec<size_t> dims, bool stereo, DXGI_FORMAT format, UINT flags)
{
	//Swap chain descriptor
	DXGI_SWAP_CHAIN_DESC1 swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));

	swapDesc.Width = (UINT) dims.x;
	swapDesc.Height = (UINT) dims.y;
	swapDesc.Format = format;

	swapDesc.Stereo = stereo;

	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapDesc.BufferCount = 1;
	swapDesc.Scaling = DXGI_SCALING_STRETCH;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// TODO: Which alpha mode is desired?
	swapDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	// TODO: We should look into GDI alternatives, and may not need mode-switch capability
	swapDesc.Flags = flags;

	IDXGISwapChain1* swapChain;
	HRESULT hr = renderFactory->CreateSwapChainForHwnd(renderDevice, hWnd, &swapDesc, NULL, NULL, &swapChain);
	if ( FAILED(hr) )
		sendHrErrMessage(hr);

	return swapChain;
}

ID3D11SamplerState* Renderer::createSamplerState(InterpTypes interpType)
{
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	if ( interpType == InterpTypes::Linear )
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MipLODBias = 0.0f;

	ID3D11SamplerState* sampler;
	HRESULT hr = renderDevice->CreateSamplerState(&samplerDesc, &sampler);
	if (FAILED(hr))
		sendHrErrMessage(hr);

	return sampler;
}

ID3D11ShaderResourceView* Renderer::createShaderResourceView(ID3D11Resource* textureResource)
{
	ID3D11ShaderResourceView* resourceView;
	HRESULT hr = renderDevice->CreateShaderResourceView(textureResource, NULL, &resourceView);
	if( FAILED( hr ))
		sendHrErrMessage(hr);

	return resourceView;
}

ID3D11RenderTargetView* Renderer::createRenderTargetView(ID3D11Resource* textureResource)
{
	ID3D11RenderTargetView* targetView;
	HRESULT hr = renderDevice->CreateRenderTargetView(textureResource, NULL, &targetView);
	if( FAILED( hr ))
		sendHrErrMessage(hr);

	return targetView;
}

ID3D11DepthStencilView* Renderer::createDepthTargetView(ID3D11Resource* textureResource)
{
	ID3D11DepthStencilView* depthView;
	HRESULT hr = renderDevice->CreateDepthStencilView(textureResource, NULL, &depthView);
	if( FAILED( hr ) )
		sendHrErrMessage(hr);

	return depthView;
}

ID3D11Texture2D* Renderer::createTexture2D(D3D11_TEXTURE2D_DESC* desc, D3D11_SUBRESOURCE_DATA* initData)
{
	ID3D11Texture2D* texture2D = NULL;

	HRESULT hr = renderDevice->CreateTexture2D(desc, initData, &texture2D );
	if( FAILED( hr ))
		sendHrErrMessage(hr);

	return texture2D;
}

ID3D11Texture3D* Renderer::createTexture3D(D3D11_TEXTURE3D_DESC* desc, D3D11_SUBRESOURCE_DATA* initData)
{
	ID3D11Texture3D* texture3D = NULL;

	HRESULT hr = renderDevice->CreateTexture3D(desc, initData, &texture3D );
	if( FAILED( hr ))
		sendHrErrMessage(hr);

	return texture3D;
}


void Renderer::stageResource(ID3D11Texture2D* stageTexture, ID3D11Texture2D* renderTexture)
{
	renderContext->CopyResource(stageTexture, renderTexture);
}

void Renderer::readSurface(unsigned char* outBuffer, ID3D11Texture2D* texture, Vec<size_t> dims, size_t pixelSize)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	renderContext->Map(texture, 0, D3D11_MAP_READ, 0, &resource);

	size_t outPitch = pixelSize * dims.x;

	unsigned char* outLine = outBuffer;
	unsigned char* inLine = (unsigned char*) resource.pData;
	for ( int i=0; i < dims.y; ++i )
	{
		memcpy(outLine, inLine, outPitch);
		inLine += resource.RowPitch;
		outLine += outPitch;
	}

	renderContext->Unmap(texture, 0);
}



ID3D11RasterizerState* Renderer::getRasterizerState(bool wireframe, D3D11_CULL_MODE cullFaces)
{
	unsigned int rasterFlags = (((unsigned int)wireframe) << 2) |  ((unsigned int) cullFaces);

	if ( rasterStates.count(rasterFlags) > 0 )
		return rasterStates[rasterFlags];

	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState* rasterState;

	ZeroMemory(&rasterDesc,sizeof(rasterDesc));

	if ( wireframe )
		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	else
		rasterDesc.FillMode = D3D11_FILL_SOLID;

	rasterDesc.CullMode = cullFaces;

	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FrontCounterClockwise = TRUE;
	rasterDesc.MultisampleEnable = FALSE;

	renderDevice->CreateRasterizerState(&rasterDesc,&rasterState);

	rasterStates[rasterFlags] = rasterState;
	return rasterStates[rasterFlags];
}

ID3D11DepthStencilState* Renderer::getDepthStencilState(bool depthTest)
{
	unsigned int dsFlags = ((unsigned int)depthTest);

	if ( depthStencilStates.count(dsFlags) > 0 )
		return depthStencilStates[dsFlags];

	ID3D11DepthStencilState* depthStencilState;
	D3D11_DEPTH_STENCIL_DESC descDS;

	ZeroMemory(&descDS,sizeof(descDS));

	if ( depthTest )
	{
		descDS.DepthEnable = TRUE;
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		descDS.StencilEnable = FALSE;
	}
	else
	{
		descDS.DepthEnable = TRUE;
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		descDS.StencilEnable = FALSE;
	}

	renderDevice->CreateDepthStencilState(&descDS, &depthStencilState);

	depthStencilStates[dsFlags] = depthStencilState;
	return depthStencilStates[dsFlags];
}

void Renderer::createBlendState()
{
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	// 	BlendState.RenderTarget[0].BlendEnable = TRUE;
	// 	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	// 	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	// 	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	// 	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	// 	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	// 	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	// 	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	BlendState.RenderTarget[0].BlendEnable=TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	renderDevice->CreateBlendState(&BlendState, &blendState);
	renderContext->OMSetBlendState(blendState, 0, 0xffffffff);
}

void Renderer::setPixelShaderResourceViews(int startIdx, int length, ID3D11ShaderResourceView** shaderResourceView)
{
	renderContext->PSSetShaderResources(startIdx,length,shaderResourceView);
}

void Renderer::setPixelShaderTextureSamplers(int startIdx, int length, ID3D11SamplerState** samplerState)
{
	renderContext->PSSetSamplers(startIdx,length,samplerState);
}

void Renderer::initVolumeInfo(int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physSize, bool columnMajor)
{
	SAFE_DELETE(volInfo);

	rootScene->initRenderSectionNodes(Section::Main, numFrames);
	volInfo = new VolumeInfo(this, numFrames, numChannels, dims, physSize, columnMajor);
}

DirectX::XMMATRIX Renderer::getRootWorldRotation()
{
	return rootScene->getWorldRotation();
}

int Renderer::getPolygon(Vec<float> pnt, Vec<float> direction)
{
	//WaitForSingleObject(mutexDevice,INFINITE);
	float depth;
	SceneNode* node = rootScene->pickNode(pnt, direction, currentFrame, GraphicObjectTypes::Polygons, depth);
	//ReleaseMutex(mutexDevice);

	if ( !node )
		return -1;

	return node->getIndex();
}

void Renderer::resizeViewPort(Vec<int> sizeIn, TargetChains selectChain)
{
	viewportSize[selectChain] = sizeIn;

	for ( int i=0; i < RenderTargetTypes::NumRT; ++i )
		renderChains[selectChain][i]->resizeTarget(viewportSize[selectChain].x, viewportSize[selectChain].y);

	for ( int i=0; i < DepthTargetTypes::NumDT; ++i )
		depthChains[selectChain][i]->resizeDepth(viewportSize[selectChain].x, viewportSize[selectChain].y);

	resetViewPort(selectChain);
}

void Renderer::flushContext()
{
	renderContext->ClearState();
	renderContext->Flush();
}

HRESULT Renderer::resetViewPort(TargetChains selectChain)
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)viewportSize[selectChain].x;
	vp.Height = (float)viewportSize[selectChain].y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	renderContext->RSSetViewports( 1, &vp );

	lastChain = selectChain;

	return S_OK;
}

void Renderer::setCurrentFrame(int frame)
{
	if (frame>rootScene->getNumFrames())
		frame = rootScene->getNumFrames()-1;

	if (frame<0)
		frame = 0;

	currentFrame = frame;

}

void Renderer::incrementFrame()
{
	++currentFrame;
	if (currentFrame > unsigned int(rootScene->getNumFrames() - 1))
	{
		currentFrame = 0;
		if (gCapture)
		{
			gCapture = false;
			gPlay = false;
		}
	}

	gMsgQueueToMex.addMessage("timeChange",currentFrame+1);
}

void Renderer::decrementFrame()
{
	if (currentFrame!=0)
		--currentFrame;

	gMsgQueueToMex.addMessage("timeChange",currentFrame+1);
}

unsigned int Renderer::getNumberOfFrames()
{
	return rootScene->getNumFrames();
}

void Renderer::resetRootWorldTransform()
{
	rootScene->resetWorldTransform();
}

void Renderer::setClipChunkPercent(float ccp)
{
	clipChunkPercent = ccp;
}

void Renderer::setNumPlanes(int numPlanesIn)
{
	if (numPlanes < numPlanesIn)
		numPlanes = numPlanesIn;
}

void Renderer::setWorldOrigin(Vec<float> org)
{
	rootScene->updateTranslation(org);
}

void Renderer::setWorldRotation(DirectX::XMMATRIX rotation)
{
	rootScene->updateRotation(rotation);
}


unsigned char* Renderer::captureWindow(Vec<size_t>& dims)
{
	// Run a full render to capture render target chain
	renderAll(TargetChains::Capture);

	std::shared_ptr<ReadbackRenderTarget> readback = std::static_pointer_cast<ReadbackRenderTarget>(renderChains[TargetChains::Capture][RenderTargetTypes::DefaultRT]);

	dims = readback->getDims();
	return readback->capture();
}

HRESULT Renderer::captureWindow(std::string* filenameOut)
{
	// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
	// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
	// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	// PARTICULAR PURPOSE.
	//
	// Copyright (c) Microsoft Corporation. All rights reserved

	//
	//   FUNCTION: CaptureAnImage(HWND hWnd)
	//
	//   PURPOSE: Captures a screenshot into a window and then saves it in a .bmp file.
	//
	//   COMMENTS: 
	//
	//      Note: This sample will attempt to create a file called captureqwsx.bmp 
	//        
	//     
	static int nt = 0;
	//HDC hdcScreen;

	CHAR wsz[1024];
	DWORD dwError;
	HRESULT hr = S_FALSE;

	nt++;

	memset(wsz, 0, 255 * sizeof(CHAR));
	sprintf_s(wsz, "%s\\%s_%05d.bmp", captureFilePath.c_str(),captureFileName.c_str(), nt);

	if (filenameOut!=NULL)
		*filenameOut = wsz;

	DWORD dwBmpSize = 0;
	BITMAPINFOHEADER bi;
	unsigned char* lpbitmap = captureWindow(dwBmpSize,bi);


	// A file is created, this is where we will save the screen capture.
	HANDLE hFile = CreateFileA(wsz,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		dwError = GetLastError();
		return hr;
	}


	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	BITMAPFILEHEADER   bmfHeader;
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Close the handle for the file that was created
	CloseHandle(hFile);

	delete[] lpbitmap;

	return hr;
}

HRESULT Renderer::captureWindow(std::string filePathIn, std::string fileNameIn, std::string& filenameOut)
{
	std::string oldCaptureFilePath = captureFilePath;
	std::string oldCaptureFileName = captureFileName;

	captureFilePath = filePathIn;
	captureFileName = fileNameIn;

	HRESULT hr;
	hr = captureWindow(&filenameOut);

	captureFilePath = oldCaptureFilePath;
	captureFileName = oldCaptureFileName;

	return hr;
}

unsigned char* Renderer::captureWindow(DWORD& dwBmpSize, BITMAPINFOHEADER& bi)
{
	HRESULT hr = E_FAIL;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	unsigned char* lpbitmap = NULL;

	dwBmpSize = 0;

	// Retrieve the handle to a display device context for the client 
	// area of the window. 
	renderContext->OMSetRenderTargets(0,0,0);

	HDC hdcWindow = getSwapChain()->getDC();

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMemDC = CreateCompatibleDC(hdcWindow);

	if(!hdcMemDC)
	{
		MessageBox(gWindowHandle,"CreateCompatibleDC has failed","Failed",MB_OK);
		return NULL;
	}

	// Get the client area for size calculation
	RECT rcClient;
	GetClientRect(gWindowHandle,&rcClient);

	//This is the best stretch mode
	SetStretchBltMode(hdcWindow,HALFTONE);

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcWindow,rcClient.right - rcClient.left,rcClient.bottom - rcClient.top);

	if(!hbmScreen)
	{
		MessageBox(gWindowHandle,"CreateCompatibleBitmap Failed","Failed",MB_OK);
		return NULL;
	}

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMemDC,hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if(!BitBlt(hdcMemDC,
		0,0,
		rcClient.right - rcClient.left,rcClient.bottom - rcClient.top,
		hdcWindow,
		0,0,
		SRCCOPY))
	{
		MessageBox(gWindowHandle,"BitBlt has failed","Failed",MB_OK);

		DeleteObject(hbmScreen);

		return NULL;
	}

	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen,sizeof(BITMAP),&bmpScreen);

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize);
	lpbitmap = (unsigned char *)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcWindow,hbmScreen,0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi,DIB_RGB_COLORS);

	unsigned char* imOut = new unsigned char[dwBmpSize];
	memcpy(imOut,lpbitmap,sizeof(char)*dwBmpSize);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	DeleteObject(hbmScreen);

	bool bSuccess = DeleteDC(hdcMemDC);
	if(!bSuccess)
	{
		MessageBox(gWindowHandle, "Delete DC Failed", "Failed", MB_OK);
	}

	getSwapChain()->releaseDC();

	return imOut;
}
