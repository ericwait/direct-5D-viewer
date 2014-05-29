#pragma once
#include "windows.h"

DWORD WINAPI messageLoop(LPVOID lpParam);
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);