#include "Renderer.h"
#include "Globals.h"
#include "Defines.h"
#include <d3dcompiler.h>
#include "MeshPrimitive.h"

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
}

Renderer::~Renderer()
{
	//TODO Ensure that everything has been cleaned up
	releaseDepthStencils();
	releaseRenderTarget();

	clearRenderList(Pre);
	clearRenderList(Main);
	clearRenderList(Post);
}

HRESULT Renderer::init()
{
	if (initSwapChain() == S_FALSE)
		return S_FALSE;

	if (initDepthStencils() == S_FALSE)
		return S_FALSE;

	if (initRenderTarget() == S_FALSE)
		return S_FALSE;

	if (initRasterizerStates() == S_FALSE)
		return S_FALSE;

	return S_OK;
}

HRESULT Renderer::initSwapChain()
{
	HRESULT hr = S_FALSE;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
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

	return hr;
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
	descDepth.Height = gWindowWidth;
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
	descDS.DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL    ;
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
	//d3d11rd.CullMode=D3D11_CULL_FRONT;
	d3d11rd.DepthClipEnable=TRUE;
	d3d11rd.MultisampleEnable=FALSE;
	d3d11rd.AntialiasedLineEnable=TRUE;
	d3dDevice->CreateRasterizerState(&d3d11rd,&rasterizerStateFillClip);

	ReleaseMutex(mutexDevice);

	return S_OK;
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	hr = d3dDevice->CreateInputLayout(layout, numElements, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), vertexLayoutOut);

	shaderBlob->Release();

	return hr;
}

HRESULT Renderer::compilePixelShader(const wchar_t* fileName, const char* shaderFunctionName, ID3D11PixelShader** pixelShaderOut)
{
	ID3DBlob* shaderBlob = NULL;

	HRESULT hr = CompileShaderFromFile(fileName, shaderFunctionName,"vs_4_0", &shaderBlob);

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

	DWORD waitResult = WaitForSingleObject(mutexDevice, INFINITE);
	if ( waitResult != WAIT_OBJECT_0 )
		return S_FALSE;

	HRESULT result = d3dDevice->CreateBuffer(&vertBufferDesc, &vertData, vertexBufferOut);
	ReleaseMutex(mutexDevice);
	if ( FAILED(result) )
		*vertexBufferOut = NULL;

	return result;
}

HRESULT Renderer::createIndexBuffer(std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut)
{
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

	DWORD waitResult = WaitForSingleObject(mutexDevice, INFINITE);
	if ( waitResult != WAIT_OBJECT_0 )
		return S_FALSE;

	HRESULT result = d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, indexBufferOut);
	ReleaseMutex(mutexDevice);
	if ( FAILED(result) )
		*indexBufferOut = NULL;

	return result;
}

MeshPrimitive* Renderer::addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
	std::vector<Vec<float>> textureUV, VertexShaders shader)
{
	MeshPrimitive* newMesh = new MeshPrimitive(this, faces, vertices, normals, textureUV, shader);

	meshPrimitives.push_back(newMesh);

	return newMesh;
}

MeshPrimitive* Renderer::addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
	VertexShaders shader)
{
	MeshPrimitive* newMesh = new MeshPrimitive(this, faces, vertices, normals, shader);

	meshPrimitives.push_back(newMesh);

	return newMesh;
}

HRESULT Renderer::createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut)
{
	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (unsigned int)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	return d3dDevice->CreateBuffer(&bufferDesc, NULL, constBufferOut);
}

void Renderer::updateShaderParams(void* params, ID3D11Buffer* buffer)
{
	immediateContext->UpdateSubresource(buffer,0,NULL,params,0,0);
}

