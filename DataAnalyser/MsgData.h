#pragma once

class MsgData
{
public:
	char* msg;
	unsigned cmd;
	char* data;
	unsigned dataLen;
	time_t time; // longlong数据 1900到现在的间隔时间
	char* buffer;
	unsigned bufLen;


	MsgData();
	MsgData(const char* msg, unsigned cmd, const char* data, unsigned len,time_t time);
	~MsgData();
	void Init(const char* msg, unsigned cmd, const char* data, unsigned len,time_t time);
	bool Parse(const char* buf,unsigned len);
	char* ToBuffer();
};
