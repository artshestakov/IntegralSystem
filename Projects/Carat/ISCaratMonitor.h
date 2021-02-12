#pragma once
#ifndef _ISCARATMONITOR_H_INCLUDED
#define _ISCARATMONITOR_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISCaratMonitor
{
public:
	static ISCaratMonitor& Instance();

	void RegisterQueryTime(unsigned long long MSec);

	QString GetErrorString() const;
	void Start();
	void Shutdown();

private:
	void Process();
	quint64 GetMemory() const; //ѕолучить количество потребл€емой пам€ти
	unsigned long long GetTCPTimeAvg() const; //ѕолучить среднее врем€ выполнени€ TCP-запросов

private:
	ISCaratMonitor();
	~ISCaratMonitor();
	ISCaratMonitor(const ISCaratMonitor&) = delete;
	ISCaratMonitor(ISCaratMonitor&&) = delete;
	ISCaratMonitor& operator=(const ISCaratMonitor&) = delete;
	ISCaratMonitor& operator=(ISCaratMonitor&&) = delete;

private:
	QString ErrorString;
	bool IsRunning;
	bool IsFinished;
	unsigned int Timeout;
	ISCriticalSection CriticalSection;

private:
	unsigned long long TCPQueryTime;
	unsigned long TCPQueryCount;
};
//-----------------------------------------------------------------------------
#endif
