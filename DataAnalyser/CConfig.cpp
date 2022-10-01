#include "pch.h"
#include "CConfig.h"
#include "CTool.h"
#include <io.h>


//����ʵ����ʼ��

CConfig CConfig::Instance;


//���������

CConfig::CConfig()
{
	//��ȡ�����ļ���·��
	char buffer[MAX_PATH]{};
	int i = GetModuleFileNameA(NULL, buffer, MAX_PATH);//��ȡ�ļ���·��
	for (; 0 <= i; i--)if (buffer[i] == '\\')break;
	buffer[i + 1] = 0;
	folderPath = buffer;
	configFPath = folderPath + "ConfigFile\\";
	defineFPath = folderPath + "DefineFile\\";
	logFPath = folderPath + "Log\\";

	//��������ļ���
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

	//����Ĭ�������ļ�
	if (GetDefaultConfig(buffer, MAX_PATH))
	{
		LoadFromFile(buffer);
	}
}

CConfig::~CConfig()
{
}


//����ʵ��


//�Ƿ���ڶ���

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


//��Ӷ���

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


//���ö���

bool CConfig::SetMsgDef(const char* msg, const char* msgName, const char* handleMethod, bool isInput)
{
	if (!IsExistMsgDef(msg))return false;//Ҫ�޸ĵ���Ϣ���岻����
	if (strcmp(msg, msgName) && msgDefs.count(msgName))return false;//�޸ĺ����Ϣ������ظ�����

	msgDefs.erase(msg);
	MsgDefine msgDef{ msgName,handleMethod,isInput };
	msgDefs.insert(std::pair<std::string, MsgDefine>(msgName, msgDef));
	return true;
}

bool CConfig::SetCmdDef(const char* msg, unsigned cmd, const char* cmdName, unsigned cmdId, bool isShow, bool isEdit, bool isInput, bool isAloneInput, COLORREF textColor, COLORREF bgColor)
{
	if (!IsExistCmdDef(msg, cmd))return false;//Ҫ�޸ĵ�����岻����
	std::map<std::string, MsgDefine>::iterator it;
	it = msgDefs.find(msg);
	if (cmd != cmdId && it->second.cmdDefs.count(cmdId))return false;//�޸ĺ���������ظ�����

	it->second.cmdDefs.erase(cmd);
	CmdDefine cmdDef{ cmdName, cmdId, isShow, isEdit, isInput, isAloneInput,textColor,bgColor };
	it->second.cmdDefs.insert(std::pair<DWORD, CmdDefine>(cmdId, cmdDef));
	return true;
}


//ɾ������

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


//��д�����ļ�����

bool CConfig::SaveToFile(const char* configFile)
{
	//�����ļ���
	if (_access(configFPath.c_str(), 0))
	{
		CreateDirectoryA(configFPath.c_str(), NULL);
	}

	//�������ļ�
	std::string newFile = configFPath + configFile + ".ini";
	FILE* f;
	if (fopen_s(&f, newFile.c_str(), "w+"))return false;
	fclose(f);

	//д����������
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

	//ɾ�����ļ�
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
	//�ж������ļ��Ƿ����
	std::string file = configFPath + configFile + ".ini";
	if (_access(file.c_str(), 0) == -1) return false;

	//�������
	msgDefs.clear();

	//��ȡ��������
	this->configFile = configFile;
	CString appName, value1, value2;
	int msgCount = GetPrivateProfileIntA("main", "count", 0, file.c_str());
	for (int i = 0; i < msgCount; i++)
	{
		appName.Format("MSGDEF%d", i);
		char buffer[MAX_PATH]{};
		GetPrivateProfileStringA(appName, "msgName", appName, buffer, MAX_PATH, file.c_str());
		value1 = buffer;
		GetPrivateProfileStringA(appName, "handleMethod", "���ݰ�", buffer, MAX_PATH, file.c_str());
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

	//����ΪĬ�������ļ�
	SetDefaultConfig(configFile);
	return true;
}


//����Ĭ�ϵ������ļ�

void CConfig::SetDefaultConfig(const char* configFile)
{
	//����ļ��к��ļ�
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

	//����Ĭ�������ļ�
	WritePrivateProfileStringA("main", "DefaultConfigFile", configFile, file.c_str());
}


//��ȡĬ�ϵ������ļ�

bool CConfig::GetDefaultConfig(char* buffer, int len)
{
	//����ļ��к��ļ�
	std::string file = configFPath + "config.json";
	if (_access(configFPath.c_str(), 0) || _access(file.c_str(), 0))
	{
		return false;
	}

	//��ȡĬ�������ļ�
	return GetPrivateProfileStringA("main", "DefaultConfigFile", "", buffer, len, file.c_str());
}


//���ɶ���

void CConfig::BuiltDefine()
{
	//ȷ������������
	if (configFile.size() == 0)return;

	//������Ŀ�ļ��в����
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

	//���������ļ�
	for (auto msgIt : msgDefs)
	{
		//�����ļ�
		std::string file = folder + msgIt.second.msgName + ".h";
		FILE* f;
		if (fopen_s(&f, file.c_str(), "w+"))continue;

		//д�붨��
		CString data;
		data = "//�Զ����ɵ���Ϣ����\n\n";
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


//�����־�������Ϣ��

void CConfig::Log(const MsgData* msgData)
{
	//����߼���1. ��Ϣ��������������ȷ���Ƿ��������
	//          2. ���������������жϣ�ȷ���Ƿ񵥶������


	//ȷ������������
	if (configFile.size() == 0)return;

	//��ȡ����߼�
	auto msgIt = msgDefs.find(msgData->msg);
	if (msgIt == msgDefs.end()) return;
	auto cmdIt = msgIt->second.cmdDefs.find(msgData->cmd);
	if (msgIt->second.isInput == false
		&& (cmdIt == msgIt->second.cmdDefs.end() || cmdIt->second.isAloneInput == false))return;
	if (cmdIt != msgIt->second.cmdDefs.end()
		&& cmdIt->second.isInput == false
		&& cmdIt->second.isAloneInput == false) return;

	//��֯��־
	CString tmp, data, cmd, log;
	if (msgIt->second.handleMethod == "���ݰ�")
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

	//������ļ�
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

	//����������ļ�
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