#pragma once
//-----------------------------------------------------------------------------
#include "issystem_global.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
#define IS_TRACE_MSG(MESSAGE) ISTrace __trace_helper(Q_FUNC_INFO, MESSAGE);
#define IS_TRACE() ISTrace __trace_helper(Q_FUNC_INFO, QString());
//-----------------------------------------------------------------------------
class ISSYSTEM_EXPORT ISTrace : public QObject
{
	Q_OBJECT

public:
	ISTrace(const QString &q_func_info, const QString &message_text);
	~ISTrace();

private:
	ISCountingTime *Time;
	QString Q_FUNC_INFO_STRING;
};
//-----------------------------------------------------------------------------
