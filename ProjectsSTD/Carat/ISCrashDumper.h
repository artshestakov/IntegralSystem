#pragma once
#ifndef _ISCRASHDUMPER_H_INCLUDED
#define _ISCRASHDUMPER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCrashDumper
{
public:
    static bool Install();

private:
#ifdef WIN32
    static LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs);
#else
#endif
};
//-----------------------------------------------------------------------------
#endif
