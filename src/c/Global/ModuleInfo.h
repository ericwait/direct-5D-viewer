#pragma once

#include "windows.h"
#undef min
#undef max

#include <string>

// Static class for holding some module information
class ModuleInfo
{
public:
	static void setModuleHandle(HMODULE handle)
	{
		hModule = handle;
	}

	static void initModuleInfo()
	{
		if(name.length()==0)
			name = initName();
	}

	static const std::string& getName()
	{
		return name;
	}

	static HINSTANCE getInstance()
	{
		return (HINSTANCE)hModule;
	}

private:
	static std::string initName()
	{
		if(hModule==NULL)
			return "";

		char pathBuffer[1024];
		DWORD result = GetModuleFileName((HMODULE)hModule, pathBuffer, 1024);
		if(FAILED(result))
			return "";

		std::string path(pathBuffer);

		size_t startOffset = path.find_last_of('\\')+1;
		path = path.substr(startOffset);

		size_t endOffset = path.find_last_of('.');

		return path.substr(0, endOffset);
	}

private:
	static std::string name;
	static HMODULE hModule;
};
