#include "ISTrace.h"
#include "ISLogger.h"
#include "ISAlgorithmOld.h"
//-----------------------------------------------------------------------------
ISTrace::ISTrace(const QString &function_name, const QString &message_text)
	: FunctionName(function_name)
{
	QString MessageText = "Start " + function_name;
	if (!message_text.isEmpty())
	{
		MessageText += " | " + message_text;
	}
	ISLOGGER_T(MessageText);
	TimePoint = ISAlgorithmOld::GetTick();
}
//-----------------------------------------------------------------------------
ISTrace::~ISTrace()
{
	ISLOGGER_T(QString("End %1 %2 msec").arg(FunctionName).arg(ISAlgorithmOld::GetTickDiff(ISAlgorithmOld::GetTick(), TimePoint)));
}
//-----------------------------------------------------------------------------
