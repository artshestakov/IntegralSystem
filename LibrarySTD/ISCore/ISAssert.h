#pragma once
#ifndef _ISASSERT_H_INCLUDED
#define _ISASSERT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#define IS_ASSERT(CONDITION, FALSE_MESSAGE) ISASSERT::Assert(CONDITION, FALSE_MESSAGE, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
class ISASSERT
{
public:
    static void Assert(bool Boolean, const std::string &FalseMessage, const char *FileName, int Line);
    static void Assert(int Integer, const std::string &FalseMessage, const char *FileName, int Line);
    static void Assert(const std::string &String, const std::string &FalseMessage, const char *FileName, int Line);
    static void Assert(size_t SizeT, const std::string &FalseMessage, const char *FileName, int Line);

protected:
    static void ShowAssert(const std::string &FalseMessage, const char *FileName, int Line);
};
//-----------------------------------------------------------------------------
#endif
