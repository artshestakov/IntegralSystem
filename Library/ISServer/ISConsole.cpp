#include "ISConsole.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
bool ISConsole::Question(const QString &QuestionText)
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
void ISConsole::Pause()
{
	ISDEBUG_L("To continue, press \"Enter\" buttons...");
	(void)getchar();
}
//-----------------------------------------------------------------------------
QString ISConsole::GetString(const QString &OutputString)
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
int ISConsole::GetInt(const QString &OutputString)
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
bool ISConsole::InstallEncoding(unsigned int CodePage)
{
#if defined(WIN32) && defined(DEBUG)
	if (SetConsoleOutputCP(CodePage) == FALSE)
	{
		return false;
	}
#endif
    Q_UNUSED(CodePage);
	return true;
}
//-----------------------------------------------------------------------------
