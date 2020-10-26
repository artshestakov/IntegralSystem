#pragma once
#ifndef _ISTCPQUEUE_H_INCLUDED
#define _ISTCPQUEUE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpQueue
{
public:
	static ISTcpQueue& Instance();

	void AddMessage(ISTcpMessage *TcpMessage); //Добавить сообщение в очередь
	ISTcpMessage* GetMessage(); //Получить очередное сообщение

private:
	ISTcpQueue();
	~ISTcpQueue();
	ISTcpQueue(ISTcpQueue const &) {};
	ISTcpQueue& operator=(ISTcpQueue const&) { return *this; };

private:
	std::queue<ISTcpMessage *> Queue;
	qint64 MessageID;

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
