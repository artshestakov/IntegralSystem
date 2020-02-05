#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class EXCrashDumper
{
public:
	static void Init();

private:
#ifdef WIN32
	static int ReportHook(int ReportType, char *Message, int *ReturnValue);
	static LONG ExceptionHandling(_EXCEPTION_POINTERS *ExceptionInfo);
	static void CreateReport(_EXCEPTION_POINTERS *ExceptionInfo);
#else
    static void OnSystemSignal(int Signum);
#endif
};
//-----------------------------------------------------------------------------
