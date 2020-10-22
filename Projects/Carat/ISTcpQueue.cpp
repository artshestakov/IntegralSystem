#include "ISTcpQueue.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpQueue::ISTcpQueue()
	: MessageID(0)
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
void ISTcpQueue::AddMessage(ISTcpMessage *TcpMessage)
{
	//Блокируем критическую секцию, добавляем сообщение в очередь и разблокируем секцию
	CRITICAL_SECTION_LOCK(&CriticalSection);
	TcpMessage->MessageID = ++MessageID;
	Queue.push(TcpMessage);
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpMessage* ISTcpQueue::GetMessage()
{
	ISTcpMessage *TcpMessage = nullptr;
	//Блокируем критическую секцию, забираем очередное сообщение и разблокируем секцию
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (!Queue.empty())
	{
		TcpMessage = Queue.front();
		Queue.pop();
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return TcpMessage;
}
//-----------------------------------------------------------------------------
