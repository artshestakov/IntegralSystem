#include "CGConsole.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
bool CGConsole::Question(const QString &QuestionText)
{
	bool Result = false;
	QString Answer;
	while (!Result) //����������, ���� ������������ ������ ���������� �����
	{
		printf("%s (press 'y' or 'n' for anser): ", QuestionText.toStdString().c_str());
		Answer = GetString().toLower(); //����������� �����
        Result = (!Answer.isEmpty() && (Answer == 'y' || Answer == 'n'));
		if (Result) //���� ����� �������� - ��������� ���
		{
			if (Answer == "n") //���� ������� "���" - �������, ���� ������� "��" - ����� �� ����� ���������� ������������� (������ ��� Result � ��� ��� true)
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
