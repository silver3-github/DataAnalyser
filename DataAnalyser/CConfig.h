#pragma once
#include "MsgData.h"
#include <string>
#include <map>


//命令定义数据 结构体
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

//消息定义数据 结构体
struct MsgDefine
{
	std::string msgName;
	std::string handleMethod;
	BOOL isInput;
	std::map<DWORD, CmdDefine> cmdDefs;
};


//配置类
class CConfig
{
	CConfig();
	~CConfig();
public:
	static CConfig Instance;

	//实现
public:
	std::string folderPath;
	std::string configFPath;
	std::string defineFPath;
	std::string logFPath;
	std::string configFile; //配置文件名
	std::map<std::string, MsgDefine> msgDefs;

	//操作数据
	bool IsExistMsgDef(const char* msgName);
	bool IsExistCmdDef(const char* msg, unsigned cmdId);
	bool AddMsgDef(const char* msgName, const char* handleMethod, bool isInput);
	bool AddCmdDef(const char* msg, const char* cmdName, unsigned cmdId, bool isShow, bool isEdit, bool isInput, bool isAloneInput, COLORREF textColor, COLORREF bgColor);
	bool SetMsgDef(const char* msg, const char* msgName, const char* handleMethod, bool isInput);
	bool SetCmdDef(const char* msg, unsigned cmd, const char* cmdName, unsigned cmdId, bool isShow, bool isEdit, bool isInput, bool isAloneInput, COLORREF textColor, COLORREF bgColor);
	bool DeleteMsgDef(const char* msgName);
	bool DeleteCmdDef(const char* msg, unsigned cmdId);

	//读写文件
	bool SaveToFile(const char* configFile);
	bool LoadFromFile(const char* configFile);
	void SetDefaultConfig(const char* configFile);
	bool GetDefaultConfig(char* buffer,int len);

	//生成定义
	void BuiltDefine();

	//输出日志（输出消息）
	void Log(const MsgData* msgData);
};

