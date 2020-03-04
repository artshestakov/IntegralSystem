#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
	: ErrorString(NO_ERROR_STRING),
	LastPosition(0),
	Running(false),
	Finished(false),
	CurrentYear(0),
	CurrentMonth(0),
	CurrentDay(0),
	EnableOutPrintf(false),
	EnableOutFile(false)
{
	
}
//-----------------------------------------------------------------------------
ISLogger::~ISLogger()
{
	
}
//-----------------------------------------------------------------------------
ISLogger& ISLogger::Instance()
{
	static ISLogger Logger;
	return Logger;
}
//-----------------------------------------------------------------------------
QString ISLogger::GetErrorString() const
{
	return QString::fromStdString(ErrorString);
}
//-----------------------------------------------------------------------------
bool ISLogger::Initialize(bool OutPrintf, bool OutFile, const std::string &file_prefix)
{
	if (Running) //Если логгер уже работает, возвращаем true
	{
		return Running;
	}

	EnableOutPrintf = OutPrintf;
	EnableOutFile = OutFile;

	//Если префикс не задан - используем префикс по умолчанию, иначе - тот что был задан
	FilePrefix = file_prefix.empty() ? LOGGER_NAME_DEFAULT : file_prefix;

	if (EnableOutFile) //Если включен вывод в файл - выполняем соответствующие подготовки
	{
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
	}
	else
	{
		Running = true;
	}
	return Running;
}
//-----------------------------------------------------------------------------
void ISLogger::Log(ISNamespace::DebugMessageType Type, const QString &String, const char *SourceName, int Line)
{
	if (LastPosition == LOGGER_ARRAY_SIZE || !Running) //Если превышен допустимый предел размера очереди или обработчик очереди уже остановился - выходим из функции
	{
		return;
	}

	//Получаем текущую дату и время
	SYSTEMTIME ST;
	GetLocalTime(&ST);

	if (Type == ISNamespace::DMT_Unknown)
	{
		fprintf(stdout, "%s\n", String.toStdString().c_str());
		fflush(stdout);

#ifdef DEBUG
		OutputDebugString(String.toStdString().c_str());
		OutputDebugString("\n");
#endif
	}
	else
	{
		//Формируем начало строки лога (дата, время, идентификатор текущего потока)
		char Temp[MAX_PATH];
		itoa(ST.wYear, Year, 10); //Преобразование года в строку
		sprintf(Temp, "%02d.%02d.%c%c %02d:%02d:%02d.%03d %d", ST.wDay, ST.wMonth, Year[2], Year[3], ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds, GetCurrentThreadId());

		//Формируем остальную часть (тип сообщения, имя файла с исходным кодом, номер строки)
		std::stringstream Stream;
		Stream << Temp << " [";
		switch (Type)
		{
		case ISNamespace::DMT_Info: Stream << "Info"; break;
		case ISNamespace::DMT_Debug: Stream << "Debug"; break;
		case ISNamespace::DMT_Warning: Stream << "Warning"; break;
		case ISNamespace::DMT_Error: Stream << "Error"; break;
		}
		Stream << "]";

		if (EnableOutPrintf) //Если включена опция вывода в консоль - выводим
		{
			fprintf(stdout, "%s %s\n", Stream.str().c_str(), String.toStdString().c_str());
			fflush(stdout);

#ifdef DEBUG
			OutputDebugString(Stream.str().c_str());
			OutputDebugString(" ");
			OutputDebugString(String.toStdString().c_str());
			OutputDebugString("\n");
#endif
		}

		if (EnableOutFile) //Если включена опция записи в файл - дополняем именем файла с исходным кодом, номером строки и добавляем в очередь
		{
			Stream << " [" << SourceName << ":" << Line << "] " << String.toStdString() << std::endl;
			Mutex.lock();
			Array[LastPosition++] = Stream.str();
			Mutex.unlock();
		}
	}
}
//-----------------------------------------------------------------------------
void ISLogger::Shutdown()
{
	if (!Running) //Попытка остановить не запущенный логгер
	{
		return;
	}

	if (EnableOutFile) //Если включен вывод в файл
	{
		//Останавливаем обработчик очереди
		Mutex.lock();
		Running = false;
		Mutex.unlock();

		//Ждём пока логгер не остановится
		while (!Finished)
		{
			Sleep(100);
		}
		File.close(); //Закрываем файл
	}
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (true) //Работа потока
	{
		Mutex.lock(); //Блокируем мьютекс
		bool running = Running;
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

		if (!running)
		{
			break;
		}
		Sleep(LOGGER_TIMEOUT); //Ждём 2 секунды
	}
	Finished = true;
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
	Stream << PathLogs << FilePrefix << "_" << CurrentYear <<
		(CurrentMonth < 10 ? '0' + std::to_string(CurrentMonth) : std::to_string(CurrentMonth)) <<
		(CurrentDay < 10 ? '0' + std::to_string(CurrentDay) : std::to_string(CurrentDay)) <<
		SYMBOL_POINT << EXTENSION_LOG;
	PathFile = Stream.str();
}
//-----------------------------------------------------------------------------
