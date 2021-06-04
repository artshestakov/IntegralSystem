#pragma once
#ifndef _ISTRACE_H_INCLUDED
#define _ISTRACE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
#define ISTRACE_MSG(MESSAGE) ISTrace __trace_helper(__FUNCTION__, MESSAGE);
#define ISTRACE() ISTrace __trace_helper(__FUNCTION__, QString());
//-----------------------------------------------------------------------------
class ISTrace
{
public:
	ISTrace(const QString &function_name, const QString &message_text);
	~ISTrace();

private:
	ISTimePoint TimePoint;
	QString FunctionName;
};
//-----------------------------------------------------------------------------
#endif
