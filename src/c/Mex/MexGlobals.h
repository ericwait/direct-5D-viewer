#pragma once
#include "windows.h"

#include <vector>

extern HANDLE messageLoopHandle;
extern bool registerExitFunction;
extern DWORD threadID;

extern "C" __declspec(dllexport) void exitFunc();
