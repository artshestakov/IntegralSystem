#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
	: ErrorString(NO_ERROR_STRING),
	LastPosition(0),
	Running(false),
	CurrentYear(0),
	CurrentMonth(0),
	CurrentDay(0)
{
	
}
//-----------------------------------------------------------------------------
ISLogger::~ISLogger()
{
	//Останавливаем обработчик очереди
	Mutex.lock();
	Running = false;
	Mutex.unlock();

	//Ждём пока текущая очередь не будет записана в файл
	while (LastPosition)
	{
		Sleep(100);
	}

	File.close(); //Закрываем файл
}
//-----------------------------------------------------------------------------
ISLogger& ISLogger::Instance()
{
	static ISLogger Logger;
	return Logger;
}
//-----------------------------------------------------------------------------
std::string ISLogger::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLogger::Initialize()
{
	if (Running) //Если логгер уже работает, возвращаем true
	{
		return Running;
	}

	//Получаем путь к исполняемому файлу
	char Temp[MAX_PATH];
	Running = GetModuleFileNameA(NULL, Temp, MAX_PATH) > 0 ? true : false;
	if (Running) //Путь к исполняемому файлу успешно получен
	{
		PathDirectory = &Temp[0];
		size_t Pos = PathDirectory.rfind('\\');
		PathDirectory.erase(Pos + 1, PathDirectory.size() - Pos - 1);
	}
	else //Не удалось получить путь
	{
		ErrorString = "Error getting current module file path.";
		return Running;
	}

	Running = CreateDir();
	if (!Running) //При создании директории произошла ошибка
	{
		return Running;
	}

	UpdateFilePath();
	File.open(PathFile, std::ios::app);
	Running = File.is_open();
	if (Running)
	{
		std::thread(&ISLogger::Worker, this).detach();
	}
	else
	{
		ErrorString = strerror(errno);
	}
	return Running;
}
//-----------------------------------------------------------------------------
void ISLogger::Log(MessageType Type, const std::string &String, const char *SourceName, int Line)
{
	if (LastPosition == ARRAY_MAX_SIZE && !Running) //Если превышен допустимый предел размера очереди или обработчик очереди уже остановился - выходим из функции
	{
		return;
	}

	//Получаем текущую дату и время
	SYSTEMTIME ST;
	GetLocalTime(&ST);

	//Формируем начало строки лога (дата, время, идентификатор текущего потока)
	char Temp[MAX_PATH];
	sprintf(Temp, "%02d.%02d.%02d %02d:%02d:%02d.%03d %d", ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds, GetCurrentThreadId());
	
	//Формируем остальную часть (тип сообщения, имя файла с исходным кодом, номер строки)
	std::stringstream Stream;
	Stream << Temp << " [";
	switch (Type)
	{
	case MessageType::MT_Info: Stream << "INFO"; break;
	case MessageType::MT_Debug: Stream << "DEBUG"; break;
	case MessageType::MT_Warning: Stream << "WARNING"; break;
	case MessageType::MT_Error: Stream << "ERROR"; break;
	}
	Stream << "] [" << SourceName << ":" << Line << "] " << String << std::endl;
	
	Mutex.lock();
	Array[LastPosition++] = Stream.str();
	Mutex.unlock();
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (Running) //Работа потока
	{
		DWORD
		Sleep(LOGGER_TIMEOUT); //Ждём 2 секунды

		Mutex.lock(); //Блокируем мьютекс
		if (LastPosition) //Если очередь не пустая
		{
			for (size_t i = 0; i < LastPosition; ++i) //Обходим очередь
			{
				File << Array[i];
				Array[i].clear();
			}
			File.flush();
			LastPosition = 0;
		}
		Mutex.unlock(); //Разблокируем мьютекс

		//Получаем текущие дату и время
		SYSTEMTIME ST;
		GetLocalTime(&ST);
		
		//Если сменился год/месяц/день - создаём недостающую директорию, обновляем текущий путь к файлу и переоткрываем новый файл
		if (CurrentYear != ST.wYear || CurrentMonth != ST.wMonth || CurrentDay != ST.wDay)
		{
			bool Result = CurrentMonth != ST.wMonth;
			if (Result) //Если сменился месяц - создаём недостающую директорию
			{
				Result = CreateDir();
			}
			else //Месяц не менялся - создание директории не требуется
			{
				Result = true;
			}

			if (Result) //Папка для нового месяца сущется/создана успешно - обновляем путь к файлу и переоткрываем его
			{
				UpdateFilePath();
				File.close();
				File.open(PathFile, std::ios::app); //Обрабатывать (File.is_open() == false)?
			}
		}
	}
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateDir()
{
	//Получаем текущую дату и время и формируем путь к папке Logs
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	PathLogs = PathDirectory + "Logs\\" + std::to_string(ST.wYear) + '\\' + (ST.wMonth < 10 ? '0' + std::to_string(ST.wMonth) : std::to_string(ST.wMonth)) + '\\';

	DWORD Attributes = GetFileAttributesA(PathLogs.c_str());
	bool Result = (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
	if (!Result) //Если папка не существует - создаём её
	{
		Result = SHCreateDirectoryExA(NULL, PathLogs.c_str(), NULL) == 0;
		if (!Result) //Не удалось создать папку
		{
			ErrorString = "Error create dir " + PathLogs;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISLogger::UpdateFilePath()
{
	//Получаем текущую дату и время
	SYSTEMTIME ST;
	GetLocalTime(&ST);

	//Запоминаем год, месяц и день
	CurrentYear = ST.wYear;
	CurrentMonth = ST.wMonth;
	CurrentDay = ST.wDay;

	//Формируем путь к лог-файлу
	std::stringstream Stream;
	Stream << PathLogs << "AuthServer_" << CurrentYear <<
		(CurrentMonth < 10 ? '0' + std::to_string(CurrentMonth) : std::to_string(CurrentMonth)) <<
		(CurrentDay < 10 ? '0' + std::to_string(CurrentDay) : std::to_string(CurrentDay)) <<
		".log";
	PathFile = Stream.str();
}
//-----------------------------------------------------------------------------
