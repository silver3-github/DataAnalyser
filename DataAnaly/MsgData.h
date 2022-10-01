#pragma once

class MsgData
{
public:
	char* msg;
	unsigned cmd;
	char* data;
	unsigned dataLen;
	time_t time; // longlong���� 1900�����ڵļ��ʱ��
	char* buffer;
	unsigned bufLen;


	MsgData();
	MsgData(const char* msg, unsigned cmd, const char* data, unsigned len,time_t time);
	~MsgData();
	void Init(const char* msg, unsigned cmd, const char* data, unsigned len,time_t time);
	bool Parse(const char* buf,unsigned len);
	char* ToBuffer();
};
