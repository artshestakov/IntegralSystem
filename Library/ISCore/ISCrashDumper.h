#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCrashDumper
{
public:
	static void Init();
	static void SetAssertMessage(const char *assert_message);

private:

#ifdef WIN32
	static int ReportHook(int ReportType, char *Message, int *ReturnValue);
	static LONG ExceptionHandling(_EXCEPTION_POINTERS *ExceptionInfo);
	static void CreateReport(_EXCEPTION_POINTERS *ExceptionInfo, const std::string &Message = std::string());
#else
    static void OnSystemSignal(int Signum);
#endif
};
//-----------------------------------------------------------------------------
