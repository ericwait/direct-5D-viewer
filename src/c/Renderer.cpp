#include "Renderer.h"
#include "Globals.h"
#include "Defines.h"
#include <d3dcompiler.h>
#include "MeshPrimitive.h"
#include "Camera.h"
#include "RendererPackage.h"

Renderer::Renderer()
{
	mutexDevice = CreateMutex(NULL,FALSE,NULL);

	swapChain = NULL;
	d3dDevice = NULL;
	immediateContext = NULL;
	renderTargetView = NULL;
	IDXGIBackBuffer= NULL;

	depthStencilView = NULL;
	depthStencilStateCompareAlways = NULL;
	depthStencilStateCompareLess = NULL;

	rasterizerStateNoClip = NULL;
	rasterizerStateWireClip = NULL;
	rasterizerStateFillClip = NULL;

	linearTextureSampler = NULL;

	backgroundColor = Vec<float>(0.25f, 0.25f, 0.25f);
	currentFrame = 0;
	clipChunkPercent = 0.0f;
	numPlanes = 0;
}

Renderer::~Renderer()
{
	DWORD waitTerm = WaitForSingleObject(mutexDevice,5000);

	SAFE_DELETE(rootScene);

	clearVertexShaderList();
	clearPixelShaderList();

	SAFE_RELEASE(vertexShaderConstBuffer);
	SAFE_RELEASE(linearTextureSampler);
	SAFE_RELEASE(blendState);

	releaseRasterizerStates();
	releaseRenderTarget();
	releaseDepthStencils();
	releaseSwapChain();

	CloseHandle(mutexDevice);
}

HRESULT Renderer::init()
{
	WaitForSingleObject(mutexDevice,INFINITE);

	HRESULT hr = initSwapChain();
	if (FAILED(hr))
	{
		ReleaseMutex(mutexDevice);
		return hr;
	}

	hr = initDepthStencils();
	if (FAILED(hr))
	{
		ReleaseMutex(mutexDevice);
		return hr;
	}

	hr = initRenderTarget();
	if (FAILED(hr))
	{
		ReleaseMutex(mutexDevice);
		return hr;
	}

	hr = initRasterizerStates();
	if (FAILED(hr))
	{
		ReleaseMutex(mutexDevice);
		return hr;
	}

	createBlendState();

	hr = createConstantBuffer(sizeof(VertexShaderConstBuffer),&vertexShaderConstBuffer);
	if (FAILED(hr))
	{
		ReleaseMutex(mutexDevice);
		return hr;
	}

	rootScene = new RootSceneNode();

	resetRootWorldTransform();

	ReleaseMutex(mutexDevice);

	return hr;
}

