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
#include "Global/Vec.h"
#include "VertexLayouts.h"

#include <d3d11.h>
#include <DXGI1_2.h>
#include <DirectXMath.h>

#include <Eigen/Dense>

#include <vector>
#include <map>
#include <string>
#include <memory>

class Camera;
class MeshPrimitive;
class Material;
class MaterialParameters;
class VolumeParams;
class SceneNode;
class RootSceneNode;
class GraphicObjectNode;
class RenderFilter;
class RenderTarget;
class DepthTarget;
class SwapChainTarget;
class VolumeInfo;

enum GraphicObjectTypes
{
	Group,
	Widget,
	Polygons,
	Border,
	Text,
	OriginalVolume,
	ProcessedVolume,
	NumGO
};

enum TargetChains
{
	Screen = 0,
	Capture = 1,
	NumTC
};

enum RenderTargetTypes
{
	NoneRT = -1,
	DefaultRT = 0,
	NumRT
};

enum DepthTargetTypes
{
	NoneDT = -1,
	DefaultDT = 0,
	NumDT
};

enum InterpTypes
{
	Nearest = 0,
	Linear = 1
};

class Renderer
{
	friend class TextRenderer;
public:
	enum Section
	{
		Pre,
		Main,
		Post,
		SectionEnd
	};

	struct VertexShaderConstBuffer
	{
		DirectX::XMMATRIX worldTransform;
		DirectX::XMMATRIX viewTransform;
		DirectX::XMMATRIX projectionTransform;
		DirectX::XMFLOAT4 depthPeelPlanes;
	};

	static const float cornerVolumeDist;

	Renderer();
	~Renderer();

//////////////////////////////////////////////////////////////////////////
// Class setup including getters and setters
//////////////////////////////////////////////////////////////////////////
	HRESULT init(std::string rootDir, Vec<int> viewportSizeIn);

//Setters	
	void setRendering(bool rendering){isRendering = rendering;}
	void setCurrentFrame(int frame);
	void incrementFrame();
	void decrementFrame();
	unsigned int getNumberOfFrames();

	void attachToRootScene(SceneNode* sceneIn, Section section, int frame);
	void removeSceneObjects(GraphicObjectTypes type);
	GraphicObjectNode* findSceneObject(GraphicObjectTypes type, int index);
	std::map<int,GraphicObjectNode*>& allSceneObjects(GraphicObjectTypes type);

	void clearVertexShaderList();
	void clearPixelShaderList();
	void clearFallbackShaders();

	void resizeViewPort(Vec<int> sizeIn, TargetChains selectChain = TargetChains::Screen);
	void flushContext();

	void updateShaderParams(const void* params, ID3D11Buffer* buffer);
	void setPixelShaderConsts(ID3D11Buffer* buffer);
	void setPixelShaderResourceViews(int startIdx, int length, ID3D11ShaderResourceView** shaderResourceView);
	void setPixelShaderTextureSamplers(int startIdx, int length, ID3D11SamplerState** samplerState);

	void initVolumeInfo(int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physSize, bool columnMajor);

	void setWorldOrigin(Vec<float> org);
	void setWorldRotation(DirectX::XMMATRIX rotation);
	void resetRootWorldTransform();
	
	void setClipChunkPercent(float ccp);
	void setNumPlanes(int numPlanes); //TODO this could be changed to be smarter about where to peel from
	void setLabels(bool on){labelsOn=on;}
	void toggleLabels(){labelsOn = !labelsOn;}
	void setFrameNumOn(bool on) { frameNumOn = on; }
	void toggleFrameNum() { frameNumOn = !frameNumOn; }
	void setscaleTextOn(bool on) { scaleTextOn = on; }
	void togglescaleText() { scaleTextOn = !scaleTextOn; }
	void setscaleBarOn(bool on) { scaleBarOn = on; }
	void togglescaleBar() { scaleBarOn = !scaleBarOn; }
	void setfpsOn(bool on) { fpsOn = on; }
	void togglefps() { fpsOn = !fpsOn; }
	void togglestats();
	void setCaptureFilePath(std::string fp){ captureFilePath = fp; }
	void setCaptureFileName(std::string fn){ captureFileName = fn; }

	void setBackgroundColor(Vec<float> background) { backgroundColor = background; }

//Getters
	int updateRegisteredShaders();
	int registerVertexShader(const std::string& filename, const std::string& entrypoint, VertexLayout layout,
		const std::map<std::string,std::string>& variables = std::map<std::string,std::string>());

