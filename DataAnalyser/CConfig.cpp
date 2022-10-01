#include "pch.h"
#include "CConfig.h"
#include "CTool.h"
#include <io.h>


//单例实例初始化

CConfig CConfig::Instance;


//构造和析构

CConfig::CConfig()
{
	//获取配置文件夹路径
	char buffer[MAX_PATH]{};
	int i = GetModuleFileNameA(NULL, buffer, MAX_PATH);//获取文件夹路径
	for (; 0 <= i; i--)if (buffer[i] == '\\')break;
	buffer[i + 1] = 0;
	folderPath = buffer;
	configFPath = folderPath + "ConfigFile\\";
	defineFPath = folderPath + "DefineFile\\";
	logFPath = folderPath + "Log\\";

	//检查配置文件夹
	if (_access(configFPath.c_str(), 0))
	{
		CreateDirectoryA(configFPath.c_str(), NULL);
	}
	if (_access(defineFPath.c_str(), 0))
	{
		CreateDirectoryA(defineFPath.c_str(), NULL);
	}
	if (_access(logFPath.c_str(), 0))
	{
		CreateDirectoryA(logFPath.c_str(), NULL);
	}

	//加载默认配置文件
	if (GetDefaultConfig(buffer, MAX_PATH))
	{
		LoadFromFile(buffer);
	}
}

CConfig::~CConfig()
{
}


//函数实现


//是否存在定义

bool CConfig::IsExistMsgDef(const char* msgName)
{
	return msgDefs.count(msgName);
}

bool CConfig::IsExistCmdDef(const char* msg, unsigned cmdId)
{
	std::map<std::string, MsgDefine>::iterator it;
	it = msgDefs.find(msg);
	if (it == msgDefs.end())return false;

	return it->second.cmdDefs.count(cmdId);
}


//添加定义

bool CConfig::AddMsgDef(const char* msgName, const char* handleMethod, bool isInput)
{
	if (msgDefs.count(msgName))return false;
	MsgDefine msgDef{ msgName,handleMethod,isInput };
	msgDefs.insert(std::pair<std::string, MsgDefine>(msgName, msgDef));
	return true;
}

bool CConfig::AddCmdDef(const char* msg, const char* cmdName, unsigned cmdId, bool isShow, bool isEdit, bool isInput, bool isAloneInput, COLORREF textColor, COLORREF bgColor)
{
	std::map<std::string, MsgDefine>::iterator it;
	it = msgDefs.find(msg);
	if (it == msgDefs.end())return false;

	if (it->second.cmdDefs.count(cmdId))return false;
	CmdDefine cmdDef{ cmdName, cmdId, isShow, isEdit, isInput, isAloneInput,textColor,bgColor };
	it->second.cmdDefs.insert(std::pair<DWORD, CmdDefine>(cmdId, cmdDef));
	return true;
}


//设置定义

bool CConfig::SetMsgDef(const char* msg, const char* msgName, const char* handleMethod, bool isInput)
{
	if (!IsExistMsgDef(msg))return false;//要修改的消息定义不存在
	if (strcmp(msg, msgName) && msgDefs.count(msgName))return false;//修改后的消息定义会重复定义

	msgDefs.erase(msg);
	MsgDefine msgDef{ msgName,handleMethod,isInput };
	msgDefs.insert(std::pair<std::string, MsgDefine>(msgName, msgDef));
	return true;
}

bool CConfig::SetCmdDef(const char* msg, unsigned cmd, const char* cmdName, unsigned cmdId, bool isShow, bool isEdit, bool isInput, bool isAloneInput, COLORREF textColor, COLORREF bgColor)
{
	if (!IsExistCmdDef(msg, cmd))return false;//要修改的命令定义不存在
	std::map<std::string, MsgDefine>::iterator it;
	it = msgDefs.find(msg);
	if (cmd != cmdId && it->second.cmdDefs.count(cmdId))return false;//修改后的命令定义会重复定义

	it->second.cmdDefs.erase(cmd);
	CmdDefine cmdDef{ cmdName, cmdId, isShow, isEdit, isInput, isAloneInput,textColor,bgColor };
	it->second.cmdDefs.insert(std::pair<DWORD, CmdDefine>(cmdId, cmdDef));
	return true;
}


