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
		Post
	};

	enum VertexShaders
	{
		Default,
		ViewAligned,
		End
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

	HRESULT addToRenderList(RendererPackage* rendererPackage, Section section);
	HRESULT removeFromRenderList(RendererPackage* rendererPackage);
	void clearRenderList(Section section);
	void clearVertexShaderList();
	void clearPixelShaderList();

	HRESULT createVertexBuffer(std::vector<Vertex>& verts, ID3D11Buffer** vertexBufferOut);
	HRESULT createIndexBuffer(std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut);
	HRESULT createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut);

	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		std::vector<Vec<float>> textureUV, VertexShaders shader);
	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		VertexShaders shader);

	void updateShaderParams(const void* params, ID3D11Buffer* buffer);
	void setPixelShaderConsts(ID3D11Buffer* buffer);

	int getVertexShader(const std::string& shaderFilename, const std::string& shaderFunction);
	int getPixelShader(const std::string& shaderFilename, const std::string& shaderFunction);

	void preRenderLoop();
	void mainRenderLoop();
	void postRenderLoop();
	void startRender();
	void endRender();

private:
	HRESULT initSwapChain();
	HRESULT initDepthStencils();
	HRESULT initRenderTarget();
	HRESULT resetViewPort();
	HRESULT initRasterizerStates();

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

	void renderPackage(RendererPackage* package);

	Vec<float> backgroundColor;

	HANDLE mutexDevice;
	IDXGISwapChain*	swapChain;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* immediateContext;
	ID3D11RenderTargetView* renderTargetView;
	IDXGISurface1* IDXGIBackBuffer;	

	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilStateCompareAlways;
	ID3D11DepthStencilState* depthStencilStateCompareLess;
	
	ID3D11RasterizerState* rasterizerStateNoClip;
	ID3D11RasterizerState* rasterizerStateWireClip;
	ID3D11RasterizerState* rasterizerStateFillClip;

	ID3D11Buffer* vertexShaderConstBuffer;

	std::vector<MeshPrimitive*> meshPrimitives;
	
	std::map<std::string,int> pixelShaderMap;
	std::map<std::string,int> vertexShaderMap;

	std::vector<ID3D11PixelShader*> pixelShaderList;
	std::vector<ID3D11VertexShader*> vertexShaderList;
	std::vector<ID3D11InputLayout*> vertexLayoutList;

	std::vector<RendererPackage*> renderPreList;
	std::vector<RendererPackage*> renderMainList;
	std::vector<RendererPackage*> renderPostList;

	std::map<RendererPackage*,int> renderPreListMap;
	std::map<RendererPackage*,int> renderMainListMap;
	std::map<RendererPackage*,int> renderPostListMap;
};

const std::string VERTEX_SHADER_FILENAMES[Renderer::VertexShaders::End] = {"DefaultMeshShaders.fx"};
const std::string VERTEX_SHADER_FUNCNAMES[Renderer::VertexShaders::End] = {"DefaultMeshVertexShader"};
