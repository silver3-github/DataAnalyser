#include "pch.h"
#include "MsgData.h"


//���������

MsgData::MsgData()
{
	msg = nullptr;
	cmd = -1;
	data = nullptr;
	dataLen = 0;
	time = 0;
	buffer = nullptr;
	bufLen = 0;
}

MsgData::MsgData(const char* msg, unsigned cmd, const char* data, unsigned len, time_t time)
{
	this->msg = nullptr;
	this->data = nullptr;
	this->buffer = nullptr;
	Init(msg, cmd, data, len, time);
}

MsgData::~MsgData()
{
	if (msg)delete[] msg;
	if (data)delete[] data;
	if (buffer)delete[] buffer;
}

void MsgData::Init(const char* msg, unsigned cmd, const char* data, unsigned len, time_t time)
{
	//��ն���
	if (this->msg)delete[] this->msg;
	if (this->data)delete[] this->data;
	if (this->buffer)delete[] this->buffer;

	//��ʼ������
	int msgLen = strlen(msg) + 1;
	this->msg = new char[msgLen] {};
	memcpy(this->msg, msg, msgLen);
	this->cmd = cmd;
	this->data = new char[len] {};
	memcpy(this->data, data, len);
	this->dataLen = len;
	this->time = time;
	buffer = nullptr;
	bufLen = 0;
}


//������ת��

bool MsgData::Parse(const char* buf, unsigned len)
{
	//��������
	if (msg)delete[] msg;
	msg = nullptr;
	if (data)delete[] data;
	data = nullptr;
	dataLen = 0;

	//��������
	int msgLen = strlen(buf) + 1;
	if (msgLen + sizeof(time) + sizeof(cmd) > len)return false;
	msg = new char[msgLen] {};
	memcpy(msg, buf, msgLen);
	memcpy(&time, buf + msgLen, sizeof(time));
	memcpy(&cmd, buf + msgLen + sizeof(time), sizeof(cmd));
	dataLen = len - msgLen - sizeof(time) - sizeof(cmd);
	if (dataLen > 0) {
		data = new char[dataLen] {};
		memcpy(data, buf + msgLen + sizeof(time) + sizeof(cmd), dataLen);
	}
	return true;
}

char* MsgData::ToBuffer()
{
	//��������
	if (buffer)delete[] buffer;
	buffer = nullptr;
	bufLen = 0;

	//���仺����
	int msgLen = strlen(msg) + 1;
	bufLen = msgLen + sizeof(time) + sizeof(cmd) + dataLen;
	buffer = new char[bufLen] {};

	//��װ����
	memcpy(buffer, msg, msgLen);
	memcpy(buffer + msgLen, &time, sizeof(time));
	memcpy(buffer + msgLen + sizeof(time), &cmd, sizeof(cmd));
	memcpy(buffer + msgLen + sizeof(time) + sizeof(cmd), data, dataLen);
	return buffer;
}
