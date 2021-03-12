#include "ISTcpQueue.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpQueue::ISTcpQueue()
	: IsActive(true)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpQueue::~ISTcpQueue()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpQueue& ISTcpQueue::Instance()
{
	static ISTcpQueue TcpQueue;
	return TcpQueue;
}
//-----------------------------------------------------------------------------
void ISTcpQueue::Shutdown()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsActive = false;
	while (!Queue.empty())
	{
		ISTcpMessage *TcpMessage = Queue.front();
		Queue.pop();
		delete TcpMessage;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpQueue::AddMessage(ISTcpMessage *TcpMessage)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (IsActive) //���� ������� ������� - ��������� ��������� � ��
	{
		Queue.push(TcpMessage);
	}
	else //����� �������
	{
		delete TcpMessage;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpMessage* ISTcpQueue::GetMessage()
{
	ISTcpMessage *TcpMessage = nullptr;
	//��������� ����������� ������, �������� ��������� ��������� � ������������ ������
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (IsActive) //���� ������� ������� - ��������� ��������� ���������
	{
		if (!Queue.empty())
		{
			TcpMessage = Queue.front();
			Queue.pop();
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return TcpMessage;
}
//-----------------------------------------------------------------------------