HRESULT Renderer::initSwapChain()
{
	WaitForSingleObject(mutexDevice,INFINITE);

	HRESULT hr = S_FALSE;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =	{D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =	{D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//Swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = gWindowWidth;
	sd.BufferDesc.Height = gWindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = gWindowHandle;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.Flags= DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	for(UINT driverTypeIndex=0; driverTypeIndex<numDriverTypes; ++driverTypeIndex)
	{
		D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
		for (UINT featureLevelTypeIndex=0; featureLevelTypeIndex<numFeatureLevels; ++featureLevelTypeIndex)
		{
			D3D_FEATURE_LEVEL featureLevel = featureLevels[featureLevelTypeIndex];

			hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &sd, &swapChain, &d3dDevice, &featureLevel, &immediateContext);

			if( SUCCEEDED(hr) )
				break;
		}

		if( SUCCEEDED(hr) )
			break;
	}

	if (FAILED(hr))
		return hr;

	D3D11_VIEWPORT vp;
	vp.Width = (float)gWindowWidth;
	vp.Height = (float)gWindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediateContext->RSSetViewports( 1, &vp );

	ReleaseMutex(mutexDevice);

	return hr;
}

void Renderer::releaseSwapChain()
{
	SAFE_RELEASE(swapChain);
	SAFE_RELEASE(d3dDevice);
	SAFE_RELEASE(immediateContext);
}

HRESULT Renderer::initDepthStencils()
{
	HRESULT hr = S_FALSE;

	DWORD waitResult = WaitForSingleObject(mutexDevice, INFINITE);
	if ( waitResult != WAIT_OBJECT_0 )
		return S_FALSE;

	// Create depth stencil texture
	ID3D11Texture2D* depthStencil = NULL;

	D3D11_TEXTURE2D_DESC descDepth;

	descDepth.Width = gWindowWidth;
	descDepth.Height = gWindowHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	hr = d3dDevice->CreateTexture2D(&descDepth, NULL, &depthStencil);
	if( FAILED(hr) )
		return hr;

	D3D11_DEPTH_STENCIL_DESC descDS;

	ZeroMemory(&descDS,sizeof(descDS));
	descDS.DepthEnable = TRUE;
	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	descDS.StencilEnable = FALSE;

	hr = d3dDevice->CreateDepthStencilState(&descDS,&depthStencilStateCompareLess);
	if( FAILED( hr ) )
		return hr;

	//Compare always depth stencil
	ZeroMemory(&descDS,sizeof(descDS));
	descDS.DepthEnable=TRUE;
	descDS.DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
	descDS.DepthFunc=D3D11_COMPARISON_ALWAYS;
	descDS.StencilEnable=FALSE;

	hr = d3dDevice->CreateDepthStencilState(&descDS,&depthStencilStateCompareAlways);
	if( FAILED( hr ) )
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;

	descDSV.Format = descDepth.Format;
	descDSV.Flags = 0;

	if( descDepth.SampleDesc.Count > 1 )
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	else
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	descDSV.Texture2D.MipSlice = 0;

	hr = d3dDevice->CreateDepthStencilView(depthStencil, &descDSV, &depthStencilView);
	if( FAILED( hr ) )
		return hr;

	depthStencil->Release();

	ReleaseMutex(mutexDevice);

	return S_OK;
}

void Renderer::releaseDepthStencils()
{
	SAFE_RELEASE(depthStencilStateCompareLess);
	SAFE_RELEASE(depthStencilStateCompareAlways);
	SAFE_RELEASE(depthStencilView);
}

HRESULT Renderer::initRenderTarget()
{
	HRESULT hr;

	// Create a render target view
	ID3D11Texture2D* backBuffer = NULL;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if( FAILED(hr) )
		return hr;

	hr = backBuffer->QueryInterface(__uuidof( IDXGISurface1) ,(LPVOID*)&IDXGIBackBuffer);
	if( FAILED(hr) )
		return hr;

	DWORD waitResult = WaitForSingleObject(mutexDevice, INFINITE);
	if ( waitResult != WAIT_OBJECT_0 )
		return S_FALSE;

	hr = d3dDevice->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);

	ReleaseMutex(mutexDevice);

	backBuffer->Release();

	if( FAILED(hr) )
		return hr;

	immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	return S_OK;
}

void Renderer::releaseRenderTarget()
{
	SAFE_RELEASE(IDXGIBackBuffer);
	SAFE_RELEASE(renderTargetView);
}

HRESULT Renderer::initRasterizerStates()
{
	D3D11_RASTERIZER_DESC d3d11rd;

	DWORD waitResult = WaitForSingleObject(mutexDevice, INFINITE);
	if ( waitResult != WAIT_OBJECT_0 )
		return S_FALSE;

	memset(&d3d11rd,0,sizeof(d3d11rd));
	d3d11rd.FillMode=D3D11_FILL_SOLID;
	d3d11rd.CullMode=D3D11_CULL_NONE;
	//d3d11rd.CullMode=D3D11_CULL_FRONT;
	d3d11rd.DepthClipEnable=FALSE;
	d3d11rd.MultisampleEnable=FALSE;
	d3dDevice->CreateRasterizerState(&d3d11rd,&rasterizerStateNoClip);

	memset(&d3d11rd,0,sizeof(d3d11rd));
	d3d11rd.FillMode=D3D11_FILL_WIREFRAME;
	d3d11rd.CullMode=D3D11_CULL_NONE;
	//d3d11rd.CullMode=D3D11_CULL_FRONT;
	d3d11rd.DepthClipEnable=TRUE;
	d3d11rd.MultisampleEnable=FALSE;
	d3d11rd.AntialiasedLineEnable=TRUE;
	d3dDevice->CreateRasterizerState(&d3d11rd,&rasterizerStateWireClip);

	memset(&d3d11rd,0,sizeof(d3d11rd));
	d3d11rd.FillMode=D3D11_FILL_SOLID;
	d3d11rd.CullMode=D3D11_CULL_NONE;
	//d3d11rd.CullMode=D3D11_CULL_BACK;
	d3d11rd.DepthClipEnable=TRUE;
	d3d11rd.MultisampleEnable=FALSE;
	d3d11rd.AntialiasedLineEnable=TRUE;
	d3dDevice->CreateRasterizerState(&d3d11rd,&rasterizerStateFillClip);

	ReleaseMutex(mutexDevice);

	return S_OK;
}

