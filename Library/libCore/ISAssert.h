#pragma once
#ifndef _ISASSERT_H_INCLUDED
#define _ISASSERT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//-----------------------------------------------------------------------------
#define IS_ASSERT(CONDITION, FALSE_MESSAGE) ISASSERT::Assert(CONDITION, FALSE_MESSAGE, Q_FUNC_INFO, __LINE__, __FILE__);
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISASSERT
{
public:
	static void Assert(QObject *Object, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName);
	static void Assert(bool Boolean, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName);
	static void Assert(int Integer, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName);
	static void Assert(QString String, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName);

protected:
	static void ShowAssert(const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName);
};
//-----------------------------------------------------------------------------
#endif
