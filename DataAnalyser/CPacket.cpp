#include "pch.h"
#include "CPacket.h"

bool CPacket::Parse(char* buf, unsigned len)
{
	//��������
	data.clear();

	//��ȡ EE EE ��β
	int tailCount = -1;
	for (int i = 0; i < len - 1; i++)
	{
		WORD* tail = (WORD*)(buf + i);
		if (*tail == (WORD)0xEEEE)tailCount = i;
	}
	if (tailCount == -1)return false;

	//���仺����
	data.reserve(tailCount);

	//�������ݰ�
	for (int i = 0; i < tailCount; i++)
	{
		data.push_back(buf[i]);
		if (buf[i] == (char)0xEE)i++;
	}
	return true;
}

void CPacket::Pack(char* buf, unsigned len)
{
	//���� EE ������
	int ecount = 0;
	for (int i = 0; i < len; i++)
		if (buf[i] == (char)0xEE)ecount++;

	//���仺����
	packed.clear();
	packed.reserve(len + ecount + 2);

	//�������
	for (int i = 0; i < len; i++)
	{
		packed.push_back(buf[i]);
		if (buf[i] == (char)0xEE)packed.push_back(0);
	}
	packed.push_back((char)0xEE);
	packed.push_back((char)0xEE);
}
