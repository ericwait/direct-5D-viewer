#pragma once

#include <string>

class QueuePolygon
{
public:
	QueuePolygon(size_t numFaces, size_t numVerts, size_t numNormals, int frame, int index, std::string label);
	~QueuePolygon();

	void QueuePolygon::setfaceData(double* faceData);
	void QueuePolygon::setvertData(double* vertData);
	void QueuePolygon::setnormData(double* normData);
	void QueuePolygon::setcolorData(double* colorData);

	size_t getNumFaces() { return numFaces; }
	size_t getNumVerts() { return numVerts; }
	size_t getNumNormals() { return numNormals; }
	int getFrame() { return frame; }
	int getIndex() { return index; }
	std::string getLabel() { return label; }

	double* QueuePolygon::getfaceData();
	double* QueuePolygon::getvertData();
	double* QueuePolygon::getnormData();
	double* QueuePolygon::getcolorData();

private:
	void setDefaults();

	double* faceData;
	double* vertData;
	double* normData;
	double* colorData;

	size_t numFaces;
	size_t numVerts;
	size_t numNormals;
	int frame;
	int index;
	std::string label;
};