	int registerPixelShader(const std::string& filename, const std::string& entrypoint,
		const std::map<std::string,std::string>& variables = std::map<std::string,std::string>());

	VolumeInfo* getVolumeInfo(){ return volInfo; }
	DirectX::XMMATRIX getRootWorldRotation();

	int getPolygon(Vec<float> pnt, Vec<float> direction);
	float getClipChunkPercent(){return clipChunkPercent;}
	HRESULT captureWindow(std::string* filenameOut);
	HRESULT captureWindow(std::string filePathIn, std::string fileNameIn, std::string& filenameOut);
	unsigned char* captureWindow(DWORD& dwBmpSize,BITMAPINFOHEADER& bi);

	unsigned char* captureWindow(Vec<size_t>& dims);

	std::string getDllDir() { return dllRoot; }

//////////////////////////////////////////////////////////////////////////
// Rendering to screen
//////////////////////////////////////////////////////////////////////////
	void forceUpdate() { isDirty = true; }
	bool needsUpdate(){return (isRendering && isDirty);}

	void renderUpdate();

	void renderAll(TargetChains renderChain);
	void startRender(TargetChains renderChain);

	void renderBackground(TargetChains renderChain);
	void renderPolygons(TargetChains renderChain);
	void renderVolume(TargetChains renderChain);
	void renderWidget(TargetChains renderChain);
	void renderTextOverlays(TargetChains renderChain);

	void endRender(TargetChains renderChain);

//////////////////////////////////////////////////////////////////////////
// Resource setup for external classes
//////////////////////////////////////////////////////////////////////////
	HRESULT createVertexBuffer(UINT accessFlags, D3D11_USAGE usage, size_t bufferSize, const void* initData, ID3D11Buffer** vertexBufferOut);
	HRESULT createIndexBuffer(UINT accessFlags, D3D11_USAGE usage, const std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut);
	HRESULT createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut);

	HRESULT lockBuffer(ID3D11Buffer* buffer, D3D11_MAP mapType, D3D11_MAPPED_SUBRESOURCE& outResource);
	void releaseBuffer(ID3D11Buffer* buffer);

	IDXGISwapChain1* createSwapChain(HWND hWnd, Vec<size_t> dims, bool stereo, DXGI_FORMAT format, UINT flags);

	ID3D11SamplerState* createSamplerState(InterpTypes interpType);
	ID3D11ShaderResourceView* createShaderResourceView(ID3D11Resource* textureResource);
	ID3D11RenderTargetView* createRenderTargetView(ID3D11Resource* textureResource);
	ID3D11DepthStencilView* createDepthTargetView(ID3D11Resource* textureResource);
	ID3D11Texture2D* createTexture2D(D3D11_TEXTURE2D_DESC* desc, D3D11_SUBRESOURCE_DATA* initData = NULL);
	ID3D11Texture3D* createTexture3D(D3D11_TEXTURE3D_DESC* desc, D3D11_SUBRESOURCE_DATA* initData = NULL);

	void stageResource(ID3D11Texture2D* stageTexture, ID3D11Texture2D* renderTexture);
	void readSurface(unsigned char* outBuffer, ID3D11Texture2D* texture, Vec<size_t> dims, size_t pixelSize);

	ID3D11RasterizerState* getRasterizerState(bool wireframe, D3D11_CULL_MODE cullFaces);
	ID3D11DepthStencilState* getDepthStencilState(bool depthTest);

	void detachTargets();
	void attachTargets(TargetChains chain, RenderTargetTypes rt, DepthTargetTypes dt);

	void clearRenderTarget(TargetChains chain, RenderTargetTypes rt, Vec<float> clearColor);
	void clearDepthTarget(TargetChains chain, DepthTargetTypes dt, float clearDepth);

	float FrontClipPos() const;
	void FrontClipPos(float val);
	float BackClipPos() const;
	void BackClipPos(float val);