int Renderer::getVertexShader(const std::string& shaderFilename, const std::string& shaderFunction)
{
	std::string shaderLookupName = shaderFilename;
	shaderLookupName += ":";
	shaderLookupName += shaderFunction;

	if (0!=vertexShaderMap.count(shaderLookupName))
		return vertexShaderMap[shaderLookupName];

	ID3D11VertexShader* newShader;
	ID3D11InputLayout* newLayout;

	std::wstring fn;
	fn.assign(shaderFilename.begin(),shaderFilename.end());

	if (compileVertexShader(fn.c_str(), shaderFunction.c_str(),&newShader,&newLayout)==S_FALSE)
		return -1;

	vertexShaderList.push_back(newShader);
	vertexLayoutList.push_back(newLayout);

	vertexShaderMap.insert(std::pair<std::string,int>(shaderLookupName,vertexShaderList.size()-1));

	return vertexShaderMap[shaderLookupName];
}

int Renderer::getPixelShader(const std::string& shaderFilename, const std::string& shaderFunction)
{
	std::string shaderLookupName = shaderFilename;
	shaderLookupName += ":";
	shaderLookupName += shaderFunction;

	if (pixelShaderMap.count(shaderLookupName)!=0)
		return pixelShaderMap[shaderLookupName];

	ID3D11PixelShader* newShader;

	std::wstring fn;
	fn.assign(shaderFilename.begin(),shaderFilename.end());

	if (compilePixelShader(fn.c_str(), shaderFunction.c_str(),&newShader)==S_FALSE)
		return -1;

	pixelShaderList.push_back(newShader);

	pixelShaderMap.insert(std::pair<std::string,int>(shaderLookupName,pixelShaderList.size()-1));

	return pixelShaderMap[shaderLookupName];
}

HRESULT Renderer::addToRenderList(RendererPackage* rendererPackage, Section section)
{
	if (rendererPackage==NULL)
		return S_FALSE;

	switch (section)
	{
	case Pre:
		if (0!=renderPreListMap.count(rendererPackage))
			break;

		renderPreList.push_back(rendererPackage);
		renderPreListMap.insert(std::pair<RendererPackage*,int>(rendererPackage,renderPreList.size()-1));
		break;
	case Main:
		if (0!=renderMainListMap.count(rendererPackage))
			break;

		renderMainList.push_back(rendererPackage);
		renderMainListMap.insert(std::pair<RendererPackage*,int>(rendererPackage,renderMainList.size()-1));
		break;
	case Post:
		if (0!=renderPostListMap.count(rendererPackage))
			break;

		renderPostList.push_back(rendererPackage);
		renderPostListMap.insert(std::pair<RendererPackage*,int>(rendererPackage,renderPostList.size()-1));
		break;
	}

	return S_OK;
}

HRESULT Renderer::removeFromRenderList(RendererPackage* rendererPackage, Section section)
{
	if (rendererPackage==NULL)
		return S_FALSE;
	int idx = -1;

	switch (section)
	{
	case Renderer::Pre:
		if (0==renderPreListMap.count(rendererPackage))
			break;

		idx = renderPreListMap[rendererPackage];
		renderPreList.erase(renderPreList.begin()+idx);

		renderPreListMap.erase(rendererPackage);
		break;
	case Renderer::Main:
		if (0==renderMainListMap.count(rendererPackage))
			break;

		idx = renderMainListMap[rendererPackage];
		renderMainList.erase(renderMainList.begin()+idx);
		renderMainListMap.erase(rendererPackage);
		break;
	case Renderer::Post:
		if (0==renderPostListMap.count(rendererPackage))
			break;

		idx = renderPostListMap[rendererPackage];
		renderPostList.erase(renderPostList.begin()+idx);
		renderPostListMap.erase(rendererPackage);
		break;
	default:
		return S_FALSE;
		break;
	}

	return S_OK;
}

HRESULT Renderer::clearRenderList(Section section)
{
	switch (section)
	{
	case Renderer::Pre:
		renderPreList.clear();
		break;
	case Renderer::Main:
		renderMainList.clear();
		break;
	case Renderer::Post:
		renderPostList.clear();
		break;
	default:
		break;
	}

	return S_OK;
}
