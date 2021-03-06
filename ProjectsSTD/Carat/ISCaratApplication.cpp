#include "ISCaratApplication.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv)
	: ErrorString(NO_ERROR_STRING),
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
	if (!ISAlgorithm::ConsoleSetEncoding(65001, ErrorString))
	{
		//warning...
	}

	//������ ����� Temp
	std::string DirTemp = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp";
	if (!ISAlgorithm::DirExist(DirTemp))
	{
		if (!ISAlgorithm::DirCreate(DirTemp, ErrorString))
		{
			//error...
			return false;
		}
	}

	//������� stop-����
	if (ISAlgorithm::FileExist(FileShutdown))
	{
		if (!ISAlgorithm::FileDelete(FileShutdown))
		{
			//error...
			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
int ISCaratApplication::Start()
{
	std::string Argument = Arguments.empty() ? std::string() : Arguments.front();
	if (Argument == "--shutdown") //����� ��������� ������
	{
		std::ofstream File(FileShutdown);
		File.close();
		return EXIT_SUCCESS;
	}
	else //����� ������
	{
		//��������� ����� �������� ������
		std::thread(&ISCaratApplication::ShutdownController, this).detach();

		while (true) //����������� ���� ��������� ������
		{
			ISSleep(1);

			//���������, �������� ������ ��� ��������� ������
			CRITICAL_SECTION_LOCK(&CriticalSection);
			bool is_running = IsRunning;
			CRITICAL_SECTION_UNLOCK(&CriticalSection);
			if (!is_running)
			{
				break;
			}
		}
		return EXIT_SUCCESS;
	}
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ShutdownController()
{
	while (true)
	{
		ISSleep(1000);
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
