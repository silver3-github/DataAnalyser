#include "pch.h"
#include "CServer.h"
#include <ws2tcpip.h>


//����ʵ����ʼ��

CServer CServer::Instance;


//���������

CServer::CServer()
{
	//��ʼ���׽��ֻ���
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data)) {
		MessageBoxA(NULL, "�޷���ʼ���׽��ֻ����������������ã�", "�����ʼ������", MB_OK | MB_ICONERROR);
		exit(0);
	}

	//��ʼ������
	m_serv = INVALID_SOCKET;
	m_client = INVALID_SOCKET;
	recvHandler = nullptr;
	arg = nullptr;
	buffer.reserve(100000);
}

CServer::~CServer()
{
	//�ر��׽���
	CloseClientSocket();
	CloseServSocket();

	// �����׽��ֻ���
	WSACleanup();
}


//����ͨ��ʵ��

bool CServer::Invoke(const char* ip, unsigned port, RecvHandler handler, LPVOID arg)
{
	//�����ͻ���socket
	m_serv = socket(PF_INET, SOCK_STREAM, 0);
	if (m_serv == INVALID_SOCKET)return false;

	//�󶨵�ַ
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(port);
	if (bind(m_serv, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		CloseServSocket();
		return false;
	}

	//�����ͻ���
	if (listen(m_serv, 1) == SOCKET_ERROR) {
		CloseServSocket();
		return false;
	}

	//�����ڲ��߳�
	if ((HANDLE)_beginthread(ThreadFunc, 0, this) == INVALID_HANDLE_VALUE)
	{
		CloseServSocket();
		return false;
	}

	//���ý��մ������
	recvHandler = handler;
	this->arg = arg;

	return true;
}

void __cdecl CServer::ThreadFunc(void* arg)
{
	//��ȡthisָ��
	CServer* thiz = (CServer*)arg;

	//���ӡ����ա���������
	if (!thiz->Accept())return _endthread();
	while (true)
	{
		if (thiz->isConnected() && thiz->Recv() > 0)//���յ�����
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
		else //�Ͽ�����
		{
			if (!thiz->Accept()) return _endthread();
		}
	}
}

bool CServer::Accept()
{
	//�ر�֮ǰ�Ŀͻ�������
	CloseClientSocket();

	//���ܿͻ�������
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
		if (ret > 0) //���ճɹ�
		{
			len += ret;
			if (packet.Parse(buffer.data(), len))
				return len;
		}
		else //socket �ر�/���ִ���
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