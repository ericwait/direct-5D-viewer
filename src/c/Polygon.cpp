#include "Polygon.h"
#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"

Polygon::Polygon(size_t numFaces, size_t numVerts){
	this->numFaces = numFaces;
	this->numVerts = numVerts;

	faceData = (double*)malloc(numFaces);
	vertData = (double*)malloc(numVerts);
	normData = (double*)malloc(numVerts);
	colorData = (double*)malloc(numVerts);
}

Polygon::~Polygon(){
	free(faceData);
	free(vertData);
	free(normData);
	free(colorData);
}