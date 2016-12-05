#include "Global/Globals.h"
#include "mex.h"
#include "D3d/MessageProcessor.h"
#include "MexFunctions.h"
#include "Messages/MessageQueue.h"
#include "Messages/TransferObj.h"
#include "D3d/TextureLightingObj.h"
#include "Messages/MessageQueue.h"
#include "Messages/Image.h"

HANDLE messageLoopHandle = NULL;
DWORD threadID;
HANDLE mexMessageMutex = NULL;
bool registerExitFunction = false;
DataQueue gMsgQueueToDirectX;


