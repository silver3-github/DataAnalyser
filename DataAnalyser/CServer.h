#pragma once
#include <vector>
#include "CPacket.h"

class CServer
{
	//单例：
	CServer();
	~CServer();
public:
	static CServer Instance;

	//实现
private:
	typedef void(__stdcall* RecvHandler)(char* buf, unsigned len, void* arg);
	SOCKET m_serv;//已绑定、监听，但未连接的 服务器socket（主要用于接受客户端的连接）
	SOCKET m_client;//已连接客户端的 服务器socket
	std::vector<char> buffer;
	CPacket packet;
	RecvHandler recvHandler;
	LPVOID arg;

	bool Accept();
	int  Recv();
	bool isConnected();
	void CloseServSocket();
	void CloseClientSocket();
	static void __cdecl ThreadFunc(void* arg);
public:

	bool Invoke(const char* ip, unsigned port, RecvHandler handler, LPVOID arg);
	int  Send(char* buf, unsigned len);
};

