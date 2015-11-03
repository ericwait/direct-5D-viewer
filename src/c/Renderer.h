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

#pragma once
#include "Vec.h"
#include <d3d11.h>
#include <vector>
#include <map>
#include <string>
#include <DirectXMath.h>

class MeshPrimitive;
class Material;
class RendererPackage;
class SceneNode;
class RootSceneNode;

struct Vertex
{
	Vec<float> pos;
	Vec<float> texUV;
	Vec<float> normal;
};

class Renderer
{
public:
	enum Section
	{
		Pre,
		Main,
		Post,
		SectionEnd
	};

	enum VertexShaders
	{
		DefaultVS,
		ViewAligned,
		VertexShadersEnd
	};

	enum PixelShaders
	{
		DefaultPS,
		StaticVolume,
		PixelShadersEnd
	};

	struct VertexShaderConstBuffer
	{
		DirectX::XMMATRIX worldTransform;
		DirectX::XMMATRIX viewTransform;
		DirectX::XMMATRIX projectionTransform;
		DirectX::XMFLOAT4 depthPeelPlanes;
	};

	Renderer();
	~Renderer();

//////////////////////////////////////////////////////////////////////////
// Class setup including getters and setters
//////////////////////////////////////////////////////////////////////////
	HRESULT init(std::string rootDir);

//Setters	
	void setCurrentFrame(int frame);
	void incrementFrame();
	void decrementFrame();
	unsigned int getNumberOfFrames();
	void attachToRootScene(SceneNode* sceneIn, Section section, int frame);
	void removeFromRootScene(SceneNode* sceneIn);

	void clearVertexShaderList();
	void clearPixelShaderList();
	void resizeViewPort();

	void updateShaderParams(const void* params, ID3D11Buffer* buffer);
	void setPixelShaderConsts(ID3D11Buffer* buffer);
	void setPixelShaderResourceViews(int startIdx, int length, ID3D11ShaderResourceView** shaderResourceView);
	void setPixelShaderTextureSamplers(int startIdx, int length, ID3D11SamplerState** samplerState);

	void setWorldOrigin(Vec<float> org);
	void setWorldRotation(DirectX::XMMATRIX rotation);
	void resetRootWorldTransform();
	
	void setClipChunkPercent(float ccp);
	void setNumPlanes(int numPlanes); //TODO this could be changed to be smarter about where to peel from
	void updateRenderList();
	void setLabels(bool on){labelsOn=on;}
	void toggleLabels(){labelsOn = !labelsOn;}
	void setCaptureFilePath(std::string fp){ captureFilePath = fp; }
	void setCaptureFileName(std::string fn){ captureFileName = fn; }

//Getters
	int getVertexShader(const std::string& shaderFilename, const std::string& shaderFunction);
	int getPixelShader(const std::string& shaderFilename, const std::string& shaderFunction, const std::string& shaderParams);
	ID3D11SamplerState* getSamplerState();

	DirectX::XMMATRIX getRootWorldRotation();
	//void getMutex(); // comment these out
	//void releaseMutex();

	int getHull(Vec<float> pnt, Vec<float> direction);
	float getClipChunkPercent(){return clipChunkPercent;}
	HRESULT captureWindow(std::string* filenameOut);
	HRESULT captureWindow(std::string filePathIn, std::string fileNameIn, std::string& filenameOut);
	unsigned char* captureWindow(DWORD& dwBmpSize,BITMAPINFOHEADER& bi);

	std::string getDllDir() { return dllRoot; }

//////////////////////////////////////////////////////////////////////////
// Rendering to screen
//////////////////////////////////////////////////////////////////////////
	void renderAll();
	void startRender();
	void preRenderLoop();
	void mainRenderLoop();
	void postRenderLoop();
	void gdiRenderLoop();
	void endRender();

//////////////////////////////////////////////////////////////////////////
// Resource setup for external classes
//////////////////////////////////////////////////////////////////////////
	HRESULT createVertexBuffer(std::vector<Vertex>& verts, ID3D11Buffer** vertexBufferOut);
	HRESULT createIndexBuffer(std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut);
	HRESULT createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut);
	ID3D11ShaderResourceView* createTextureResourceView(Vec<size_t> dims, unsigned char* image);

	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		std::vector<Vec<float>> textureUV, VertexShaders shader);
	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		VertexShaders shader);

private:
	HRESULT initSwapChain();
	HRESULT initDepthStencils();
	HRESULT initRenderTarget();
	HRESULT resetViewPort();
	HRESULT initRasterizerStates();
	void createBlendState();

	void releaseDepthStencils();
	void releaseRenderTarget();
	void releaseRasterizerStates();
	void releaseSwapChain();

	void setVertexShader(int vertexShaderListIdx);
	void setRasterizerState(bool wireframe);
	void setPixelShader(int pixelShaderListIdx);
	void setDepthStencilState(bool depthTest);
	void setGeometry(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer);
	void drawTriangles(size_t numFaces);

	HRESULT compileVertexShader(const wchar_t* fileName, const char* shaderFunctionName, ID3D11VertexShader** vertexShaderOut, ID3D11InputLayout** vertexLayoutOut);
	HRESULT compilePixelShader(const wchar_t* fileName, const char* shaderFunctionName, ID3D11PixelShader** pixelShaderOut);

	void renderPackage(const RendererPackage* package, float frontClip=-10, float backClip=10);
	void renderLabel(const RendererPackage* package, HDC hdc);

	//Member variables 
	Vec<float> backgroundColor;

	//HANDLE mutexDevice; // comment this out
	IDXGISwapChain*	swapChain;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* immediateContext;
	ID3D11RenderTargetView* renderTargetView;
	IDXGISurface1* IDXGIBackBuffer;	
	ID3D11BlendState* blendState;

	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilStateCompareAlways;
	ID3D11DepthStencilState* depthStencilStateCompareLess;
	
	ID3D11RasterizerState* rasterizerStateNoClip;
	ID3D11RasterizerState* rasterizerStateWireClip;
	ID3D11RasterizerState* rasterizerStateFillClip;

	ID3D11Buffer* vertexShaderConstBuffer;
	
	std::map<std::string,int> pixelShaderMap;
	std::map<std::string,int> vertexShaderMap;

	std::vector<ID3D11PixelShader*> pixelShaderList;
	std::vector<ID3D11VertexShader*> vertexShaderList;
	std::vector<ID3D11InputLayout*> vertexLayoutList;

	ID3D11SamplerState* linearTextureSampler;

	RootSceneNode* rootScene;
	unsigned int currentFrame;
	float clipChunkPercent;
	int numPlanes;
	bool labelsOn;
	std::string captureFilePath;
	std::string captureFileName;
	std::string dllRoot;
};


const std::string VERTEX_SHADER_FILENAMES[Renderer::VertexShaders::VertexShadersEnd] = {"\\DefaultMeshShaders.fx","\\ViewAlignedVertexShader.fx"};
const std::string VERTEX_SHADER_FUNCNAMES[Renderer::VertexShaders::VertexShadersEnd] = {"DefaultMeshVertexShader","ViewAlignedVertexShader"};

const std::string PIXEL_SHADER_FILENAMES[Renderer::PixelShaders::PixelShadersEnd] ={"\\DefaultMeshShaders.fx","\\StaticVolumePixelShader.fx"};
const std::string PIXEL_SHADER_FUNCNAMES[Renderer::PixelShaders::PixelShadersEnd] ={"DefaultMeshPixelShader","MultiChanVolumePixelShader"};