#include <iostream>
#include <Shlobj.h>
#include <io.h>
#include <direct.h>
#include <windows.h>


int main()
{
	//激活DataAnaly环境变量
	HKEY hkey;
	long result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
		0, KEY_ALL_ACCESS, &hkey);
	if (result != ERROR_SUCCESS)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//设置红色
		std::cout << "激活DataAnaly环境变量失败！请用管理员权限运行程序！\r\nDataAnaly库激活失败！" << std::endl;
		system("Pause");
		return 0;
	}

	int count = 0;
	char htdPath[MAX_PATH]{};
	GetModuleFileNameA(NULL, htdPath, MAX_PATH);
	for (int i = MAX_PATH - 1; i > -1; i--)
	{
		if (htdPath[i] == '\\') {
			count = i;
			htdPath[i] = '\0';
			break;
		}
	}
#ifdef _WIN64
	result = RegSetValueExA(hkey, "DataAnalyX64", 0, REG_SZ, (const BYTE*)htdPath, count + 1);
#else
	result = RegSetValueExA(hkey, "DataAnalyX32", 0, REG_SZ, (const BYTE*)htdPath, count + 1);
#endif
	if (result != ERROR_SUCCESS)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);//设置红色
		std::cout << "激活DataAnaly环境变量失败！请用管理员权限运行程序！\r\nDataAnaly库激活失败！" << std::endl;
		system("Pause");
		RegCloseKey(hkey);
		return 0;
	}
	RegCloseKey(hkey);

	SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		(LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, (PDWORD_PTR)&result);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);//设置绿色
	std::cout << "DataAnaly环境变量激活成功！\r\nDataAnaly库激活成功！" << std::endl;
	system("Pause");
	return 0;
}
