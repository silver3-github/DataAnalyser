#pragma once
#include <stdarg.h>

class CTool
{
public:
	static bool IsValidCppName(const char* strName);
	static void DeleteAllFiles(const char* directory);
	static void  MessageBoxA(const char* format, ...);
};

