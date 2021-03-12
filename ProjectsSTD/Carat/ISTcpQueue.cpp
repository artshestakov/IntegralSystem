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
	if (IsActive) //Если очередь активна - добавляем сообщение в неё
	{
		Queue.push(TcpMessage);
	}
	else //Иначе удаляем
	{
		delete TcpMessage;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpMessage* ISTcpQueue::GetMessage()
{
	ISTcpMessage *TcpMessage = nullptr;
	//Блокируем критическую секцию, забираем очередное сообщение и разблокируем секцию
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (IsActive) //Если очередь активна - возвращем очередное сообщение
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
