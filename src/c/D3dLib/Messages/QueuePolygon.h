#pragma once

#include <string>

class QueuePolygon
{
public:
	QueuePolygon(size_t numFaces, size_t numVerts, size_t numNormals, int frame, int index, std::string label);
	~QueuePolygon();

	void setfaceData(double* faceData);
	void setvertData(double* vertData);
	void setnormData(double* normData);
	void setcolorData(double* colorData);

	size_t getNumFaces() { return numFaces; }
	size_t getNumVerts() { return numVerts; }
	size_t getNumNormals() { return numNormals; }
	int getFrame() { return frame; }
	int getIndex() { return index; }
	std::string getLabel() { return label; }

	double* getfaceData();
	double* getvertData();
	double* getnormData();
	double* getcolorData();

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