void Renderer::releaseRasterizerStates()
{
	SAFE_RELEASE(rasterizerStateNoClip);
	SAFE_RELEASE(rasterizerStateWireClip);
	SAFE_RELEASE(rasterizerStateFillClip);
}

HRESULT CompileShaderFromFile( LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;

	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
			MessageBoxA(NULL, (LPCSTR)pErrorBlob->GetBufferPointer(), "Shader Compile Error", MB_OK | MB_ICONERROR);

		SAFE_RELEASE(pErrorBlob);

		return hr;
	}

	SAFE_RELEASE(pErrorBlob);

	return S_OK;
}

HRESULT Renderer::compileVertexShader(const wchar_t* fileName, const char* shaderFunctionName, ID3D11VertexShader** vertexShaderOut, ID3D11InputLayout** vertexLayoutOut)
{
	ID3DBlob* shaderBlob = NULL;

	HRESULT hr = CompileShaderFromFile(fileName, shaderFunctionName,"vs_4_0", &shaderBlob);

	if( FAILED( hr ) )
	{
		MessageBox(NULL, TEXT("The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file."), TEXT("Error"), MB_OK);
		return hr;
	}

	hr = d3dDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, vertexShaderOut);

	if( FAILED( hr ) )
	{	
		shaderBlob->Release();
		return hr;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	hr = d3dDevice->CreateInputLayout(layout, numElements, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), vertexLayoutOut);

	shaderBlob->Release();

	return hr;
}

HRESULT Renderer::compilePixelShader(const wchar_t* fileName, const char* shaderFunctionName, ID3D11PixelShader** pixelShaderOut)
{
	ID3DBlob* shaderBlob = NULL;

	HRESULT hr = CompileShaderFromFile(fileName, shaderFunctionName,"ps_4_0", &shaderBlob);

	if( FAILED( hr ) )
	{
		MessageBox(NULL, TEXT("The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file."), TEXT("Error"), MB_OK);
		return hr;
	}

	hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, pixelShaderOut);

	shaderBlob->Release();
	return hr;
}

HRESULT Renderer::createVertexBuffer(std::vector<Vertex>& verts, ID3D11Buffer** vertexBufferOut)
{
	WaitForSingleObject(mutexDevice,INFINITE);

	if ( verts.size() == 0 )
		return S_FALSE;

	D3D11_BUFFER_DESC vertBufferDesc;
	D3D11_SUBRESOURCE_DATA vertData;

	vertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertBufferDesc.ByteWidth = (unsigned int)(sizeof(Vertex) * verts.size());
	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.CPUAccessFlags = 0;
	vertBufferDesc.MiscFlags = 0;
	vertBufferDesc.StructureByteStride = 0;

	vertData.pSysMem = verts.data();
	vertData.SysMemPitch = 0;
	vertData.SysMemSlicePitch = 0;

	HRESULT result = d3dDevice->CreateBuffer(&vertBufferDesc, &vertData, vertexBufferOut);
	if ( FAILED(result) )
		*vertexBufferOut = NULL;

	ReleaseMutex(mutexDevice);

	return result;
}

