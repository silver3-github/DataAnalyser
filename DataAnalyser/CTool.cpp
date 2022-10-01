#include "pch.h"
#include "CTool.h"
#include <string>

bool CTool::IsValidCppName(const char* strName)
{
	//字符长度不能超过 MAX_PATH
	if (strlen(strName) + 1 > MAX_PATH)return false;

	//首字母不能是数字
	if (strName[0] >= 65 && strName[0] <= 90 //大写字母
		|| strName[0] >= 97 && strName[0] <= 122 //小写字母
		|| strName[0] == 95); //下划线
	else return false;

	//必须是字母、数字、下划线
	for (int i = 1; strName[i]; i++)
	{
		if (strName[i] >= 65 && strName[i] <= 90 //大写字母
			|| strName[i] >= 97 && strName[i] <= 122 //小写字母
			|| strName[i] >= 48 && strName[i] <= 57 //数字
			|| strName[i] == 95); //下划线
		else return false;
	}

	//不能是c++关键字
	char* keywords[62]
	{
		"if","else", "while","signed", "throw", "union", "this",
		"int", "char", "double","unsigned","const","goto","virtual",
		"for", "float", "break","auto","class","operator","case",
		"do", "long", "typedef","static","friend","template","default",
		"new", "void", "register","extern","return","enum","inline",
		"try", "short", "continue","sizeof","switch","private","protected",
		"asm", "const_cast", "catch","delete","public","volatile","struct",
		"dynamic_cast","explicit","export","mutable","namespace","reinterpret_cast",
		"static_cast","true","false","typeid","typename","using","wchar_t"
	};
	for (int i = 0; i < 62; i++)
	{
		if (!strcmp(strName, keywords[i]))return false;
	}
	return true;
}

void CTool::DeleteAllFiles(const char* directory)
{
	std::string dir = directory;
	if (dir.at(dir.length() - 1) != '\\') dir += '\\';
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFileA((dir + "*.*").c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE) return;
	do
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		else DeleteFileA((dir + wfd.cFileName).c_str());
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);
	return;
}

void CTool::MessageBoxA(const char* format, ...)
{
	ATLASSERT(AtlIsValidString(format));

	va_list argList;
	va_start(argList, format);
	CString txt;
	txt.FormatV(format, argList);
	::MessageBoxA(NULL, txt, "CTool::MessageBoxA", MB_DEFAULT_DESKTOP_ONLY);
	va_end(argList);
}