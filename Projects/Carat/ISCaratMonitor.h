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

	QString GetErrorString() const;
	bool Start();
	void Shutdown();

private:
	void Process();
	quint64 GetMemory() const;

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
	ISCriticalSection CriticalSection;
};
//-----------------------------------------------------------------------------
#endif
