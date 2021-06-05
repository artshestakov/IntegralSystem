#include "ISConsole.h"
#include "ISDebug.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
bool ISConsole::Question(const std::string &QuestionText)
{
	bool Result = false;
	std::string Answer;
	while (!Result) //����������, ���� ������������ ������ ���������� �����
	{
		printf("%s (press 'y' or 'n' for anser): ", QuestionText.c_str());
		Answer = GetString(); //����������� �����
        ISAlgorithm::StringToLower(Answer); //�������� ����� � ������� ��������
        Result = (!Answer.empty() && (Answer == "y" || Answer == "n"));
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
void ISConsole::Pause()
{
	ISDEBUG_L("To continue, press \"Enter\" button...");
	(void)getchar();
}
//-----------------------------------------------------------------------------
std::string ISConsole::GetString(const std::string &OutputString)
{
	if (!OutputString.empty())
	{
        std::cout << OutputString;
	}
	std::string String;
	std::getline(std::cin, String);
	return String;
}
//-----------------------------------------------------------------------------
int ISConsole::GetInt(const std::string &OutputString)
{
	if (!OutputString.empty())
	{
        ISDEBUG_L(OutputString);
	}
	std::string Integer;
	std::getline(std::cin, Integer);
	return std::atoi(Integer.c_str());
}
//-----------------------------------------------------------------------------
bool ISConsole::InstallEncoding(unsigned int CodePage, std::string &ErrorString)
{
#if defined(WIN32) && defined(DEBUG)
    if (SetConsoleOutputCP(CodePage) == FALSE) //�� ������� ���������� ������� ��������
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }
#else //���������� ��� Linux �� ����������
    IS_UNUSED(CodePage);
    IS_UNUSED(ErrorString);
#endif
	return true;
}
//-----------------------------------------------------------------------------
