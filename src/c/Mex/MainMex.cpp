#include "..\Global\Globals.h"
#include "mex.h"
#include "..\D3d\MessageProcessor.h"
#include "MexFunctions.h"
#include "..\Messages\DirectXCommands.h"

/* This is the entry point for Matlab
http://www.mathworks.com/help/matlab/apiref/mexfunction.html
Left hand side args and right hand side args
eg: [x,y] = level_3d('init', arrowFaces, arrowVerts, arrowNorms, sphereFaces, sphereVerts, shereNorms);
This has nlhs = 2, plhs points to null for them
This has nrhs = 7, prhs points to the arguments
*/
void mexFunction1(int nlhs,mxArray* plhs[],int nrhs,const mxArray* prhs[])
{
	// If we get no arguments, or not a string array, we error
	if(nrhs<1 || !mxIsChar(prhs[0])) mexErrMsgTxt("Usage:\n");

	// Check if we have a mutex or not
	if(mexMessageMutex == NULL)
		mexMessageMutex = CreateMutex(NULL,FALSE,NULL); // Create one

	// Wait for the mutex, else we terminate
	DWORD waitTerm = WaitForSingleObject(mexMessageMutex,360000);
	if(waitTerm == WAIT_TIMEOUT)
	{
		//cleanUp();
		//mexErrMsgTxt("Closed down Lever 3-D because it was not responding for over 6 min!");
		return;
	}

	// 	if (messageLoopHandle)
	// 	{
	// 		checkRenderThread();
	// 	}

	// Parse out the command
	char* command = mxArrayToString(prhs[0]);

	try
	{
		// init command
		if(_strcmpi("init",command) == 0)
		{
			//initCommand(nrhs,prhs);
		}
		// close command
		else if(_strcmpi("close",command) == 0)
		{
			//closeCommand();
		}
		// poll command
		else if(_strcmpi("poll",command) == 0)
		{
			//pollCommand(nlhs,plhs);
		}
		else if(messageLoopHandle != NULL)
		{
// 			if(_strcmpi("loadTexture",command) == 0)
// 			{
// 				loadTextureCommand(prhs,nrhs);
// 			}
// 			else if(_strcmpi("peelUpdate",command) == 0)
// 			{
// 				peelUpdateCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("textureLightingUpdate",command) == 0)
// 			{
// 				textureLightingUpdateCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("textureAttenUpdate",command) == 0)
// 			{
// 				textureAttenUpdateCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("segmentationLighting",command) == 0)
// 			{
// 				segmentationLightingCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("play",command) == 0)
// 			{
// 				playCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("rotate",command) == 0)
// 			{
// 				rotateCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("showLabels",command) == 0)
// 			{
// 				showLabelsCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("resetView",command) == 0)
// 			{
// 				resetViewCommand();
// 			}
// 			else if(_strcmpi("captureSpinMovie",command) == 0)
// 			{
// 				captureSpinMovieCommand();
// 			}
// 			else if(_strcmpi("transferUpdate",command) == 0)
// 			{
// 				transferUpdateCommand(nrhs,nlhs,prhs);
// 			}
// 			else if(_strcmpi("viewTexture",command) == 0)
// 			{
// 				viewTextureCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("viewSegmentation",command) == 0)
// 			{
// 				viewSegmentationCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("wireframeSegmentation",command) == 0)
// 			{
// 				wireframeSegmentationCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("loadPolygons",command) == 0)
// 			{
// 				loadPolygonsCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("removePolygon",command) == 0)
// 			{
// 				removePolygonCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("displayPolygons",command) == 0)
// 			{
// 				displayPolygonsCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("setFrame",command) == 0)
// 			{
// 				setFrameCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("setViewOrigin",command) == 0)
// 			{
// 				setViewOriginCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("updatePolygons",command) == 0)
// 			{
// 				updatePolygonsCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("addPolygons",command) == 0)
// 			{
// 				addPolygonsCommand(nrhs,prhs);
// 			}
// 			else if(_strcmpi("takeControl",command) == 0)
// 			{
// 				takeControlCommand();
// 			}
// 			else if(_strcmpi("releaseControl",command) == 0)
// 			{
// 				releaseControlCommand();
// 			}
// 			else if(_strcmpi("captureImage",command) == 0)
// 			{
// 				captureImageCommand(nlhs,nrhs,prhs,plhs);
// 			}
// 			else if(_strcmpi("deleteAllPolygons",command) == 0)
// 			{
// 				deleteAllPolygonsCommand();
// 			}
// 			else if(_strcmpi("captureWindow",command) == 0)
// 			{
// 				dataQueue->writeMessage("captureWindow",(void*)NULL);
// 			}
// 			else if(_strcmpi("setCapturePath", command)==0)
// 			{
// 				setCapturePathCommand(nrhs, prhs);
// 			}
// 			else if(_strcmpi("setRotation",command) == 0)
// 			{
// 				if(nrhs<3)
// 					mexErrMsgTxt("setRotation takes two arguments rotationX and rotationY!");
// 
// 				double* rotations = new double[2];
// 				rotations[0] = mxGetScalar(prhs[1]);
// 				rotations[1] = mxGetScalar(prhs[2]);
// 
// 				dataQueue->writeMessage(command,rotations);
// 			}
// 			else if(_strcmpi("setBackgroundColor", command)==0)
// 			{
// 				setBackColor(nrhs, prhs);
// 			}
// 			else
// 			{
// 				char buff[255];
// 				sprintf_s(buff,"%s is not a valid command!\n",command);
// 				mexErrMsgTxt(buff);
// 			}
		}
	}
	catch(const std::exception& e)
	{
		mxFree(command);
		ReleaseMutex(mexMessageMutex);
		mexErrMsgTxt(e.what());
	}
	catch(const std::string& e)
	{
		mxFree(command);
		ReleaseMutex(mexMessageMutex);
		mexErrMsgTxt(e.c_str());
	}
	catch(...)
	{
		mxFree(command);
		ReleaseMutex(mexMessageMutex);
		mexErrMsgTxt("Caught an unknown error!");
	}

	mxFree(command);

	ReleaseMutex(mexMessageMutex);
}