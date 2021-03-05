#include "ISCaratApplication.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int argc, char **argv)
	: ErrorString(NO_ERROR_STRING),
	IsRunning(true)
{
	IS_UNUSED(argc);
	IS_UNUSED(argv);
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
	ISAlgorithm::ConsoleSetEncoding(/*65001*/0);

	if (!ISAlgorithm::DirCreate(ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp", ErrorString))
	{
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
int ISCaratApplication::Start()
{
	//«апускаем поток контрол€ работы
	std::thread(&ISCaratApplication::ShutdownController, this).detach();

	while (true) //Ѕесконечный цикл основного потока
	{
		ISSleep(1);

		//ѕровер€ем, работать дальше или завершить работу
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
//-----------------------------------------------------------------------------
void ISCaratApplication::ShutdownController()
{
	std::string FilePath = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Temp" + PATH_SEPARATOR + "Carat.stop";
	while (true)
	{
		ISSleep(1000);

		CRITICAL_SECTION_LOCK(&CriticalSection);
		IsRunning = false;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		break;
	}
}
//-----------------------------------------------------------------------------
