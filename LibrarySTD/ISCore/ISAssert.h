#pragma once
#ifndef _ISASSERT_H_INCLUDED
#define _ISASSERT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#define IS_ASSERT(CONDITION, FALSE_MESSAGE) ISASSERT::Assert(CONDITION, FALSE_MESSAGE, Q_FUNC_INFO, __LINE__, __FILE__);
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISASSERT
{
public:
	static void Assert(bool Boolean, const std::string &FalseMessage, const char *FunctionName, int Line, const char *FileName);
	static void Assert(int Integer, const std::string &FalseMessage, const char *FunctionName, int Line, const char *FileName);
	static void Assert(const std::string &String, const std::string &FalseMessage, const char *FunctionName, int Line, const char *FileName);
	static void Assert(size_t SizeT, const std::string &FalseMessage, const char *FunctionName, int Line, const char *FileName);

protected:
	static void ShowAssert(const std::string &FalseMessage, const char *FunctionName, int Line, const char *FileName);
};
//-----------------------------------------------------------------------------
#endif
