#include "Globals.h"
#include "MessageProcessor.h"
#include "Threads.h"
#include <stdexcept>
#include <string>
#include <windows.h>

// Globals required by D3dLib (defined in Globals.h as extern)
// gTermEvent and gRendererInit are defined in D3dLib/Messages/ViewerGlobals.cpp
// gWindowHandle and gFont are defined in D3dLib/D3d/Initialization.cpp
// So we just use the declarations from Globals.h

// Helper globals from ViewerGlobals.cpp (but not in headers)
extern DWORD threadID;
extern HANDLE messageLoopHandle;

bool checkRenderThread() {
  DWORD waitTerm = WaitForSingleObject(messageLoopHandle, 0);
  if (waitTerm == WAIT_OBJECT_0) {
    CloseHandle(messageLoopHandle);
    CloseHandle(gTermEvent);
    messageLoopHandle = NULL;
    gTermEvent = NULL;
    gRendererInit = false;

    return false;
  }

  return true;
}

void startThread(std::string rootDir) {
  if (gTermEvent == NULL) {
    gTermEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!gTermEvent)
      throw std::runtime_error("Could not create thread-monitor event");
  }

  if (messageLoopHandle == NULL) {
    std::string *rootDirPtr = new std::string(rootDir);

    messageLoopHandle =
        CreateThread(NULL, 0, messageLoop, (LPVOID)(rootDirPtr), 0, &threadID);
    if (!messageLoopHandle) {
      throw std::runtime_error("Could not create thread");
    }

    // Wait for renderer to init (with timeout)
    int attempts = 100;
    while (gRendererInit == false && attempts > 0) {
      Sleep(100); // Wait 100ms
      attempts--;
    }

    if (gRendererInit == false) {
      // Try to clean up
      if (messageLoopHandle) {
        TerminateThread(messageLoopHandle, -1);
        CloseHandle(messageLoopHandle);
        messageLoopHandle = NULL;
      }
      throw std::runtime_error(
          "Renderer thread failed to initialize (timeout)");
    }

    if (!checkRenderThread())
      throw std::runtime_error(
          "Failed to initialize the renderer thread (thread check failed)!");
  }
}

void termThread() {
  if (gTermEvent != NULL) {
    // Send thread a termination event
    SetEvent(gTermEvent);
    PostThreadMessage(threadID, WM_QUIT, (WPARAM)0, (LPARAM)0);
  }

  if (messageLoopHandle != NULL) {
    // Resume the thread in case it's suspended
    ResumeThread(messageLoopHandle);

    // Wait for thread termination/force termination if it times out
    DWORD waitTerm =
        WaitForSingleObject(messageLoopHandle, 5000); // 5 seconds timeout
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

void cleanUp() {
  try {
    termThread();
  } catch (...) {
  }
}
