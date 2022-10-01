#pragma once
#include "CPacket.h"
#include <WinSock2.h>

class CClient
{
	//������
	CClient();
	~CClient();
public:
	static CClient Instance;

	//ʵ��
private:
	SOCKET m_sock;
	std::vector<char> buffer;

public:
	CPacket packet;

	bool isConnected();
	bool Connect(const char* ip,unsigned short port);
	void CloseSocket();
	int Send(char* buf,unsigned len);
	int Recv();
};

