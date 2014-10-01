////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "mex.h"

#include "tracker.h"
#include "Hull.h"

void loadHulls( const mxArray * hulls, int numFrames ) 
{
	int numHulls = mxGetNumberOfElements(hulls);

	gHulls.clear();
	gHulls.resize(numHulls);
	gHashedHulls.clear();
	gHashedHulls.resize(numFrames);

	for(int hullIdx=0; hullIdx<numHulls; ++hullIdx)
	{
		Hull& hull = gHulls[hullIdx];
		mxArray* framePtr = mxGetField(hulls,hullIdx,"frame");
		if (framePtr==NULL) mexErrMsgTxt("Missing frame!\n");
		hull.setFrame(MatToC((unsigned int)mxGetScalar(framePtr)));

		mxArray* comPtr = mxGetField(hulls,hullIdx,"centerOfMass");
		if (comPtr==NULL) mexErrMsgTxt("Missing Center of Mass!\n");
		hull.setCenterOfMass((double*)mxGetData(comPtr));

		mxArray* boundingBoxPtr = mxGetField(hulls,hullIdx,"boundingBox");
		if (boundingBoxPtr==NULL) mexErrMsgTxt("Missing Bounding Box!\n");
		hull.setBoundingBox((double*)mxGetData(boundingBoxPtr));

		mxArray* colorPtr = mxGetField(hulls,hullIdx,"color");
		if (colorPtr==NULL) mexErrMsgTxt("Missing Color!\n");
		double* colorData = (double*)mxGetData(colorPtr);

		mxArray* pixelPtr = mxGetField(hulls,hullIdx,"pixels");
		if (pixelPtr==NULL) mexErrMsgTxt("Missing pixels!\n");

		size_t numPixels = mxGetM(pixelPtr);
		double* pixelData = (double*)mxGetData(pixelPtr);
		std::vector<PointType> pixelCoords;
		pixelCoords.resize(numPixels);

		for (int i=0; i<numPixels; ++i)
		{
			pixelCoords[i].x = pixelData[i];
			pixelCoords[i].y = pixelData[i+numPixels];
			pixelCoords[i].z = pixelData[i+2*numPixels];
		}

		hull.setInteriorVoxelIndices(pixelCoords);

		gHashedHulls[hull.getFrame()].insert(hullIdx);
	}
}

void returnHulls(mxArray* plhs[])
{
	const int NUM_FIELDS = 2;
	const char* fields[NUM_FIELDS];
	fields[0] = "track";
	fields[1] = "color";

	int numHulls = gHulls.size();
	plhs[0] = mxCreateStructMatrix(1,numHulls,NUM_FIELDS,fields);
	int maxTrackId = 0;

	for(int hullIdx=0; hullIdx<numHulls; ++hullIdx)
	{
		if (gHulls[hullIdx].getTrack()>maxTrackId)
			maxTrackId = gHulls[hullIdx].getTrack();

		mxArray* track = mxCreateDoubleScalar(CToMat(gHulls[hullIdx].getTrack()));
		mxSetField(plhs[0],hullIdx,"track",track);

		mxArray* colorPtr = mxCreateDoubleMatrix(1,3,mxREAL);
		double* colorData = (double*)mxGetData(colorPtr);
		gHulls[hullIdx].getColor(colorData);
		mxSetField(plhs[0],hullIdx,"color",colorPtr);
	}

	mxArray* numTracks = mxCreateDoubleScalar(CToMat(maxTrackId));
	plhs[1] = numTracks;

	int numEdges = 0;
	for (int i=0; i<numHulls; ++i)
	{
		numEdges += gTrackerData.connectOut[i].size();
	}

	mxArray* sparseArray = mxCreateSparse(numHulls,numHulls,numEdges,mxREAL);
	mwIndex* mxjc = mxGetJc(sparseArray);
	mwIndex* mxir = mxGetIr(sparseArray);
	double* mxpr = mxGetPr(sparseArray);

	mxjc[0] = 0;

	std::map<int,CSourcePath*>* inEdges;
	for (int i=0; i<numHulls; ++i)
	{
		inEdges = &gTrackerData.connectIn[i];
		mxjc[i+1] = mxjc[i]+inEdges->size();
		std::map<int,CSourcePath*>::iterator it = inEdges->begin();
		for (int j=0; j<inEdges->size(); ++j, ++it)
		{
			mxir[mxjc[i]+j] = it->first;
			mxpr[mxjc[i]+j] = it->second->cost;
		}
	}

	plhs[2] = sparseArray;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs!=4) mexErrMsgTxt("Usage:\n");// TODO make this useful
	if (nlhs!=3) mexErrMsgTxt("Not right LHS arguments!\n");

	int numFrames = mxGetScalar(prhs[0]);
	const mxArray* hulls = prhs[1];
	if (hulls==NULL) mexErrMsgTxt("No hulls passed as the second argument!\n");

	gVMax = mxGetScalar(prhs[2]);
	gCCMax = mxGetScalar(prhs[3]);

	loadHulls(hulls,numFrames);
	trackHulls(numFrames);
	returnHulls(plhs);

	destroyTrackStructures();
}