HRESULT Renderer::createIndexBuffer(std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	if (faces.size()==0)
		return S_FALSE;

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = (unsigned int)(sizeof(Vec<unsigned int>) * faces.size());
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = faces.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	HRESULT result = d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, indexBufferOut);
	if ( FAILED(result) )
		*indexBufferOut = NULL;

	ReleaseMutex(mutexDevice);

	return result;
}

MeshPrimitive* Renderer::addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
	std::vector<Vec<float>> textureUV, VertexShaders shader)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	MeshPrimitive* newMesh = new MeshPrimitive(this, faces, vertices, normals, textureUV, shader);

	ReleaseMutex(mutexDevice);

	return newMesh;
}

MeshPrimitive* Renderer::addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
	VertexShaders shader)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	MeshPrimitive* newMesh = new MeshPrimitive(this, faces, vertices, normals, shader);

	//meshPrimitives.push_back(newMesh);
	ReleaseMutex(mutexDevice);

	return newMesh;
}

HRESULT Renderer::createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (unsigned int)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, NULL, constBufferOut);

	ReleaseMutex(mutexDevice);

	return hr;
}

void Renderer::updateShaderParams(const void* params, ID3D11Buffer* buffer)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	immediateContext->UpdateSubresource(buffer,0,NULL,params,0,0);
	ReleaseMutex(mutexDevice);
}

int Renderer::getVertexShader(const std::string& shaderFilename, const std::string& shaderFunction)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	std::string shaderLookupName = shaderFilename;
	shaderLookupName += ":";
	shaderLookupName += shaderFunction;

	if (0!=vertexShaderMap.count(shaderLookupName))
	{
		int idx = vertexShaderMap[shaderLookupName];
		ReleaseMutex(mutexDevice);
		return idx;
	}

	ID3D11VertexShader* newShader;
	ID3D11InputLayout* newLayout;

	std::wstring fn;
	fn.assign(shaderFilename.begin(),shaderFilename.end());

	if (compileVertexShader(fn.c_str(), shaderFunction.c_str(),&newShader,&newLayout)==S_FALSE)
	{
		ReleaseMutex(mutexDevice);
		return -1;
	}

	vertexShaderList.push_back(newShader);
	vertexLayoutList.push_back(newLayout);

	vertexShaderMap.insert(std::pair<std::string,int>(shaderLookupName,vertexShaderList.size()-1));

	int idx = vertexShaderMap[shaderLookupName];

	ReleaseMutex(mutexDevice);

	return idx;
}

int Renderer::getPixelShader(const std::string& shaderFilename, const std::string& shaderFunction, const std::string& shaderParams)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	std::string shaderLookupName = shaderFilename + ":" + shaderFunction + ":" + shaderParams;

	if (pixelShaderMap.count(shaderLookupName)!=0)
	{
		int idx = pixelShaderMap[shaderLookupName];
		ReleaseMutex(mutexDevice);
		return idx;
	}

	ID3D11PixelShader* newShader;

	std::wstring fn;
	fn.assign(shaderFilename.begin(),shaderFilename.end());

	if (compilePixelShader(fn.c_str(), shaderFunction.c_str(),&newShader)==S_FALSE)
	{
		ReleaseMutex(mutexDevice);
		return -1;
	}

	pixelShaderList.push_back(newShader);

	pixelShaderMap.insert(std::pair<std::string,int>(shaderLookupName,pixelShaderList.size()-1));

	int idx = pixelShaderMap[shaderLookupName];

	ReleaseMutex(mutexDevice);

	return idx;
}

void Renderer::clearVertexShaderList()
{
	for (int i = 0; i<vertexShaderList.size(); ++i)
		SAFE_RELEASE(vertexShaderList[i]);

	vertexShaderList.clear();

	for (int i = 0; i<vertexLayoutList.size(); ++i)
		SAFE_RELEASE(vertexLayoutList[i]);

	vertexLayoutList.clear();

	vertexShaderMap.clear();
}

void Renderer::clearPixelShaderList()
{
	for (int i = 0; i<pixelShaderList.size(); ++i)
		SAFE_RELEASE(pixelShaderList[i]);

	pixelShaderList.clear();
	pixelShaderMap.clear();
}

