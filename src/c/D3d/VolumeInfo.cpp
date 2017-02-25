#include "VolumeInfo.h"

#include "SceneNode.h"
#include "MeshPrimitive.h"
#include "Material.h"
#include "MaterialParams.h"


VolumeInfo::VolumeInfo(Renderer* renderer, int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physSize, bool columnMajor)
	: renderer(renderer), numFrames(numFrames), numChannels(numChannels), dims(dims), physSize(physSize), columnMajor(columnMajor)
{
	updateImToModel();

	volumeMesh = createMesh<ViewAlignedPlanes>();

	for ( int i=GraphicObjectTypes::OriginalVolume; i < GraphicObjectTypes::NumGO; ++i )
		createParams<StaticVolumeParams>((GraphicObjectTypes)i, numChannels);
}

std::shared_ptr<VolumeParams> VolumeInfo::getParams(GraphicObjectTypes type) const
{
	int volType = type - GraphicObjectTypes::OriginalVolume;
	return sharedParams[volType];
}

GraphicObjectNode* VolumeInfo::createNode(GraphicObjectTypes type, int frame, const unsigned char* imageData) const
{
	// Create a new material and graphics node and initialize texture data if available
	std::shared_ptr<Material> material = createMaterial<StaticVolumeTextureMaterial>(type);

	GraphicObjectNode* node = new GraphicObjectNode(frame, type, volumeMesh, material);

	if ( imageData == NULL )
		return node;

	// Attach the texture data if it was passed in.
	for ( int c=0; c < numChannels; ++c )
	{
		const unsigned char* imChan = imageData + c*dims.product();
		node->getMaterial()->attachTexture(c, std::make_shared<ConstTexture>(renderer, dims, imChan));
	}

	return node;
}

void VolumeInfo::updateImToModel()
{
	Vec<float> dimsf = getDims();
	Vec<float> sizef = getPhysSize();

	Vec<float> normalizeFactor = Vec<float>(2.0f) / dimsf;
	Vec<float> anisotropicFactor = sizef / sizef.maxValue();

	// TODO: Use this correction for left-handedness of column-major data buffers?
	Eigen::Matrix4f cmCorrection(Eigen::Matrix4f::Identity());
	if ( columnMajor )
		cmCorrection.row(0).swap(cmCorrection.row(1));

	Eigen::Affine3f transform = Eigen::Scaling(anisotropicFactor.y, anisotropicFactor.x, anisotropicFactor.z)
							* Eigen::Translation3f(-1.0f, -1.0f, -1.0f)
							* Eigen::Scaling(normalizeFactor.y, normalizeFactor.x, normalizeFactor.z)
							* Eigen::Translation3f(-1.0f, -1.0f, -1.0f);

	imToModel = (transform).matrix();
}

