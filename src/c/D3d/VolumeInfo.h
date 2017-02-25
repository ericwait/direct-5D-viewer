#pragma once

#include "Renderer.h"
#include "MaterialParams.h"

// This is a helper class that keeps track of information related to 
// the currently loaded volume (created using InitTexture call)
class VolumeInfo
{
public:
	VolumeInfo(Renderer* renderer, int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physSize, bool columnMajor = false);

	int getFrames() const { return numFrames; }
	int getChannels() const { return numChannels; }
	Vec<float> getPhysSize() const { return physSize; }
	Vec<size_t> getDims() const { return dims; }

	std::shared_ptr<VolumeParams> getParams(GraphicObjectTypes type) const;
	GraphicObjectNode* createNode(GraphicObjectTypes type, int frame, const unsigned char* imageData = NULL) const;


	void setFrames(int framesIn)
	{
		numFrames = framesIn;
	}

	void setChannels(int channelsIn)
	{
		numChannels = channelsIn;
	}

	void setPhysSize(Vec<float> physSizeIn)
	{
		physSize = physSizeIn;
		updateImToModel();
	}

	void setDims(Vec<size_t> dimsIn)
	{
		dims = dimsIn;
		updateImToModel();
	}



	// Convert from image to normalized model space
	template <typename T>
	void imageToModelSpace(T* verts, size_t numVerts) const
	{
		for ( size_t i = 0; i < numVerts; ++i )
		{
			Eigen::Vector4f newVert((T)verts[i], (T)verts[i+numVerts], (T)verts[i+2*numVerts], 1.0f);
			newVert = imToModel * newVert;

			verts[i] = newVert[0];
			verts[i+numVerts] = newVert[1];
			verts[i+2*numVerts] = newVert[2];
		}
	}

private:
	VolumeInfo(){}
	VolumeInfo(const VolumeInfo& other){}

	// Create an Eigen transform for converting from image to model space
	void updateImToModel();

	template <typename T>
	std::shared_ptr<VolumeParams> createParams(GraphicObjectTypes type, int numChannels)
	{
		int volType = type - GraphicObjectTypes::OriginalVolume;

		sharedParams[volType] = std::make_shared<T>(renderer, numChannels);
		return sharedParams[volType];
	}

	template <typename T>
	std::shared_ptr<MeshPrimitive> createMesh()
	{
		volumeMesh = std::make_shared<T>(renderer, dims, physSize);
		return volumeMesh;
	}

	template <typename T>
	std::shared_ptr<Material> createMaterial(GraphicObjectTypes type) const
	{
		std::shared_ptr<T::ParamType> params = std::dynamic_pointer_cast<T::ParamType>(getParams(type));
		std::shared_ptr<T> material = std::make_shared<T>(renderer, numChannels, dims, params);
		return material;
	}


private:
	Renderer* renderer;

	// Is this a column-major volume
	bool columnMajor;

	int numFrames;
	int numChannels;

	// Total size of volume in physical units
	Vec<float> physSize;
	// Number of voxels in volume
	Vec<size_t> dims;

	Eigen::Matrix4f imToModel;

	// Shared parameters (transfer function, etc.) for rendering volume frames
	std::shared_ptr<VolumeParams> sharedParams[GraphicObjectTypes::NumGO - GraphicObjectTypes::OriginalVolume];

	// Shared volume mesh for rendering
	std::shared_ptr<MeshPrimitive> volumeMesh;
};