void Renderer::renderAll()
{
	WaitForSingleObject(mutexDevice,INFINITE);
	startRender();
	preRenderLoop();
	mainRenderLoop();
	postRenderLoop();
	endRender();

	ReleaseMutex(mutexDevice);
}

void Renderer::attachToRootScene(SceneNode* sceneIn, Section section,int frame)
{
	WaitForSingleObject(mutexDevice,INFINITE);

	sceneIn->attachToParentNode(rootScene->getRenderSectionNode(section,frame));

	ReleaseMutex(mutexDevice);
}

void Renderer::preRenderLoop()
{
	if (rootScene->getNumRenderableObjects(Pre)<1)
		return;

	const std::vector<GraphicObjectNode*>& renderPreList = rootScene->getRenderableList(Pre,currentFrame);
	for (int i=0; i<renderPreList.size(); ++i)
	{
		renderPackage(renderPreList[i]->getRenderPackage());
	}
}

void Renderer::mainRenderLoop()
{
	if (rootScene->getNumRenderableObjects(Main)<1)
		return;

	immediateContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );

	const std::vector<GraphicObjectNode*>& renderMainList = rootScene->getRenderableList(Main,currentFrame);

	float numChunks = numPlanes*clipChunkPercent +1;

	float chunkWidth = 3.0f/numChunks;

	for (int i=numChunks-1; i>=0; --i)
	{
		float frontPlane = i*chunkWidth - 1.5f;
		float backPlane = frontPlane + chunkWidth;

		for (int i=0; i<renderMainList.size(); ++i)
		{
			renderPackage(renderMainList[i]->getRenderPackage(),frontPlane,backPlane);
		}
	}
}

void Renderer::postRenderLoop()
{
	if (rootScene->getNumRenderableObjects(Post)<1)
		return;

	immediateContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );

	const std::vector<GraphicObjectNode*>& renderPostList = rootScene->getRenderableList(Post,currentFrame);
	for (int i=0; i<renderPostList.size(); ++i)
	{
		renderPackage(renderPostList[i]->getRenderPackage());
	}
}

void Renderer::startRender()
{
	float ClearColor[4] = {backgroundColor.x, backgroundColor.y, backgroundColor.z};
	immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);
	immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0);
	immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void Renderer::endRender()
{
	swapChain->Present( 0, 0 );
}

void Renderer::renderPackage(const RendererPackage* package, float frontClip, float backClip)
{
	static int previousVertexShaderIdx = -1;
	static int previousPixelShaderIdx = -1;
	//Vertex Shader setup
	VertexShaderConstBuffer vcb;
	const Camera* camera = package->getCamera();
	vcb.projectionTransform = DirectX::XMMatrixTranspose(camera->getProjectionTransform());
	vcb.viewTransform = DirectX::XMMatrixTranspose(camera->getViewTransform());
	vcb.worldTransform = DirectX::XMMatrixTranspose(package->getLocalToWorld());
	vcb.depthPeelPlanes.x = frontClip;
	vcb.depthPeelPlanes.y = backClip;
	updateShaderParams(&vcb,vertexShaderConstBuffer);
	if (previousVertexShaderIdx!=package->getMeshPrimitive()->shaderIdx)
	{
		setVertexShader(package->getMeshPrimitive()->shaderIdx);
		previousVertexShaderIdx = package->getMeshPrimitive()->shaderIdx;
	}

	setGeometry(package->getMeshPrimitive()->vertexBuffer,package->getMeshPrimitive()->indexBuffer);

	//Rasterizer setup
	setRasterizerState(package->getMaterial()->wireframe);

	//Pixel Shader setup
	package->getMaterial()->updateParams();//can be sped up by doing this differently
	package->getMaterial()->setShaderResources();//TODO this needs tweeking
	if (previousPixelShaderIdx!=package->getMaterial()->shaderIdx)
	{
		setPixelShader(package->getMaterial()->shaderIdx);
		previousPixelShaderIdx = package->getMaterial()->shaderIdx;
	}
	
	setDepthStencilState(package->getMaterial()->testDepth);

	drawTriangles(package->getMeshPrimitive()->numFaces);
}

