#include "pch.h"
#include "CClient.h"
#include <ws2tcpip.h>


//����ʵ����ʼ��

CClient CClient::Instance;


//���������

CClient::CClient()
{
	//��ʼ���׽��ֻ���
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data)) {
		MessageBoxA(NULL, "�޷���ʼ���׽��ֻ����������������ã�", "�����ʼ������", MB_OK | MB_ICONERROR);
		exit(0);
	}

	//��ʼ������
	m_sock = INVALID_SOCKET;
	buffer.reserve(100000);
}

CClient::~CClient()
{
	//�ر��׽���
	CloseSocket();

	// �����׽��ֻ���
	WSACleanup();
}


//����ͨ��ʵ��

bool CClient::isConnected()
{
	return m_sock != INVALID_SOCKET;
}

bool CClient::Connect(const char* ip, unsigned short port)
{
	//�ر�֮ǰ������
	CloseSocket();

	//�����ͻ���socket
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET)return false;

	//��ʼ����ַ
	sockaddr_in serv_adr;
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &serv_adr.sin_addr.s_addr);;//�����IP��ַ
	serv_adr.sin_port = htons(port);//����˶˿�

	//���ӷ�����
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
		if (ret > 0) //���ճɹ�
		{
			len += ret;
			if (packet.Parse(buffer.data(), len))
				return len;
		}
		else //socket �ر�/���ִ���
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
