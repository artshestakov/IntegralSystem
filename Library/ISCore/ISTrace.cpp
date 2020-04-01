#include "ISTrace.h"
#include "ISSystem.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISTrace::ISTrace(const QString &q_func_info, const QString &message_text)
{
	Q_FUNC_INFO_STRING = ISSystem::FormatQFuncInfo(q_func_info, ISNamespace::FNF_TypeAndFunction);
	QString MessageText = "START TRACE: " + Q_FUNC_INFO_STRING;

	if (message_text.length())
	{
		MessageText += " | " + message_text;
	}

	ISLOGGER_DEBUG(MessageText);
	Time = new ISCountingTime();
}
//-----------------------------------------------------------------------------
ISTrace::~ISTrace()
{
	ISLOGGER_DEBUG("END TRACE (" + QString::number(Time->Elapsed()) + " msec): " + Q_FUNC_INFO_STRING);
	delete Time;
}
//-----------------------------------------------------------------------------
