#pragma once
#ifndef _ISCONTROLLER_H_INCLUDED
#define _ISCONTROLLER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISAlgorithm.h"
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
#endif
	return Result;
}
//-----------------------------------------------------------------------------
#endif