//删除定义

bool CConfig::DeleteMsgDef(const char* msgName)
{
	if (!IsExistMsgDef(msgName))return true;

	return msgDefs.erase(msgName);
}

bool CConfig::DeleteCmdDef(const char* msg, unsigned cmdId)
{
	if (!IsExistCmdDef(msg, cmdId))return true;

	std::map<std::string, MsgDefine>::iterator it;
	it = msgDefs.find(msg);
	return it->second.cmdDefs.erase(cmdId);
}


//读写配置文件数据

bool CConfig::SaveToFile(const char* configFile)
{
	//创建文件夹
	if (_access(configFPath.c_str(), 0))
	{
		CreateDirectoryA(configFPath.c_str(), NULL);
	}

	//创建新文件
	std::string newFile = configFPath + configFile + ".ini";
	FILE* f;
	if (fopen_s(&f, newFile.c_str(), "w+"))return false;
	fclose(f);

	//写入配置数据
	CString appName, value;
	value.Format("%d", msgDefs.size());
	WritePrivateProfileStringA("main", "count", value, newFile.c_str());
	int msgCount = 0;
	for (auto msgIt : msgDefs)
	{
		appName.Format("MSGDEF%d", msgCount);
		WritePrivateProfileStringA(appName, "msgName", msgIt.second.msgName.c_str(), newFile.c_str());
		WritePrivateProfileStringA(appName, "handleMethod", msgIt.second.handleMethod.c_str(), newFile.c_str());
		WritePrivateProfileStringA(appName, "isInput", msgIt.second.isInput ? "1" : "0", newFile.c_str());
		value.Format("%d", msgIt.second.cmdDefs.size());
		WritePrivateProfileStringA(appName, "count", value, newFile.c_str());

		int cmdCount = 0;
		for (auto cmdIt : msgIt.second.cmdDefs)
		{
			appName.Format("MSGDEF%d_CMDDEF%d", msgCount, cmdCount);
			WritePrivateProfileStringA(appName, "cmdName", cmdIt.second.cmdName.c_str(), newFile.c_str());
			value.Format("%u", cmdIt.second.cmdId);
			WritePrivateProfileStringA(appName, "cmdId", value, newFile.c_str());
			WritePrivateProfileStringA(appName, "isShow", cmdIt.second.isShow ? "1" : "0", newFile.c_str());
			WritePrivateProfileStringA(appName, "isEdit", cmdIt.second.isEdit ? "1" : "0", newFile.c_str());
			WritePrivateProfileStringA(appName, "isInput", cmdIt.second.isInput ? "1" : "0", newFile.c_str());
			WritePrivateProfileStringA(appName, "isAloneInput", cmdIt.second.isAloneInput ? "1" : "0", newFile.c_str());
			value.Format("%u", cmdIt.second.textColor);
			WritePrivateProfileStringA(appName, "textColor", value, newFile.c_str());
			value.Format("%u", cmdIt.second.bgColor);
			WritePrivateProfileStringA(appName, "bgColor", value, newFile.c_str());
			cmdCount++;
		}

		msgCount++;
	}

	//删除旧文件
	if (this->configFile != configFile)
	{
		std::string oldFile = configFPath + this->configFile + ".ini";
		DeleteFileA(oldFile.c_str());
		this->configFile = configFile;
	}
	return true;
}

