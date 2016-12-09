#pragma once
#include "D3d/SceneNode.h"

#include "mex.h"

#include <concrt.h>
#include <vector>
#include <set>
#include <memory>

extern HANDLE mexMessageMutex;
extern HANDLE messageLoopHandle;
extern bool registerExitFunction;
extern std::vector<SceneNode*> hullRootNodes;
extern DWORD threadID;

extern "C" __declspec(dllexport) void exitFunc();

void setCurrentTexture(GraphicObjectTypes textureType);
void togglePolygons(bool on);
void toggleWidget(bool on);
void togglePolygonWireframe(bool wireframe);
void togglePolygonLighting(bool lighting);
void toggleSelectedPolygon(std::set<int> labels);
HRESULT updatePolygons(const mxArray* hulls);

HRESULT loadPolygons(const mxArray* hulls);
std::shared_ptr<MeshPrimitive> createPolygonMesh(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals);

HRESULT createBorder(Vec<float> &scale);
HRESULT loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ);

HRESULT loadWidget(const mxArray* widget[]);