void Renderer::setVertexShader(int vertexShaderListIdx)
{
	if (vertexShaderListIdx>vertexShaderList.size()-1)
		gMexMessageQueueOut.addErrorMessage("There is no shader in the list of this type!");

	immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	immediateContext->VSSetShader(vertexShaderList[vertexShaderListIdx],NULL,0);
	immediateContext->IASetInputLayout(vertexLayoutList[vertexShaderListIdx]);
	immediateContext->VSSetConstantBuffers(0,1,&vertexShaderConstBuffer);
}

void Renderer::setRasterizerState(bool wireframe)
{
	if (wireframe)
		immediateContext->RSSetState(rasterizerStateWireClip);
	else
		immediateContext->RSSetState(rasterizerStateFillClip);
}

void Renderer::setPixelShader(int pixelShaderListIdx)
{
	if (pixelShaderListIdx>pixelShaderList.size()-1)
		gMexMessageQueueOut.addErrorMessage("No pixel shader found in list with this index!");

	immediateContext->PSSetShader(pixelShaderList[pixelShaderListIdx],NULL,0);
}

void Renderer::setDepthStencilState(bool depthTest)
{
	if (depthTest)
		immediateContext->OMSetDepthStencilState(depthStencilStateCompareLess,0);
	else
		immediateContext->OMSetDepthStencilState(depthStencilStateCompareAlways,0);
}

void Renderer::setGeometry(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	immediateContext->IASetIndexBuffer(indexBuffer,DXGI_FORMAT_R32_UINT,0);
	immediateContext->IASetVertexBuffers(0,1,&vertexBuffer,&stride,&offset);
}

void Renderer::drawTriangles(size_t numFaces)
{
	immediateContext->DrawIndexed(unsigned int(3*numFaces),0,0);
}

void Renderer::setPixelShaderConsts(ID3D11Buffer* buffer)
{
	immediateContext->PSSetConstantBuffers(1,1,&buffer);
}

ID3D11SamplerState* Renderer::getSamplerState()
{
	if (linearTextureSampler==NULL)
	{
		D3D11_SAMPLER_DESC samDesc;
		ZeroMemory( &samDesc, sizeof(samDesc) );
		samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samDesc.BorderColor[0] = 0.0;
		samDesc.BorderColor[1] = 0.0;
		samDesc.BorderColor[2] = 0.0;
		samDesc.BorderColor[3] = 0.0;
		samDesc.MaxAnisotropy = 16;
		samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = d3dDevice->CreateSamplerState(&samDesc,&linearTextureSampler);
		if (FAILED(hr))
			gMexMessageQueueOut.addErrorMessage(hr);
	}

	return linearTextureSampler;
}

ID3D11ShaderResourceView* Renderer::createTextureResourceView(Vec<size_t> dims, unsigned char* image)
{
	HRESULT hr = S_OK;
	UINT iMipCount = 1;
	UINT BitSize = 0;

	D3D11_TEXTURE3D_DESC desc;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Width = (unsigned int)dims.x;
	desc.Height = (unsigned int)dims.y;
	desc.Depth = (unsigned int)dims.z;
	desc.MipLevels = iMipCount;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture3D* pTex3D = NULL;

	D3D11_SUBRESOURCE_DATA initData;
	initData.SysMemPitch = (unsigned int)dims.x;
	initData.SysMemSlicePitch = unsigned int(dims.x*dims.y);
	initData.pSysMem = (void*)image;

	hr = d3dDevice->CreateTexture3D( &desc, &initData, &pTex3D );
	if( FAILED( hr ))
		gMexMessageQueueOut.addErrorMessage(hr);

	ID3D11ShaderResourceView* localSRV;
	hr = d3dDevice->CreateShaderResourceView( pTex3D, NULL, &localSRV);
	SAFE_RELEASE( pTex3D );
	if( FAILED( hr ))
		gMexMessageQueueOut.addErrorMessage(hr);

	return localSRV;
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

	d3dDevice->CreateBlendState(&BlendState, &blendState);
	immediateContext->OMSetBlendState(blendState, 0, 0xffffffff);
}

