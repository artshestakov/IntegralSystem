#include "ISCommandLine.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
bool ISCommandLine::Question(const QString &QuestionText)
{
	bool Result = true;
	ISDebug::ShowString();
	ISDebug::ShowString(QuestionText);

	std::string Answer;
	std::getline(std::cin, Answer);
	std::transform(Answer.begin(), Answer.end(), Answer.begin(), tolower); //ѕроведение ответа-строки к нижнему регистру

	if (Answer == "y" || Answer == "yes" || Answer == "true" || Answer == "t" || Answer == "ok")
	{
		Result = true;
	}
	else if (Answer == "n" || Answer == "no" || Answer == "false" || Answer == "f")
	{
		Result = false;
	}
	else
	{
		ISDebug::ShowString("Invalid answer!");
		Question(QuestionText);
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCommandLine::Pause()
{
	ISDebug::ShowString("To continue, press \"Enter\" button...");
	getchar();
}
//-----------------------------------------------------------------------------
QString ISCommandLine::GetText()
{
	std::string String;
	std::getline(std::cin, String);
	return String.c_str();
}
//-----------------------------------------------------------------------------
