#pragma once

#include <string>
#include "windows.h"

#define sendErrMessage(ans) { errMessage((ans), __FILE__, __LINE__); }
#define sendHrErrMessage(ans) { hrErrMessage((ans), __FILE__, __LINE__); }

void errMessage(std::string message, const char *file, int line);
void hrErrMessage(HRESULT hr, const char *file, int line);