private:
	HRESULT initDevice();
	HRESULT initDepthStencils();
	HRESULT initRenderTargets();
	HRESULT resetViewPort(TargetChains selectChain);
	void createBlendState();
	
	void releaseDepthStencils();
	void releaseRenderTargets();
	void releaseMaterialStates();
	void releaseDevice();

	void initFallbackShaders();
	void updateVertexShader(int entryIdx);
	void updatePixelShader(int entryIdx);

	void setVertexShader(ID3D11VertexShader* shader, ID3D11InputLayout* layout);
	void setRasterizerState(ID3D11RasterizerState* rasterState);
	void setPixelShader(ID3D11PixelShader* shader);
	void setDepthStencilState(ID3D11DepthStencilState* depthStencilState);
	void setGeometry(VertexLayout layoutInfo, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer);
	void drawTriangles(size_t numFaces);

	HRESULT compileVertexShader(const std::string& filename, const std::string& functionName,
		const std::map<std::string, std::string>& variables, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc,
		ID3D11VertexShader** vertexShaderOut, ID3D11InputLayout** vertexLayoutOut);

	HRESULT compilePixelShader(const std::string& filename, const std::string& functionName,
		const std::map<std::string, std::string>& variables, ID3D11PixelShader** pixelShaderOut);

	std::string preprocessShader(const std::string& shader, const std::map<std::string, std::string>& repVars);
	ID3DBlob* compileShaderFile(const std::string& filename, const std::string& entryFunction, const std::string& shaderModel, const std::map<std::string, std::string>& repVars);

	void renderNode(const Camera* camera, const GraphicObjectNode* node, float frontClip=-10, float backClip=10);

	void renderLabel(TargetChains chain, const Camera* camera, const GraphicObjectNode* node);
	void renderScaleValue(TargetChains chain, const Camera* camera);
	void renderFrameNum(TargetChains chain);
	void renderFPS(TargetChains chain);

	const SwapChainTarget* getSwapChain() const;


	//Member variables 
	bool isDirty;
	bool isRendering;

	TargetChains lastChain;
	Vec<int> viewportSize[TargetChains::NumTC];

	Vec<float> backgroundColor;

	VolumeInfo* volInfo;

	D3D_FEATURE_LEVEL renderFeatureLevel;

	ID3D11Device* renderDevice;
	ID3D11DeviceContext* renderContext;
	IDXGIFactory2* renderFactory;

	ID3D11BlendState* blendState;

	ID3D11Buffer* vertexShaderConstBuffer;
	
	std::map<std::string,int> pixelShaderMap;
	std::map<std::string,int> vertexShaderMap;

	struct VertexShaderEntry
	{
		bool error;

		FILETIME update;

		std::string filename;
		std::string entryFunc;
		std::map<std::string,std::string> vars;

		VertexLayout layoutInfo;

		ID3D11VertexShader* shader;
		ID3D11InputLayout* layout;
	};

	struct PixelShaderEntry
	{
		bool error;

		FILETIME update;

		std::string filename;
		std::string entryFunc;
		std::map<std::string,std::string> vars;

		ID3D11PixelShader* shader;
	};

	std::vector<VertexShaderEntry> vertexShaderRegistry;
	std::vector<PixelShaderEntry> pixelShaderRegistry;

	std::map<VertexLayout::Types, ID3D11VertexShader*> fallbackVSs;
	std::map<VertexLayout::Types, ID3D11InputLayout*> fallbackLayouts;
	ID3D11PixelShader* fallbackPS;

	// Scene objects
	//TODO: Should probably put cameras in the scene
	RootSceneNode* rootScene;
	TextRenderer* textRenderer;

	unsigned int currentFrame;
	float clipChunkPercent;
	float frontClipPos;
	float backClipPos;
	int numPlanes;
	bool labelsOn;
	bool frameNumOn;
	bool scaleTextOn;
	bool scaleBarOn;
	bool fpsOn;
	bool statsOn;
	std::string captureFilePath;
	std::string captureFileName;
	std::string dllRoot;

	static const int NUM_TIMES = 20;
	UINT64 frameTimes[NUM_TIMES];
	UINT64 startTimes[NUM_TIMES];
	UINT64 preTimes[NUM_TIMES];
	UINT64 mainTimes[NUM_TIMES];
	UINT64 postTimes[NUM_TIMES];
	UINT64 gdiTimes[NUM_TIMES];
	UINT64 endTimes[NUM_TIMES];
	int curTimeIdx;

private:
	std::shared_ptr<RenderTarget> renderChains[TargetChains::NumTC][RenderTargetTypes::NumRT];
	std::shared_ptr<DepthTarget> depthChains[TargetChains::NumTC][DepthTargetTypes::NumDT];

	std::map<unsigned int,ID3D11DepthStencilState*> depthStencilStates;
	std::map<unsigned int,ID3D11RasterizerState*> rasterStates;
};