void Renderer::setPixelShaderResourceViews(int startIdx, int length, ID3D11ShaderResourceView** shaderResourceView)
{
	immediateContext->PSSetShaderResources(startIdx,length,shaderResourceView);
}

void Renderer::setPixelShaderTextureSamplers(int startIdx, int length, ID3D11SamplerState** samplerState)
{
	immediateContext->PSSetSamplers(startIdx,length,samplerState);
}

DirectX::XMMATRIX Renderer::getRootWorldRotation()
{
	return curRotationMatrix;
}

void Renderer::getMutex()
{
	WaitForSingleObject(mutexDevice,INFINITE);
}

void Renderer::releaseMutex()
{
	ReleaseMutex(mutexDevice);
}

int Renderer::getHull(Vec<float> pnt, Vec<float> direction)
{
	WaitForSingleObject(mutexDevice,INFINITE);
	float depth;
	int label = rootScene->getHull(pnt,direction,currentFrame,depth);
	ReleaseMutex(mutexDevice);

	return label;
}

void Renderer::resizeViewPort()
{
	WaitForSingleObject(mutexDevice,INFINITE);

	DXGI_SWAP_CHAIN_DESC desc;  
	swapChain->GetDesc( &desc );  

	immediateContext->OMSetRenderTargets(0, 0, 0);

	releaseRenderTarget();
	releaseDepthStencils();

	HRESULT hr;
	hr = swapChain->ResizeBuffers(2, gWindowWidth, gWindowHeight,DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	initDepthStencils();
	initRenderTarget();

	resetViewPort();
	gCameraWidget->updateProjectionTransform();
	gCameraDefaultMesh->updateProjectionTransform();

	ReleaseMutex(mutexDevice);
}

HRESULT Renderer::resetViewPort()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)gWindowWidth;
	vp.Height = (float)gWindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediateContext->RSSetViewports( 1, &vp );

	return S_OK;
}

void Renderer::setCurrentFrame(int frame)
{
	if (frame>rootScene->getNumFrames())
		frame = rootScene->getNumFrames()-1;

	currentFrame = frame;

	gMexMessageQueueOut.addMessage("timeChange",currentFrame);
}

void Renderer::incrementFrame()
{
	++currentFrame;
	if (currentFrame>unsigned int(rootScene->getNumFrames()-1))
		currentFrame = 0;

	gMexMessageQueueOut.addMessage("timeChange",currentFrame);
}

void Renderer::decrementFrame()
{
	if (currentFrame!=0)
		--currentFrame;

	gMexMessageQueueOut.addMessage("timeChange",currentFrame);
}

unsigned int Renderer::getNumberOfFrames()
{
	return rootScene->getNumFrames();
}

void Renderer::resetRootWorldTransform()
{
	origin = Vec<float>(0.0f,0.0f,0.0f);
	curRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f,0.0f,DirectX::XM_PI);
	updateWorldTransform();
}

void Renderer::setClipChunkPercent(float ccp)
{
	WaitForSingleObject(mutexDevice,INFINITE);

	clipChunkPercent = ccp;

	ReleaseMutex(mutexDevice);
}

void Renderer::setNumPlanes(int numPlanesIn)
{
	WaitForSingleObject(mutexDevice,INFINITE);

	if (numPlanes < numPlanesIn)
		numPlanes = numPlanesIn;

	ReleaseMutex(mutexDevice);
}


void Renderer::setWorldOrigin(Vec<float> org)
{
	origin = org;
	updateWorldTransform();
}

void Renderer::setWorldRotation(DirectX::XMMATRIX rotation)
{
	curRotationMatrix = rotation;
	updateWorldTransform();
}

DirectX::XMMATRIX Renderer::createWorldMatrix()
{
	DirectX::XMMATRIX transMatrix = DirectX::XMMatrixTranslation(-origin.x,-origin.y,-origin.z);

	return transMatrix * curRotationMatrix;
}

void Renderer::updateWorldTransform()
{
	WaitForSingleObject(mutexDevice,INFINITE);

	rootScene->setLocalToParent(createWorldMatrix());

	ReleaseMutex(mutexDevice);
}
