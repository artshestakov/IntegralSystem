#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTcpWorker : public QObject
{
	Q_OBJECT

public:
	ISTcpWorker();
	virtual ~ISTcpWorker();

	bool GetRunning();
	void SetMessage(ISTcpMessage *TcpMessage);
	void Run();

private:
	void Finish();

private:
	void Auth(ISTcpMessage *TcpMessage);

private:
	bool IsRunning;
	ISTcpMessage *CurrentMessage;

	//Критическая секция для синхронизации
#ifdef WIN32
	CRITICAL_SECTION CriticalSection;
#else
	pthread_mutex_t CriticalSection;
#endif
};
//-----------------------------------------------------------------------------
#endif
