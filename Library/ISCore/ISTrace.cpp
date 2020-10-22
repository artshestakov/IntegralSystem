#include "ISTrace.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
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
	TimePoint = ISAlgorithm::GetTick();
}
//-----------------------------------------------------------------------------
ISTrace::~ISTrace()
{
	ISLOGGER_T(QString("End %1 %2 msec").arg(FunctionName).arg(ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint)));
}
//-----------------------------------------------------------------------------
