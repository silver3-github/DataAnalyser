#pragma once
#include "MsgData.h"
#include <string>
#include <map>


//��������� �ṹ��
struct CmdDefine
{
	std::string cmdName;
	DWORD cmdId;
	BOOL isShow;
	BOOL isEdit;
	BOOL isInput;
	BOOL isAloneInput;
	COLORREF textColor;
	COLORREF bgColor;
};

//��Ϣ�������� �ṹ��
struct MsgDefine
{
	std::string msgName;
	std::string handleMethod;
	BOOL isInput;
	std::map<DWORD, CmdDefine> cmdDefs;
};


//������
class CConfig
{
	CConfig();
	~CConfig();
public:
	static CConfig Instance;

	//ʵ��
public:
	std::string folderPath;
	std::string configFPath;
	std::string defineFPath;
	std::string logFPath;
	std::string configFile; //�����ļ���
	std::map<std::string, MsgDefine> msgDefs;

	//��������
	bool IsExistMsgDef(const char* msgName);
	bool IsExistCmdDef(const char* msg, unsigned cmdId);
	bool AddMsgDef(const char* msgName, const char* handleMethod, bool isInput);
	bool AddCmdDef(const char* msg, const char* cmdName, unsigned cmdId, bool isShow, bool isEdit, bool isInput, bool isAloneInput, COLORREF textColor, COLORREF bgColor);
	bool SetMsgDef(const char* msg, const char* msgName, const char* handleMethod, bool isInput);
	bool SetCmdDef(const char* msg, unsigned cmd, const char* cmdName, unsigned cmdId, bool isShow, bool isEdit, bool isInput, bool isAloneInput, COLORREF textColor, COLORREF bgColor);
	bool DeleteMsgDef(const char* msgName);
	bool DeleteCmdDef(const char* msg, unsigned cmdId);

	//��д�ļ�
	bool SaveToFile(const char* configFile);
	bool LoadFromFile(const char* configFile);
	void SetDefaultConfig(const char* configFile);
	bool GetDefaultConfig(char* buffer,int len);

	//���ɶ���
	void BuiltDefine();

	//�����־�������Ϣ��
	void Log(const MsgData* msgData);
};

