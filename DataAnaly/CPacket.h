#pragma once
#include <vector>

class CPacket
{
public:
	std::vector<char> data;
	std::vector<char> packed;

	bool Parse(char* buf, unsigned len);
	void Pack(char* buf, unsigned len);
};

