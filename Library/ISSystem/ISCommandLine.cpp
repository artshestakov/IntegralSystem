#include "StdAfx.h"
#include "ISCommandLine.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISCommandLine::ISCommandLine()
{

}
//-----------------------------------------------------------------------------
ISCommandLine::~ISCommandLine()
{

}
//-----------------------------------------------------------------------------
bool ISCommandLine::Question(const QString &QuestionText)
{
	bool Result = true;
	ISDebug::ShowEmptyString(true);
	ISDebug::ShowString(QuestionText);

	string Answer;
	getline(cin, Answer);
	transform(Answer.begin(), Answer.end(), Answer.begin(), ::tolower); //ѕроведение ответа-строки к нижнему регистру

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
	string String;
	getline(cin, String);
	QString Result(String.c_str());

	return Result;
}
//-----------------------------------------------------------------------------
