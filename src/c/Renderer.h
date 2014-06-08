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
		Default,
		ViewAligned,
		VertexShadersEnd
	};

	struct VertexShaderConstBuffer
	{
		DirectX::XMMATRIX worldTransform;
		DirectX::XMMATRIX viewTransform;
		DirectX::XMMATRIX projectionTransform;
	};

	Renderer();
	~Renderer();

	HRESULT init();
	void renderAll();
	void attachToRootScene(SceneNode* sceneIn, Section section);
	void clearVertexShaderList();
	void clearPixelShaderList();

	HRESULT createVertexBuffer(std::vector<Vertex>& verts, ID3D11Buffer** vertexBufferOut);
	HRESULT createIndexBuffer(std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut);
	HRESULT createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut);
	ID3D11ShaderResourceView* createTextureResourceView(Vec<size_t> dims, unsigned char* image);

	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		std::vector<Vec<float>> textureUV, VertexShaders shader);
	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		VertexShaders shader);

	void updateShaderParams(const void* params, ID3D11Buffer* buffer);
	void setPixelShaderConsts(ID3D11Buffer* buffer);
	void setPixelShaderResourceViews(int startIdx, int length, ID3D11ShaderResourceView** shaderResourceView);
	void setPixelShaderTextureSamplers(int startIdx, int length, ID3D11SamplerState** samplerState);

	int getVertexShader(const std::string& shaderFilename, const std::string& shaderFunction);
	int getPixelShader(const std::string& shaderFilename, const std::string& shaderFunction, const std::string& shaderParams);
	ID3D11SamplerState* getSamplerState();

	void preRenderLoop();
	void mainRenderLoop();
	void postRenderLoop();
	void startRender();
	void endRender();

	void setRootWorldTransform(DirectX::XMMATRIX worldTransform);
	DirectX::XMMATRIX getRootWorldTransorm();

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

	void renderPackage(const RendererPackage* package);

	Vec<float> backgroundColor;

	HANDLE mutexDevice;
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
};

const std::string VERTEX_SHADER_FILENAMES[Renderer::VertexShaders::VertexShadersEnd] = {"DefaultMeshShaders.fx","ViewAlignedVertexShader.fx"};
const std::string VERTEX_SHADER_FUNCNAMES[Renderer::VertexShaders::VertexShadersEnd] = {"DefaultMeshVertexShader","ViewAlignedVertexShader"};
