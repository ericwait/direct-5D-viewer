#include "QueuePolygon.h"
#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"

QueuePolygon::QueuePolygon(size_t numFaces, size_t numVerts, size_t numNormals, int frame, int label, int track){
	this->numFaces = numFaces;
	this->numVerts = numVerts;
	this->numNormals = numNormals;
	this->frame = frame;
	this->label = label;
	this->track = track;

	faceData = (double*)malloc(numFaces*3*sizeof(double));
	vertData = (double*)malloc(numVerts*3*sizeof(double));
	normData = (double*)malloc(numVerts*3*sizeof(double));
	colorData = (double*)malloc(numVerts*3*sizeof(double));
}

void QueuePolygon::setfaceData(double* faceData){
	memcpy(this->faceData, faceData, numFaces * 3 * sizeof(double));
}

void QueuePolygon::setvertData(double* vertData){
	memcpy(this->vertData, vertData, numVerts * 3 * sizeof(double));
}

void QueuePolygon::setnormData(double* normData){
	memcpy(this->normData, normData, numVerts * 3 * sizeof(double));
}

void QueuePolygon::setcolorData(double* colorData){
	memcpy(this->colorData, colorData, numVerts * 3 * sizeof(double));
}

double* QueuePolygon::getfaceData(){
	return faceData;
}

double* QueuePolygon::getvertData(){
	return vertData;
}

double* QueuePolygon::getnormData(){
	return normData;
}

double* QueuePolygon::getcolorData(){
	return colorData;
}

QueuePolygon::~QueuePolygon(){
	free(faceData);
	free(vertData);
	free(normData);
	free(colorData);
}