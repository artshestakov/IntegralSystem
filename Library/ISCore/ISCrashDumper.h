#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCrashDumper : public QObject
{
	Q_OBJECT

public:
	ISCrashDumper();
	virtual ~ISCrashDumper();

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
