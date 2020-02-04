#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCrashDumper
{
public:
	static void Startup();

private:
#ifdef WIN32
	static int ReportHook(int ReportType, char *Message, int *ReturnValue);
	static LONG ExceptionHandling(_EXCEPTION_POINTERS *ExceptionInfo);
	static void CreateReport(char *Text, _EXCEPTION_POINTERS *ExceptionInfo);
#else
    static void OnSystemSignal(int Signum);
#endif

    static QString GetLogPath(const QString &FileName);
};
//-----------------------------------------------------------------------------