bool CConfig::LoadFromFile(const char* configFile)
{
	//判断配置文件是否存在
	std::string file = configFPath + configFile + ".ini";
	if (_access(file.c_str(), 0) == -1) return false;

	//清空数据
	msgDefs.clear();

	//读取配置数据
	this->configFile = configFile;
	CString appName, value1, value2;
	int msgCount = GetPrivateProfileIntA("main", "count", 0, file.c_str());
	for (int i = 0; i < msgCount; i++)
	{
		appName.Format("MSGDEF%d", i);
		char buffer[MAX_PATH]{};
		GetPrivateProfileStringA(appName, "msgName", appName, buffer, MAX_PATH, file.c_str());
		value1 = buffer;
		GetPrivateProfileStringA(appName, "handleMethod", "数据包", buffer, MAX_PATH, file.c_str());
		value2 = buffer;
		MsgDefine msgDef
		{
			value1,
			value2,
			GetPrivateProfileIntA(appName, "isInput", false, file.c_str())
		};
		msgDefs[msgDef.msgName] = msgDef;


		int cmdCount = GetPrivateProfileIntA(appName, "count", 0, file.c_str());
		for (int j = 0; j < cmdCount; j++)
		{
			appName.Format("MSGDEF%d_CMDDEF%d", i, j);
			GetPrivateProfileStringA(appName, "cmdName", "Undefined", buffer, MAX_PATH, file.c_str());
			CmdDefine cmdDef
			{
				buffer,
				GetPrivateProfileIntA(appName, "cmdId", -1, file.c_str()),
				GetPrivateProfileIntA(appName, "isShow", true, file.c_str()),
				GetPrivateProfileIntA(appName, "isEdit", false, file.c_str()),
				GetPrivateProfileIntA(appName, "isInput", false, file.c_str()),
				GetPrivateProfileIntA(appName, "isAloneInput", false, file.c_str()),
				GetPrivateProfileIntA(appName, "textColor", 0, file.c_str()),
				GetPrivateProfileIntA(appName, "bgColor", 0xFFFFFF, file.c_str())
			};
			msgDefs[msgDef.msgName].cmdDefs[cmdDef.cmdId] = cmdDef;
		}
	}

	//设置为默认配置文件
	SetDefaultConfig(configFile);
	return true;
}


//设置默认的配置文件

void CConfig::SetDefaultConfig(const char* configFile)
{
	//检查文件夹和文件
	if (_access(configFPath.c_str(), 0))
	{
		CreateDirectoryA(configFPath.c_str(), NULL);
	}

	std::string file = configFPath + "config.json";
	if (_access(file.c_str(), 0))
	{
		FILE* f;
		if (fopen_s(&f, file.c_str(), "w+"))return;
		fclose(f);
	}

	//设置默认配置文件
	WritePrivateProfileStringA("main", "DefaultConfigFile", configFile, file.c_str());
}


//获取默认的配置文件

bool CConfig::GetDefaultConfig(char* buffer, int len)
{
	//检查文件夹和文件
	std::string file = configFPath + "config.json";
	if (_access(configFPath.c_str(), 0) || _access(file.c_str(), 0))
	{
		return false;
	}

	//获取默认配置文件
	return GetPrivateProfileStringA("main", "DefaultConfigFile", "", buffer, len, file.c_str());
}


//生成定义

void CConfig::BuiltDefine()
{
	//确保加载了配置
	if (configFile.size() == 0)return;

	//创建项目文件夹并清空
	if (_access(defineFPath.c_str(), 0))
	{
		CreateDirectoryA(defineFPath.c_str(), NULL);
	}

	std::string folder = defineFPath + configFile + '\\';
	if (_access(folder.c_str(), 0))
	{
		CreateDirectoryA(folder.c_str(), NULL);
	}
	else
	{
		CTool::DeleteAllFiles(folder.c_str());
	}

	//创建定义文件
	for (auto msgIt : msgDefs)
	{
		//创建文件
		std::string file = folder + msgIt.second.msgName + ".h";
		FILE* f;
		if (fopen_s(&f, file.c_str(), "w+"))continue;

		//写入定义
		CString data;
		data = "//自动生成的消息定义\n\n";
		fwrite(data, sizeof(char), data.GetLength(), f);
		data.Format("enum msg_%s\n{\n", msgIt.second.msgName.c_str());
		fwrite(data, sizeof(char), data.GetLength(), f);
		for (auto cmdIt : msgIt.second.cmdDefs)
		{
			data.Format("    %s = %u,\n", cmdIt.second.cmdName.c_str(), cmdIt.second.cmdId);
			fwrite(data, sizeof(char), data.GetLength(), f);
		}
		data = "};\n";
		fwrite(data, sizeof(char), data.GetLength(), f);
		fclose(f);
	}
	return;
}


