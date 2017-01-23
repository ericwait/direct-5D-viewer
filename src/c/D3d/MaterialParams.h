#pragma once
#include "Renderer.h"

#include <DirectXMath.h>

#include <unordered_map>

class Material;

class MaterialParameters
{
	typedef unsigned char byte;

	class Param
	{
	public:
		Param(){};
		Param(const std::string& type, size_t offset, size_t size, const std::string& desc = "")
			: type(type), byteOffset(offset), byteSize(size), desc(desc)
		{}

		size_t size(){return byteSize;}
		template<typename T> T* getPtr(void* base){return ((T*) (((byte*) base) + byteOffset));}

	private:
		size_t byteSize;
		size_t byteOffset;

		std::string type;
		std::string desc;
	};

public:
	friend class Renderer;
	friend class Material;
	virtual ~MaterialParameters();

	virtual DirectX::XMFLOAT4 getColor(){return DirectX::XMFLOAT4(0.0f,0.0f,0.0f,0.0f);}

	void setLightOn(bool on);
	void setAttenuationOn(bool on);

	// Get a pointer for arrays
	template<typename T> T* ptr(const std::string& name){return getPtr<T>(name);}
	// Get a reference for single types
	template<typename T> T& ref(const std::string& name){return getRef<T>(name);}

private:
	// Disable creation of this without a renderer (force children to initialize properly)
	MaterialParameters(){};

protected:
	MaterialParameters(Renderer* rendererIn);

	// Handling parameters creation/destruction dynamic access
	template<typename T> T* getPtr(const std::string& name){return (paramHash[name].getPtr<T>(constMemory.data()));}
	template<typename T> T& getRef(const std::string& name){return *(paramHash[name].getPtr<T>(constMemory.data()));}

	template<typename T>
	void addParam(const std::string& name, const T& defaultValue = T(), const std::string& desc = "")
	{
		if ( paramHash.count(name) > 0 )
			return;

		size_t offset = constMemory.size();
		size_t size = sizeof(T);

		constMemory.resize(offset + size);
		paramHash.emplace(name, Param(typeid(T).name(),offset,size,desc));

		getRef<T>(name) = defaultValue;
	}

	template<typename T>
	void addParamArray(const std::string& name, size_t number, const std::string& desc = "")
	{
		if ( paramHash.count(name) > 0 )
			return;

		size_t offset = constMemory.size();
		size_t size = number * sizeof(T);

		char type[256];
		sprintf(type, "%s[%zu]", typeid(T).name(), number);

		constMemory.resize(offset + size);
		paramHash.emplace(name, Param(type,offset,size,desc));
	}

	void clearParams();

	// Render resources
	void createResources()
	{
		releaseResources();
		renderer->createConstantBuffer(constMemory.size(), &constBuffer);
	}

	void releaseResources(){SAFE_RELEASE(constBuffer);}

	void setShaderResources(){renderer->setPixelShaderConsts(constBuffer);}

	void updateParams()
	{
		// TODO: Use lazy resource creation for now, should probably figure out a better way post construction!
		if ( !constBuffer )
			createResources();

		renderer->updateShaderParams(constMemory.data(),constBuffer);
	}

	Renderer* renderer;
	ID3D11Buffer* constBuffer;

	std::unordered_map<std::string,Param> paramHash;
	std::vector<byte> constMemory;
};

class SingleColorParams: public MaterialParameters
{
public:
	SingleColorParams(Renderer* rendererIn);
	SingleColorParams(Renderer* rendererIn, Vec<float> colorIn, float alpha);

	void setColor(Vec<float> colorIn, float alpha);
	void setColorModifier(Vec<float> colorMod, float alphaMod);

	virtual DirectX::XMFLOAT4 getColor();
private:
	SingleColorParams() : MaterialParameters(NULL){};
};

// Intermediate parameter class for any volume renderer,
// Handles channel colors and transfer functions.
class VolumeParams : public MaterialParameters
{
public:
	void setTransferFunction(int channel, Vec<float> transferFunction);
	void setRange(int channel, Vec<float> ranges);

	void setColor(int channel, Vec<float> color, float alphaMod);

protected:
	VolumeParams(Renderer* rendererIn);
	VolumeParams(Renderer* rendererIn, int numChannelsIn);

	//NOTE: This will reset all transfer function data
	void resetChannels(int numChannelsIn);
	void setDefaultParams();

private:
	static const Vec<float> defaultColors[6];

	// Invalid constructor
	VolumeParams() : MaterialParameters(NULL){}

	int numChannels;
};


class StaticVolumeParams : public VolumeParams
{
public:
	StaticVolumeParams(Renderer* rendererIn);
	StaticVolumeParams(Renderer* rendererIn, int numChannelsIn);

	void setGradientSampleDir(Vec<float> xDir, Vec<float> yDir, Vec<float> zDir);

private:
	StaticVolumeParams() : VolumeParams(NULL,0){};
};
