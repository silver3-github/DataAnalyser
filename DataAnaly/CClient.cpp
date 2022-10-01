#include "pch.h"
#include "CClient.h"
#include <ws2tcpip.h>


//单例实例初始化

CClient CClient::Instance;


//构造和析构

CClient::CClient()
{
	//初始化套接字环境
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data)) {
		MessageBoxA(NULL, "无法初始化套接字环境，请检查网络设置！", "网络初始化错误！", MB_OK | MB_ICONERROR);
		exit(0);
	}

	//初始化变量
	m_sock = INVALID_SOCKET;
	buffer.reserve(100000);
}

CClient::~CClient()
{
	//关闭套接字
	CloseSocket();

	// 清理套接字环境
	WSACleanup();
}


//网络通信实现

bool CClient::isConnected()
{
	return m_sock != INVALID_SOCKET;
}

bool CClient::Connect(const char* ip, unsigned short port)
{
	//关闭之前的连接
	CloseSocket();

	//创建客户端socket
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET)return false;

	//初始化地址
	sockaddr_in serv_adr;
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &serv_adr.sin_addr.s_addr);;//服务端IP地址
	serv_adr.sin_port = htons(port);//服务端端口

	//连接服务器
	if (connect(m_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR) 
	{
		CloseSocket();
		return false;
	}
	return true;
}

int CClient::Send(char* buf, unsigned len)
{
	packet.Pack(buf, len);
	int ret = send(m_sock, packet.packed.data(), packet.packed.size(), 0);
	if (ret <= 0)CloseSocket();
	return ret;
}

int CClient::Recv()
{
	while (true)
	{
		int len = 0;
		int ret = recv(m_sock, buffer.data() + len, buffer.capacity() - len, 0);
		if (ret > 0) //接收成功
		{
			len += ret;
			if (packet.Parse(buffer.data(), len))
				return len;
		}
		else //socket 关闭/出现错误
		{
			CloseSocket();
			packet.data.clear();
			return ret;
		}
	}
}

void CClient::CloseSocket()
{
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}
