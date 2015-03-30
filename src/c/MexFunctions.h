#pragma once
#include <set>

extern HANDLE mexMessageMutex;
extern HANDLE messageLoopHandle;
extern bool registerExitFunction;
extern std::vector<VolumeTextureObject*> firstVolumeTextures;
extern std::vector<SceneNode*> hullRootNodes;
extern DWORD threadID;


void pollCommand(int nlhs, mxArray** plhs);
void initCommand(int nrhs, const mxArray** prhs);
void closeCommand();
void loadTextureCommand(const mxArray** prhs, int nrhs);
void peelUpdateCommand(int nrhs, const mxArray** prhs);
void textureLightingUpdateCommand(int nrhs, const mxArray** prhs);
void textureAttenUpdateCommand(int nrhs, const mxArray** prhs);
void segmentationLightingCommand(int nrhs, const mxArray** prhs);
void playCommand(int nrhs, const mxArray** prhs);
void rotateCommand(int nrhs, const mxArray** prhs);
void showLabelsCommand(int nrhs, const mxArray** prhs);
void resetViewCommand();
void captureSpinMovieCommand();
void transferUpdateCommand(int nrhs, int nlhs, const mxArray** prhs);
void viewTextureCommand(int nrhs, const mxArray** prhs);
void viewSegmentationCommand(int nrhs, const mxArray** prhs);
void wireframeSegmentationCommand(int nrhs, const mxArray** prhs);
void loadHullsCommand(int nrhs, const mxArray** prhs);
void displayHullsCommand(int nrhs, const mxArray** prhs);
void setFrameCommand(int nrhs, const mxArray** prhs);
void setViewOriginCommand(int nrhs, const mxArray** prhs);
void updateHullsCommand(int nrhs, const mxArray** prhs);
void addHullsCommand(int nrhs, const mxArray** prhs);
void setCapturePathCommand(int nrhs, const mxArray** prhs);
void takeControlCommand();
void releaseControlCommand();
void captureImageCommand(int nlhs, int nrhs, const mxArray** prhs, mxArray** plhs);
extern "C" void exitFunc();


void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
extern "C" void exitFunc();

void setCurrentTexture(GraphicObjectTypes textureType);
void toggleSegmentationResults(bool on);
void toggleSegmentaionWireframe(bool wireframe);
void toggleSegmentaionLighting(bool lighting);
void toggleSelectedCell(std::set<int> labels);
HRESULT updateHulls(const mxArray* hulls);
HRESULT addHulls(const mxArray* hulls);

CellHullObject* createCellHullObject(double* faceData, size_t numFaces, double* vertData, size_t numVerts, double* normData, size_t numNormals,
	Camera* camera);
HRESULT loadHulls(const mxArray* hulls);
HRESULT createBorder(Vec<float> &scale);
HRESULT loadVolumeTexture(unsigned char* image, Vec<size_t> dims, int numChannel, int numFrames, Vec<float> scales, GraphicObjectTypes typ);

HRESULT loadWidget(const mxArray* widget[]);

void startThread();
bool checkRenderThread();
void termThread();
void cleanUp();