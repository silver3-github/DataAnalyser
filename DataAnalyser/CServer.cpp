#include "pch.h"
#include "CServer.h"
#include <ws2tcpip.h>


//单例实例初始化

CServer CServer::Instance;


//构造和析构

CServer::CServer()
{
	//初始化套接字环境
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data)) {
		MessageBoxA(NULL, "无法初始化套接字环境，请检查网络设置！", "网络初始化错误！", MB_OK | MB_ICONERROR);
		exit(0);
	}

	//初始化变量
	m_serv = INVALID_SOCKET;
	m_client = INVALID_SOCKET;
	recvHandler = nullptr;
	arg = nullptr;
	buffer.reserve(100000);
}

CServer::~CServer()
{
	//关闭套接字
	CloseClientSocket();
	CloseServSocket();

	// 清理套接字环境
	WSACleanup();
}


//网络通信实现

bool CServer::Invoke(const char* ip, unsigned port, RecvHandler handler, LPVOID arg)
{
	//创建客户端socket
	m_serv = socket(PF_INET, SOCK_STREAM, 0);
	if (m_serv == INVALID_SOCKET)return false;

	//绑定地址
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(port);
	if (bind(m_serv, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		CloseServSocket();
		return false;
	}

	//监听客户端
	if (listen(m_serv, 1) == SOCKET_ERROR) {
		CloseServSocket();
		return false;
	}

	//启动内部线程
	if ((HANDLE)_beginthread(ThreadFunc, 0, this) == INVALID_HANDLE_VALUE)
	{
		CloseServSocket();
		return false;
	}

	//设置接收处理程序
	recvHandler = handler;
	this->arg = arg;

	return true;
}

void __cdecl CServer::ThreadFunc(void* arg)
{
	//获取this指针
	CServer* thiz = (CServer*)arg;

	//连接、接收、处理、发送
	if (!thiz->Accept())return _endthread();
	while (true)
	{
		if (thiz->isConnected() && thiz->Recv() > 0)//接收到数据
		{
			if (thiz->recvHandler)
			{
				thiz->recvHandler(
					thiz->packet.data.data(),
					thiz->packet.data.size(),
					thiz->arg
				);
			}
		}
		else //断开连接
		{
			if (!thiz->Accept()) return _endthread();
		}
	}
}

bool CServer::Accept()
{
	//关闭之前的客户端连接
	CloseClientSocket();

	//接受客户端连接
	sockaddr_in client_addr;
	int addrlen = sizeof(client_addr);
	m_client = accept(m_serv, (sockaddr*)&client_addr, &addrlen);
	if (m_client == INVALID_SOCKET)return false;
	return true;
}

int CServer::Send(char* buf, unsigned len)
{
	packet.Pack(buf, len);
	int ret = send(m_client, packet.packed.data(), packet.packed.size(), 0);
	if (ret <= 0)CloseClientSocket();
	return ret;
}

int CServer::Recv()
{
	while (true)
	{
		int len = 0;
		int ret = recv(m_client, buffer.data() + len, buffer.capacity() - len, 0);
		if (ret > 0) //接收成功
		{
			len += ret;
			if (packet.Parse(buffer.data(), len))
				return len;
		}
		else //socket 关闭/出现错误
		{
			CloseClientSocket();
			packet.data.clear();
			return ret;
		}
	}
}

bool CServer::isConnected()
{
	return m_client != INVALID_SOCKET;
}

void CServer::CloseServSocket()
{
	if (m_serv != INVALID_SOCKET)
	{
		closesocket(m_serv);
		m_serv = INVALID_SOCKET;
	}
}

void CServer::CloseClientSocket()
{
	if (m_client != INVALID_SOCKET)
	{
		closesocket(m_client);
		m_client = INVALID_SOCKET;
	}
}