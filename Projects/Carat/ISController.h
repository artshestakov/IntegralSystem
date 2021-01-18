#pragma once
#ifndef _ISCONTROLLER_H_INCLUDED
#define _ISCONTROLLER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAlgorithm.h"
#include "ISCaratApplication.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
BOOL WINAPI ControlHandler(DWORD SignalType)
{
	Q_UNUSED(SignalType);
	ISCaratApplication::quit();
	while (QCoreApplication::instance())
	{
		ISSleep(10);
	}
	return TRUE;
}
#else
void ControlHandler(int SignalType)
{
    Q_UNUSED(SignalType);
    ISCaratApplication::quit();
}
#endif
//-----------------------------------------------------------------------------
bool InstallController(QString &ErrorString)
{
	bool Result = true;
#ifdef WIN32
	Result = SetConsoleCtrlHandler(ControlHandler, TRUE) == TRUE;
	if (!Result)
	{
		ErrorString = ISAlgorithm::GetLastErrorString();
	}
#else
    Q_UNUSED(ErrorString);
    signal(SIGTERM, ControlHandler);
    signal(SIGHUP, ControlHandler);
#endif
	return Result;
}
//-----------------------------------------------------------------------------
#endif
