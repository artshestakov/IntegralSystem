#include "CGConsole.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
bool CGConsole::Question(const QString &QuestionText)
{
	bool Result = false;
	QString Answer;
	while (!Result) //Дожидаемся, пока пользователь введет правильный ответ
	{
		printf("%s (press 'y' or 'n' for anser): ", QuestionText.toStdString().c_str());
		Answer = GetString().toLower(); //Запрашиваем ответ
        Result = (!Answer.isEmpty() && (Answer == 'y' || Answer == 'n'));
		if (Result) //Если ответ валидный - проверяем его
		{
			if (Answer == "n") //Если введено "НЕТ" - выходим, если введено "ДА" - выход из цикла произойдет автоматически (потому что Result и так уже true)
			{
				Result = false;
				break;
			}
		}
		else
		{
			ISDEBUG_L("Invalid answer!");
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void CGConsole::Pause()
{
	ISDEBUG_L("To continue, press \"Enter\" buttons...");
	(void)getchar();
}
//-----------------------------------------------------------------------------
QString CGConsole::GetString(const QString &OutputString)
{
	if (!OutputString.isEmpty())
	{
		printf("%s", OutputString.toStdString().c_str());
	}

	std::string String;
	std::getline(std::cin, String);
	return QString::fromStdString(String);
}
//-----------------------------------------------------------------------------
int CGConsole::GetInt(const QString &OutputString)
{
	if (!OutputString.isEmpty())
	{
		printf("%s", OutputString.toStdString().c_str());
	}

	std::string Integer;
	std::getline(std::cin, Integer);
	return atoi(Integer.c_str());
}
//-----------------------------------------------------------------------------
