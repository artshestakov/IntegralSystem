#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCrashDumper
{
public:
	ISCrashDumper();
	~ISCrashDumper();

	static void Startup();

protected:
#ifdef WIN32
	static int ReportHook(int ReportType, char *Message, int *ReturnValue);
	static LONG ExceptionHandling(_EXCEPTION_POINTERS *ExceptionInfo);
	static void CreateReport(const QString &FileName, char *Text, _EXCEPTION_POINTERS *ExceptionInfo);
#else
    static void OnSystemSignal(int Signum);
#endif

    static QString GetLogPath(const QString &FileName);
};
//-----------------------------------------------------------------------------
