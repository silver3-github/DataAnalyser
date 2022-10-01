#include "pch.h"
#include "CPacket.h"

bool CPacket::Parse(char* buf, unsigned len)
{
	//清理数据
	data.clear();

	//获取 EE EE 包尾
	int tailCount = -1;
	for (int i = 0; i < len - 1; i++)
	{
		WORD* tail = (WORD*)(buf + i);
		if (*tail == (WORD)0xEEEE)tailCount = i;
	}
	if (tailCount == -1)return false;

	//分配缓冲区
	data.reserve(tailCount);

	//解析数据包
	for (int i = 0; i < tailCount; i++)
	{
		data.push_back(buf[i]);
		if (buf[i] == (char)0xEE)i++;
	}
	return true;
}

void CPacket::Pack(char* buf, unsigned len)
{
	//计算 EE 的数量
	int ecount = 0;
	for (int i = 0; i < len; i++)
		if (buf[i] == (char)0xEE)ecount++;

	//分配缓冲区
	packed.clear();
	packed.reserve(len + ecount + 2);

	//打包数据
	for (int i = 0; i < len; i++)
	{
		packed.push_back(buf[i]);
		if (buf[i] == (char)0xEE)packed.push_back(0);
	}
	packed.push_back((char)0xEE);
	packed.push_back((char)0xEE);
}
