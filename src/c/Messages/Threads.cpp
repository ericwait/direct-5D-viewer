#include "Threads.h"
#include "Global/Globals.h"
#include "mex.h"
#include "D3d/MessageProcessor.h"
#include "Mex/MexFunctions.h"
#include "MexErrorMsg.h"

void startThread(std::string rootDir)
{
	if (gTermEvent == NULL)
	{
		gTermEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!gTermEvent)
			mexErrMsgTxt("Could not create thread-monitor event");
	}

	if (messageLoopHandle == NULL)
	{
		std::string* rootDirPtr = new std::string;
		*rootDirPtr = rootDir;

		messageLoopHandle = CreateThread(NULL, 0, messageLoop, (LPVOID)(rootDirPtr), 0, &threadID);
		if (!messageLoopHandle)
		{
			mexErrMsgTxt("Could not create thread");
		}

		while (gRendererInit == false)
			Sleep(1000);

		if (!checkRenderThread())
			sendErrMessage("Failed to initialize the renderer thread!");
	}
}

bool checkRenderThread()
{
	DWORD waitTerm = WaitForSingleObject(messageLoopHandle, 0);
	if (waitTerm == WAIT_OBJECT_0)
	{
		CloseHandle(messageLoopHandle);
		CloseHandle(gTermEvent);
		messageLoopHandle = NULL;
		gTermEvent = NULL;
		gRendererInit = false;

		return false;
	}

	return true;
}

void termThread()
{
	if (gTermEvent != NULL)
	{
		// Send thread a termination event
		SetEvent(gTermEvent);
		PostThreadMessage(threadID, WM_QUIT, (WPARAM)0, (LPARAM)0);
	}

	if (messageLoopHandle != NULL)
	{
		// Resume the thread in case it's suspended
		ResumeThread(messageLoopHandle);

		// Wait for thread termination/force termination if it times out
		DWORD waitTerm = WaitForSingleObject(messageLoopHandle, 300000);
		if (waitTerm == WAIT_TIMEOUT)
			TerminateThread(messageLoopHandle, -1000);

		CloseHandle(messageLoopHandle);
	}
	if (gTermEvent != NULL)
		CloseHandle(gTermEvent);

	messageLoopHandle = NULL;
	gTermEvent = NULL;
	gRendererInit = false;
}

void cleanUp()
{
	try
	{
		termThread();
	}
	catch (const std::exception& e)
	{
		mexErrMsgTxt(e.what());
	}
	catch (const std::string& e)
	{
		mexErrMsgTxt(e.c_str());
	}
	catch (...)
	{
		mexErrMsgTxt("Caught an unknown error!");
	}
}
