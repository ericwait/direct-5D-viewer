#include "Vec.h"
#include "Globals.h"
#include "LoadData.h"
#include "MessageQueue.h"

#include "mex.h"
#include <windows.h>

#include <set>

HRESULT loadWidget(const mxArray* widget[])
{
	if (gRenderer == NULL) return E_FAIL;

	while (!gRendererInit)
		Sleep(10);

	//gRenderer->getMutex();

	size_t numArrowFaces = mxGetM(widget[0]);
	size_t numArrowVerts = mxGetM(widget[1]);
	size_t numArrowNormals = mxGetM(widget[2]);

	size_t numSphereFaces = mxGetM(widget[3]);
	size_t numSphereVerts = mxGetM(widget[4]);
	size_t numSphereNormals = mxGetM(widget[5]);

	if (numArrowVerts < 1)
		mexErrMsgTxt("No Verts!");

	if (numArrowFaces < 1)
		mexErrMsgTxt("No faces!");

	if (numArrowNormals < 1)
		mexErrMsgTxt("No norms!");

	if (numSphereVerts < 1)
		mexErrMsgTxt("No Verts!");

	if (numSphereFaces < 1)
		mexErrMsgTxt("No faces!");

	if (numSphereNormals < 1)
		mexErrMsgTxt("No norms!");

	double* arrowFaces = (double*)mxGetData(widget[0]);
	double* arrowVerts = (double*)mxGetData(widget[1]);
	double* arrowNorms = (double*)mxGetData(widget[2]);

	double* sphereFaces = (double*)mxGetData(widget[3]);
	double* sphereVerts = (double*)mxGetData(widget[4]);
	double* sphereNorms = (double*)mxGetData(widget[5]);

	attachWidget(arrowFaces, numArrowFaces, arrowVerts, numArrowVerts, arrowNorms, numArrowNormals,
				sphereFaces, numSphereFaces, sphereVerts, numSphereVerts, sphereNorms, numSphereNormals);

	//gRenderer->releaseMutex();

	return S_OK;
}