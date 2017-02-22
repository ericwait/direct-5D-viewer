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

struct Vertex
{
	Vec<float> pos;
	Vec<float> texUV;
	Vec<float> normal;
};

enum GraphicObjectTypes
{
	Group,
	Widget,
	Polygons,
	Border,
	OriginalVolume,
	ProcessedVolume,
	VTend
};

enum RenderTargetTypes
{
	NoneRT = -1,
	SwapChain = 0,
	Capture = 1,
	NumRT
};

enum DepthTargetTypes
{
	NoneDT = -1,
	Default = 0,
	NumDT
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
	HRESULT init(std::string rootDir);

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
	void resizeViewPort();
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
	int registerVertexShader(const std::string& filename, const std::string& entrypoint, const std::map<std::string,std::string>& variables = std::map<std::string,std::string>());
	int registerPixelShader(const std::string& filename, const std::string& entrypoint, const std::map<std::string,std::string>& variables = std::map<std::string,std::string>());

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

	void renderAll();
	void startRender();

	void renderBackground();
	void renderPolygons();
	void renderVolume();
	void renderWidget();

	void renderGDIOverlay();

	void endRender();

//////////////////////////////////////////////////////////////////////////
// Resource setup for external classes
//////////////////////////////////////////////////////////////////////////
	HRESULT createVertexBuffer(std::vector<Vertex>& verts, ID3D11Buffer** vertexBufferOut);
	HRESULT createIndexBuffer(std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut);
	HRESULT createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut);

	IDXGISwapChain1* createSwapChain(HWND hWnd, Vec<size_t> dims, bool stereo, DXGI_FORMAT format, UINT flags);

	ID3D11SamplerState* createSamplerState();
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
	void attachTargets(RenderTargetTypes rt, DepthTargetTypes dt);

	void clearRenderTarget(RenderTargetTypes rt, Vec<float> clearColor);
	void clearDepthTarget(DepthTargetTypes dt, float clearDepth);

	float FrontClipPos() const;
	void FrontClipPos(float val);
	float BackClipPos() const;
	void BackClipPos(float val);
private:
	HRESULT initDevice();
	HRESULT initDepthStencils();
	HRESULT initRenderTargets();
	HRESULT resetViewPort();
	void createBlendState();
	
	void releaseDepthStencils();
	void releaseRenderTarget();
	void releaseMaterialStates();
	void releaseDevice();

	void initFallbackShaders();
	void updateVertexShader(int entryIdx);
	void updatePixelShader(int entryIdx);

	void setVertexShader(ID3D11VertexShader* shader, ID3D11InputLayout* layout);
	void setRasterizerState(ID3D11RasterizerState* rasterState);
	void setPixelShader(ID3D11PixelShader* shader);
	void setDepthStencilState(ID3D11DepthStencilState* depthStencilState);
	void setGeometry(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer);
	void drawTriangles(size_t numFaces);

	HRESULT compileVertexShader(const std::string& filename, const std::string& functionName, const std::map<std::string, std::string>& variables, ID3D11VertexShader** vertexShaderOut, ID3D11InputLayout** vertexLayoutOut);
	HRESULT compilePixelShader(const std::string& filename, const std::string& functionName, const std::map<std::string, std::string>& variables, ID3D11PixelShader** pixelShaderOut);

	std::string preprocessShader(const std::string& shader, const std::map<std::string, std::string>& repVars);
	ID3DBlob* compileShaderFile(const std::string& filename, const std::string& entryFunction, const std::string& shaderModel, const std::map<std::string, std::string>& repVars);

	void renderNode(const Camera* camera, const GraphicObjectNode* node, float frontClip=-10, float backClip=10);
	void renderLabel(const Camera* camera, const GraphicObjectNode* node, HDC hdc);
	void renderScaleValue(const Camera* camera, HDC hdc);

	void renderFrameNum(HDC hdc);
	void renderFPS(HDC hdc);

	const SwapChainTarget* getSwapChain() const;


	//Member variables 
	bool isDirty;
	bool isRendering;

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

	ID3D11PixelShader* fallbackPS;
	ID3D11VertexShader* fallbackVS;
	ID3D11InputLayout* fallbackLayout;

	// Scene objects
	//TODO: Should probably put cameras in the scene
	RootSceneNode* rootScene;

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
	std::shared_ptr<RenderTarget> renderTargets[RenderTargetTypes::NumRT];
	std::shared_ptr<DepthTarget> depthTargets[DepthTargetTypes::NumDT];

	std::map<unsigned int,ID3D11DepthStencilState*> depthStencilStates;
	std::map<unsigned int,ID3D11RasterizerState*> rasterStates;
};
