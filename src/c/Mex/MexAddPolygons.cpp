#include "MexCommand.h"
#include "Globals.h"
#include "QueuePolygon.h"

#include "LoadMessages.h"

#include <vector>

void MexAddPolygons::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	const mxArray* mxPolygons = prhs[0];

	size_t numPolygons = mxGetNumberOfElements(mxPolygons);

	MessageLoadPolys* loadMsg = new MessageLoadPolys(numPolygons);
	for(size_t i = 0; i<numPolygons; ++i)
	{
		mxArray* mxFaces = mxGetField(mxPolygons, i, "faces");
		mxArray* mxVerts = mxGetField(mxPolygons, i, "verts");
		mxArray* mxNorms = mxGetField(mxPolygons, i, "norms");
		mxArray* mxColor = mxGetField(mxPolygons, i, "color");
		mxArray* mxFrame = mxGetField(mxPolygons, i, "frame");
		mxArray* mxIndex = mxGetField(mxPolygons, i, "index");
		mxArray* mxLabel = mxGetField(mxPolygons, i, "label");

		size_t numFaces = mxGetM(mxFaces);
		size_t numVerts = mxGetM(mxVerts);
		size_t numNormals = mxGetM(mxNorms);

		char buff[256];
		if(numVerts<1)
		{
			sprintf(buff, "There are no verts for polygon for %zd!", i+1);
			mexErrMsgTxt(buff);
		}

		if(numFaces<1)
		{
			sprintf(buff, "There are no faces for polygon for %zd!", i+1);
			mexErrMsgTxt(buff);
		}

		if(numNormals<1)
		{
			sprintf(buff, "There are no normals for polygon for %zd!", i+1);
			mexErrMsgTxt(buff);
		}

		if(numNormals!=numVerts)
		{
			sprintf(buff, "Number of verts does not match the number of normals for %zd!", i+1);
			mexErrMsgTxt(buff);
		}

		double* faceData = (double*)mxGetData(mxFaces);
		double* vertData = (double*)mxGetData(mxVerts);
		double* normData = (double*)mxGetData(mxNorms);
		double* colorData = (double*)mxGetData(mxColor);
		int frame = MAT_TO_C(int(mxGetScalar(mxFrame)));

		mxGetString(mxLabel, buff, 256);


		QueuePolygon* newPoly = new QueuePolygon(numFaces, numVerts, numNormals, frame, (int)mxGetScalar(mxIndex), buff);
		newPoly->setfaceData(faceData);
		newPoly->setvertData(vertData);
		newPoly->setnormData(normData);
		newPoly->setcolorData(colorData);

		loadMsg->addPoly(newPoly);
	}

	gMsgQueueToDirectX.pushMessage(loadMsg);
}

std::string MexAddPolygons::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(gRenderer==NULL)
		return "No renderer instantiated!";

	if (nrhs != 1)
		return "Not the right arguments for loadPolygons!";

	const mxArray* polys = prhs[0];
	if(mxGetField(polys, 0, "faces")==NULL)
		return "Polygon struct is malformed, no faces!";
	if(mxGetField(polys, 0, "verts")==NULL)
		return "Polygon struct is malformed, no verts!";
	if(mxGetField(polys, 0, "norms")==NULL)
		return "Polygon struct is malformed, no norms!";
	if(mxGetField(polys, 0, "color")==NULL)
		return "Polygon struct is malformed, no color!";
	if(mxGetField(polys, 0, "frame")==NULL)
		return "Polygon struct is malformed, no frame!";
	if(mxGetField(polys, 0, "index")==NULL)
		return "Polygon struct is malformed, no index!";
	if(mxGetField(polys, 0, "label")==NULL)
		return "Polygon struct is malformed, no label!";

	return "";
}

void MexAddPolygons::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("polygonsStruct");
}

void MexAddPolygons::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This loads polygon data into the viewer to be integrated into the texture display.");

	helpLines.push_back("\tPolygonsStruct is an array of polygon structures.");
	helpLines.push_back("\t\tEach polygon needs to have the following fields:");
	helpLines.push_back("\t\t\tfaces - This is an ordered list of verticies where each group of three represent a triangle face.");
	helpLines.push_back("\t\t\tverts - This is a list of verticies that represent the exterior of polygon.");
	helpLines.push_back("\t\t\tnorms - This is a list where each group of three make up a vector that represents the normal of the corresponding face.");
	helpLines.push_back("\t\t\tcolor - This is three doubles in the range [0,1] that represent the (r,g,b) color.");
	helpLines.push_back("\t\t\t\tThis can also be three times as long as the vert list where each triple corresponds to a color for each vertex.");
	helpLines.push_back("\t\t\tframe - This is the frame in which the polygon will be displayed on.");
	helpLines.push_back("\t\t\tindex - This is a unique identifier and will error if a polygon with the same index exist.");
	helpLines.push_back("\t\t\t\tThis index can be used to show only certain polygons.  When a polygon is right clicked on, this is the number that will be returned.");
	helpLines.push_back("\t\t\tlabel - This is a string that will be displayed next to the polygon for reference.");
}