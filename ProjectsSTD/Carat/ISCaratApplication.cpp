#include "ISCaratApplication.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISDebug.h"
#include "ISTcpServer.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv)
	: ErrorString(STRING_NO_ERROR),
	IsRunning(true),
	Arguments(ISAlgorithm::ParseArgs(argc, argv)),
	FileShutdown(ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp" + PATH_SEPARATOR + "Carat.stop")
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISCaratApplication::~ISCaratApplication()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
std::string ISCaratApplication::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Init()
{
	if (!ISLogger::Instance().Initialize()) //�� ������� ��������������� ������
	{
		ISDEBUG_E("Error initialize logger: " + ISLogger::Instance().GetErrorString());
		return false;
	}

	if (!ISAlgorithm::ConsoleSetEncoding(65001, ErrorString))
	{
		ISLOGGER_W("Console", "Not setting console encoding: " + ErrorString);
	}

	//������ ����� Temp
	std::string DirTemp = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp";
	if (!ISAlgorithm::DirExist(DirTemp))
	{
		if (!ISAlgorithm::DirCreate(DirTemp, ErrorString))
		{
			ISLOGGER_E(__CLASS__, "Not created dir (" + DirTemp + "): " + ErrorString);
			return false;
		}
	}

	//������� stop-����
	if (ISAlgorithm::FileExist(FileShutdown))
	{
        if (!ISAlgorithm::FileDelete(FileShutdown, ErrorString))
		{
			ISLOGGER_E(__CLASS__, "Not delete shutdown file (" + FileShutdown + "): " + ErrorString);
			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
int ISCaratApplication::Start()
{
	std::string Argument = Arguments.empty() ? std::string() : Arguments.front();
    if (Argument.empty()) //����� ������
    {
        //��������� ����� �������� ������
        std::thread(&ISCaratApplication::ShutdownController, this).detach();

		ISTcpServer::Instance().Start();

        while (true) //����������� ���� ��������� ������
        {
            ISSleep(1);

            //���������, �������� ������ ��� ��������� ������
            CRITICAL_SECTION_LOCK(&CriticalSection);
            bool is_running = IsRunning;
            CRITICAL_SECTION_UNLOCK(&CriticalSection);
            if (!is_running)
            {
                ISLOGGER_I(__CLASS__, "Shutdown server");

                ISLogger::Instance().Shutdown();
                break;
            }
        }
    }
    else if (Argument == "--shutdown") //����� ��������� ������
	{
		std::ofstream File(FileShutdown);
		File.close();
	}
    else //�������� ���������
	{
        ISDEBUG_L("Invalid argument: " + Argument);
	}
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ShutdownController()
{
	while (true)
	{
        ISSleep(500);
		if (ISAlgorithm::FileExist(FileShutdown)) //���� ���� �������� - ������������� ����������
		{
			CRITICAL_SECTION_LOCK(&CriticalSection);
			IsRunning = false;
			CRITICAL_SECTION_UNLOCK(&CriticalSection);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
