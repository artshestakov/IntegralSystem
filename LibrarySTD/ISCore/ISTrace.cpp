#include "ISTrace.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTrace::ISTrace(const std::string &function_name, const std::string &message_text)
	: FunctionName(function_name)
{
	std::string MessageText = "Start " + function_name;
	if (!message_text.empty())
	{
		MessageText += " | " + message_text;
	}
	ISLOGGER_T(MessageText.c_str());
	TimePoint = ISAlgorithm::GetTick();
}
//-----------------------------------------------------------------------------
ISTrace::~ISTrace()
{
	ISLOGGER_T("End %1 %2 msec", FunctionName.c_str(), ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint));
}
//-----------------------------------------------------------------------------
