#pragma once
#include <mutex>

class CDataAnaly
{
	//单例：
	CDataAnaly();
	~CDataAnaly();
public:
	static CDataAnaly Instance;

	//功能：
protected:
	std::mutex m_lock;
	long long tick;

public:
	/// <summary>
	/// 发送消息到数据分析器（内部已使用线程锁，多线程下放心食用）
	/// </summary>
	/// <param name="msg">消息名 必须在数据分析器中定义，否则将无法接收到</param>
	/// <param name="cmd">命令号 所属于该消息的命令，无定义也会接收</param>
	/// <param name="data">要发送过去的数据（实时编辑下，返回编辑后的数据）</param>
	/// <param name="len">发送数据的长度（实时编辑下，返回编辑后的数据长度）</param>
	/// <returns>true 发送成功 / false 发送失败，数据分析器未启动</returns>
	bool Send(const char* msg, unsigned cmd, char*& data, unsigned& len);
};

