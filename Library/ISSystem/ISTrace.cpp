#include "StdAfx.h"
#include "ISTrace.h"
#include "ISDebug.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISTrace::ISTrace(const QString &q_func_info, const QString &message_text)
{
	Q_FUNC_INFO_STRING = ISSystem::FormatQFuncInfo(q_func_info, ISNamespace::FNF_TypeAndFunction);
	QString MessageText = "START TRACE: " + Q_FUNC_INFO_STRING;

	if (message_text.length())
	{
		MessageText += " | " + message_text;
	}

	ISDebug::ShowDebugString(MessageText);
	Time = new ISCountingTime(this);
}
//-----------------------------------------------------------------------------
ISTrace::~ISTrace()
{
	ISDebug::ShowDebugString("END TRACE (" + QString::number(Time->GetElapsed()) + " msec): " + Q_FUNC_INFO_STRING);
}
//-----------------------------------------------------------------------------