//输出日志（输出消息）

void CConfig::Log(const MsgData* msgData)
{
	//输出逻辑：1. 消息输出和命令输出，确定是否整体输出
	//          2. 命令单独输出，独立判断，确定是否单独输出，


	//确保加载了配置
	if (configFile.size() == 0)return;

	//获取输出逻辑
	auto msgIt = msgDefs.find(msgData->msg);
	if (msgIt == msgDefs.end()) return;
	auto cmdIt = msgIt->second.cmdDefs.find(msgData->cmd);
	if (msgIt->second.isInput == false
		&& (cmdIt == msgIt->second.cmdDefs.end() || cmdIt->second.isAloneInput == false))return;
	if (cmdIt != msgIt->second.cmdDefs.end()
		&& cmdIt->second.isInput == false
		&& cmdIt->second.isAloneInput == false) return;

	//组织日志
	CString tmp, data, cmd, log;
	if (msgIt->second.handleMethod == "数据包")
	{
		for (int i = 0; i < msgData->dataLen; i++)
		{
			tmp.Format("%.2X ", (UCHAR)msgData->data[i]);
			data += tmp;
		}
	}
	else if (msgIt->second.handleMethod == "ASCII")
	{
		data.Format("%s", msgData->data);
	}
	else if (msgIt->second.handleMethod == "UTF-16")
	{
		CStringW str;
		str.Format(L"%s", (wchar_t*)msgData->data);
		data = str;
	}
	tm ptm;
	gmtime_s(&ptm, &msgData->time);
	if (cmdIt == msgIt->second.cmdDefs.end())
		cmd.Format("%d", msgData->cmd);
	else cmd.Format("%s(%d)", cmdIt->second.cmdName.c_str(), cmdIt->first);
	log.Format("[%.2d:%.2d-%.2d] [%s] [%s] [%d]  %s\n\n",
		ptm.tm_hour + 8, ptm.tm_min, ptm.tm_sec,
		msgIt->first.c_str(), cmd.GetBuffer(), msgData->dataLen, data.GetBuffer());

	//输出到文件
	if (msgIt->second.isInput == true
		&& (cmdIt == msgIt->second.cmdDefs.end() || cmdIt->second.isInput))
	{
		if (_access(logFPath.c_str(), 0))
		{
			CreateDirectoryA(logFPath.c_str(), NULL);
		}

		std::string mainFolder = logFPath + configFile + "\\";
		if (_access(mainFolder.c_str(), 0))
		{
			CreateDirectoryA(mainFolder.c_str(), NULL);
		}

		CString logFile;
		logFile.Format("%s%d-%d-%d.log", mainFolder.c_str(), 1900 + ptm.tm_year, 1 + ptm.tm_mon, ptm.tm_mday);
		FILE* f;
		if (fopen_s(&f, logFile, "a+") == 0)
		{
			fwrite(log, sizeof(char), log.GetLength(), f);
			fclose(f);
		}
	}

	//单独输出到文件
	if (cmdIt != msgIt->second.cmdDefs.end()
		&& cmdIt->second.isAloneInput)
	{
		if (_access(logFPath.c_str(), 0))
		{
			CreateDirectoryA(logFPath.c_str(), NULL);
		}

		std::string mainFolder = logFPath + configFile + "\\";
		if (_access(mainFolder.c_str(), 0))
		{
			CreateDirectoryA(mainFolder.c_str(), NULL);
		}

		std::string msgFolder = mainFolder + msgIt->first + "\\";
		if (_access(msgFolder.c_str(), 0))
		{
			CreateDirectoryA(msgFolder.c_str(), NULL);
		}

		CString logFile;
		logFile.Format("%s%s(%d)_%d-%d-%d.log", msgFolder.c_str(),
			cmdIt->second.cmdName.c_str(), cmdIt->first,
			1900 + ptm.tm_year, 1 + ptm.tm_mon, ptm.tm_mday);
		FILE* f;
		if (fopen_s(&f, logFile, "a+") == 0)
		{
			fwrite(log, sizeof(char), log.GetLength(), f);
			fclose(f);
		}
	}
}