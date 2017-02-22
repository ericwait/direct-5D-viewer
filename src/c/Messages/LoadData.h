#pragma once
#include "D3d/SceneNode.h"
#include "D3d/MeshPrimitive.h"

#include <vector>
#include <set>

std::shared_ptr<MeshPrimitive> createPolygonMesh(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals, const Color& color);

void clearTextureFrame(int frame, GraphicObjectTypes typ);
void clearAllTextures(GraphicObjectTypes type);

HRESULT createBorder(Vec<float> &scale);
HRESULT initVolume(int numFrames, int numChannels, Vec<size_t> dims, Vec<float> physicalSize, bool columnMajor);
HRESULT loadTextureFrame(GraphicObjectTypes typ, int frame, unsigned char* image);
HRESULT loadVolumeTexture(unsigned char* image, GraphicObjectTypes typ);

void attachWidget(double* arrowFaces, size_t numArrowFaces, double* arrowVerts, size_t numArrowVerts, double* arrowNorms, size_t numArrowNorms,
	double* sphereFaces, size_t numSphereFaces, double* sphereVerts, size_t numSphereVerts, double* sphereNorms, size_t numSphereNorms);

