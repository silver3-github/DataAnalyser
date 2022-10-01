#include "pch.h"
#include "CDataAnaly.h"
#include "MsgData.h"
#include "CClient.h"
#include <time.h>


//����ʵ����ʼ��

CDataAnaly CDataAnaly::Instance;


//���������

CDataAnaly::CDataAnaly()
{
	tick = GetTickCount64();
}

CDataAnaly::~CDataAnaly()
{
}


//����ʵ��

bool CDataAnaly::Send(const char* msg, unsigned cmd, char*& data, unsigned& len)
{
	//ʹ�û�������ס����ֹ����߳�ͬʱִ��
	m_lock.lock();

	//��������(���ͼ������)
	CClient* client = &CClient::Instance;
	if (GetTickCount64() - tick > 1500)
		client->CloseSocket();

	//���ӷ��ʹ���
	if (client->isConnected()
		|| client->Connect("127.0.0.1", 13132)) //������
	{
		//������Ϣ
		time_t nowTime;
		time(&nowTime);
		MsgData msgData(msg, cmd, data, len,nowTime);
		msgData.ToBuffer();
		int ret = client->Send(msgData.buffer, msgData.bufLen);
		if (ret <= 0) //����ʧ��
		{
			tick = GetTickCount64();
			m_lock.unlock();
			return false;
		}

		//���մ�Ӧ��Ϣ
		ret = client->Recv();
		if (ret <= 0) //����ʧ��
		{
			tick = GetTickCount64();
			m_lock.unlock();
			return true;
		}

		//�����Ӧ��Ϣ
		msgData.Parse(
			client->packet.data.data(),
			client->packet.data.size()
		);
		switch (msgData.cmd)
		{
		case 0://ֻ��֪ͨһ�£�ɶҲ������
			break;
		case 1://�޸�����
			len = msgData.dataLen;
			data = new char[len] {};
			memcpy(data, msgData.data, len);
			break;
		}
		tick = GetTickCount64();
		m_lock.unlock();
		return true;
	}
	else //δ����
	{
		tick = GetTickCount64();
		m_lock.unlock();
		return false; 
	}
}
