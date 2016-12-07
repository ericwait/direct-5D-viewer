#pragma once
#include <concrt.h>
#include <vector>
#include <set>
#include "D3d/GraphicObject.h"

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


PolygonObject* createPolygonObject(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals,
	Camera* camera);
HRESULT loadPolygons(const mxArray* hulls);
HRESULT createBorder(Vec<float> &scale);
HRESULT loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ);

HRESULT loadWidget(const mxArray* widget[]);

