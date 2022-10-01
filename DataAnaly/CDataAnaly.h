#pragma once
#include <mutex>

class CDataAnaly
{
	//������
	CDataAnaly();
	~CDataAnaly();
public:
	static CDataAnaly Instance;

	//���ܣ�
protected:
	std::mutex m_lock;
	long long tick;

public:
	/// <summary>
	/// ������Ϣ�����ݷ��������ڲ���ʹ���߳��������߳��·���ʳ�ã�
	/// </summary>
	/// <param name="msg">��Ϣ�� ���������ݷ������ж��壬�����޷����յ�</param>
	/// <param name="cmd">����� �����ڸ���Ϣ������޶���Ҳ�����</param>
	/// <param name="data">Ҫ���͹�ȥ�����ݣ�ʵʱ�༭�£����ر༭������ݣ�</param>
	/// <param name="len">�������ݵĳ��ȣ�ʵʱ�༭�£����ر༭������ݳ��ȣ�</param>
	/// <returns>true ���ͳɹ� / false ����ʧ�ܣ����ݷ�����δ����</returns>
	bool Send(const char* msg, unsigned cmd, char*& data, unsigned& len);
};

