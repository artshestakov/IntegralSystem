#pragma once
#ifndef _ISASSERT_H_INCLUDED
#define _ISASSERT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "StdAfx.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#define IS_ASSERT(CONDITION, FALSE_MESSAGE) ISASSERT::Assert(CONDITION, FALSE_MESSAGE, __FUNCTION__, __LINE__, __FILE__)
#else
#define IS_ASSERT(CONDITION, FALSE_MESSAGE) ISASSERT::Assert(CONDITION, FALSE_MESSAGE, __PRETTY_FUNCTION__, __LINE__, __FILE__)
#endif
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISASSERT
{
public:
	static void Assert(bool Boolean, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName);
	static void Assert(int Integer, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName);
	static void Assert(const std::string &String, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName);
	static void Assert(size_t SizeT, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName);

protected:
	static void ShowAssert(const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName);
};
//-----------------------------------------------------------------------------
#endif
