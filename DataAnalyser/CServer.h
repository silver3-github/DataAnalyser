#pragma once
#include <vector>
#include "CPacket.h"

class CServer
{
	//������
	CServer();
	~CServer();
public:
	static CServer Instance;

	//ʵ��
private:
	typedef void(__stdcall* RecvHandler)(char* buf, unsigned len, void* arg);
	SOCKET m_serv;//�Ѱ󶨡���������δ���ӵ� ������socket����Ҫ���ڽ��ܿͻ��˵����ӣ�
	SOCKET m_client;//�����ӿͻ��˵� ������socket
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

