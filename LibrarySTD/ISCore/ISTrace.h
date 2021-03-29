#pragma once
#ifndef _ISTRACE_H_INCLUDED
#define _ISTRACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
#define ISTRACE_MSG(MESSAGE) ISTrace __trace_helper(__FUNCTION__, MESSAGE);
#define ISTRACE() ISTrace __trace_helper(__FUNCTION__, std::string());
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISTrace
{
public:
    ISTrace(const std::string &function_name, const std::string &message_text);
    ~ISTrace();

private:
    ISTimePoint TimePoint;
    std::string FunctionName;
};
//-----------------------------------------------------------------------------
#endif
