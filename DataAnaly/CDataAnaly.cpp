#include "pch.h"
#include "CDataAnaly.h"
#include "MsgData.h"
#include "CClient.h"
#include <time.h>


//单例实例初始化

CDataAnaly CDataAnaly::Instance;


//构造和析构

CDataAnaly::CDataAnaly()
{
	tick = GetTickCount64();
}

CDataAnaly::~CDataAnaly()
{
}


//功能实现

bool CDataAnaly::Send(const char* msg, unsigned cmd, char*& data, unsigned& len)
{
	//使用互斥锁锁住，防止多个线程同时执行
	m_lock.lock();

	//重连发送(发送间隔过长)
	CClient* client = &CClient::Instance;
	if (GetTickCount64() - tick > 1500)
		client->CloseSocket();

	//连接发送处理
	if (client->isConnected()
		|| client->Connect("127.0.0.1", 13132)) //已连接
	{
		//发送消息
		time_t nowTime;
		time(&nowTime);
		MsgData msgData(msg, cmd, data, len,nowTime);
		msgData.ToBuffer();
		int ret = client->Send(msgData.buffer, msgData.bufLen);
		if (ret <= 0) //发送失败
		{
			tick = GetTickCount64();
			m_lock.unlock();
			return false;
		}

		//接收答应消息
		ret = client->Recv();
		if (ret <= 0) //接收失败
		{
			tick = GetTickCount64();
			m_lock.unlock();
			return true;
		}

		//处理答应消息
		msgData.Parse(
			client->packet.data.data(),
			client->packet.data.size()
		);
		switch (msgData.cmd)
		{
		case 0://只是通知一下（啥也不做）
			break;
		case 1://修改数据
			len = msgData.dataLen;
			data = new char[len] {};
			memcpy(data, msgData.data, len);
			break;
		}
		tick = GetTickCount64();
		m_lock.unlock();
		return true;
	}
	else //未连接
	{
		tick = GetTickCount64();
		m_lock.unlock();
		return false; 
	}
}
