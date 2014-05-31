#pragma once
#include "Vec.h"
#include <d3d11.h>
#include <vector>
#include <map>
#include <string>

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

	Renderer();
	~Renderer();

	HRESULT init();
	void render();

	HRESULT addToRenderList(RendererPackage* rendererPackage, Section section);
	HRESULT removeFromRenderList(RendererPackage* rendererPackage, Section section);
	HRESULT clearRenderList(Section section);


	HRESULT createVertexBuffer(std::vector<Vertex>& verts, ID3D11Buffer** vertexBufferOut);
	HRESULT createIndexBuffer(std::vector<Vec<unsigned int>>& faces, ID3D11Buffer** indexBufferOut);
	HRESULT createConstantBuffer(size_t size, ID3D11Buffer** constBufferOut);

	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		std::vector<Vec<float>> textureUV, VertexShaders shader);
	MeshPrimitive* addMeshPrimitive(std::vector<Vec<unsigned int>>& faces, std::vector<Vec<float>>& vertices, std::vector<Vec<float>>& normals,
		VertexShaders shader);

	void updateShaderParams(void* params, ID3D11Buffer* buffer);

	int getVertexShader(const std::string& shaderFilename, const std::string& shaderFunction);
	int getPixelShader(const std::string& shaderFilename, const std::string& shaderFunction);

private:
	void initTranforms();
	HRESULT initSwapChain();
	HRESULT initDepthStencils();
	HRESULT initRenderTarget();
	HRESULT resetViewPort();
	void initShaders();
	HRESULT initRasterizerStates();
	void initMeshes();
	void createTexture();

	void releaseDepthStencils();
	void releaseRenderTarget();

	HRESULT compileVertexShader(const wchar_t* fileName, const char* shaderFunctionName, ID3D11VertexShader** vertexShaderOut, ID3D11InputLayout** vertexLayoutOut);
	HRESULT compilePixelShader(const wchar_t* fileName, const char* shaderFunctionName, ID3D11PixelShader** pixelShaderOut);



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

const std::string VERTEX_SHADER_FILENAMES[Renderer::VertexShaders::End] = {"Test1.fx", "Test2.fx"};
const std::string VERTEX_SHADER_FUNCNAMES[Renderer::VertexShaders::End] = {"Test1", "Test2"};
