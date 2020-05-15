#include "ISTrace.h"
#include "ISSystem.h"
#include "ISLogger.h"
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
	Time = new ISCountingTime();
}
//-----------------------------------------------------------------------------
ISTrace::~ISTrace()
{
	ISLOGGER_T("End " + FunctionName + " " + QString::number(Time->Elapsed()) + " msec");
	delete Time;
}
//-----------------------------------------------------------------------------
