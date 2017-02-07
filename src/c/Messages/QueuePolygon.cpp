#include "QueuePolygon.h"
#include "Global/Globals.h"
#include "D3d/MessageProcessor.h"

QueuePolygon::QueuePolygon(size_t numFaces, size_t numVerts, size_t numNormals, int frame, int index, std::string label)
{
	setDefaults();

	this->numFaces = numFaces;
	this->numVerts = numVerts;
	this->numNormals = numNormals;
	this->frame = frame;
	this->index = index;
	this->label = label;

	faceData = new double[numFaces * 3];
	vertData = new double[numVerts * 3];
	normData = new double[numVerts * 3];
	colorData = new double[numVerts * 3];
}

void QueuePolygon::setfaceData(double* faceData)
{
	memcpy(this->faceData, faceData, numFaces * 3 * sizeof(double));
}

void QueuePolygon::setvertData(double* vertData)
{
	memcpy(this->vertData, vertData, numVerts * 3 * sizeof(double));
	gRenderer->imageToModelSpace(this->vertData, numVerts);
}

void QueuePolygon::setnormData(double* normData)
{
	memcpy(this->normData, normData, numVerts * 3 * sizeof(double));
}

void QueuePolygon::setcolorData(double* colorData)
{
	// Possibly going out of memory space here
	memcpy(this->colorData, colorData, 4 * sizeof(double));
}

double* QueuePolygon::getfaceData()
{
	return faceData;
}

double* QueuePolygon::getvertData()
{
	return vertData;
}

double* QueuePolygon::getnormData()
{
	return normData;
}

double* QueuePolygon::getcolorData()
{
	return colorData;
}

QueuePolygon::~QueuePolygon()
{
	SAFE_DELETE(faceData);
	SAFE_DELETE(vertData);
	SAFE_DELETE(normData);
	SAFE_DELETE(colorData);

	setDefaults();
}

void QueuePolygon::setDefaults()
{
	double* faceData = NULL;
	double* vertData = NULL;
	double* normData = NULL;
	double* colorData = NULL;

	size_t numFaces = 0;
	size_t numVerts = 0;
	size_t numNormals = 0;
	int frame = -1;
	int index = -1;
	char* label = NULL;
}
