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
        PathDirectory = GetCurrentDirectory();
        Running = !PathDirectory.empty();
        if (!Running) //Не удалось получить путь
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
    ISDateTime DateTime = GetCurrentDateTime();

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
        //itoa(DateTime.Year, Year, 10); //Преобразование года в строку
        sprintf(Year, "%d", DateTime.Year);
        sprintf(Temp, "%02d.%02d.%c%c %02d:%02d:%02d.%03d %d", DateTime.Day, DateTime.Month, Year[2], Year[3], DateTime.Hour, DateTime.Minute, DateTime.Second, DateTime.Milliseconds, 10); //???

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
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        ISDateTime DateTime = GetCurrentDateTime();
		
		//Если сменился год/месяц/день - создаём недостающую директорию, обновляем текущий путь к файлу и переоткрываем новый файл
        if (CurrentYear != DateTime.Year || CurrentMonth != DateTime.Month || CurrentDay != DateTime.Day)
		{
            bool Result = CurrentMonth != DateTime.Month;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(LOGGER_TIMEOUT)); //Ждём 2 секунды
	}
	Finished = true;
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateDir()
{
	//Получаем текущую дату и время и формируем путь к папке Logs
    ISDateTime DateTime = GetCurrentDateTime();
    PathLogs = PathDirectory + PATH_SEPARATOR + "Logs" + PATH_SEPARATOR + std::to_string(DateTime.Year) + PATH_SEPARATOR + (DateTime.Month < 10 ? '0' + std::to_string(DateTime.Month) : std::to_string(DateTime.Month));

#ifdef WIN32
	DWORD Attributes = GetFileAttributesA(PathLogs.c_str());
	bool Result = (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
	if (!Result) //Если папка не существует - создаём её
	{
		Result = SHCreateDirectoryExA(NULL, PathLogs.c_str(), NULL) == 0;
		if (!Result) //Не удалось создать папку
		{
            ErrorString = "Error create dir \"" + PathLogs + "\"";
		}
	}
#else
    struct stat sb;
    bool Result = stat(PathLogs.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
    if (!Result) //Если папка не существует - создаём её
    {
        std::string Command = "mkdir -p " + PathLogs;
        Result = system(Command.c_str()) == 0;
        if (!Result)
        {
            ErrorString = "Error create dir \"" + PathLogs + "\"";
        }
    }
#endif
	return Result;
}
//-----------------------------------------------------------------------------
void ISLogger::UpdateFilePath()
{
	//Получаем текущую дату и время
    ISDateTime DateTime = GetCurrentDateTime();

	//Запоминаем год, месяц и день
    CurrentYear = DateTime.Year;
    CurrentMonth = DateTime.Month;
    CurrentDay = DateTime.Day;

	//Формируем путь к лог-файлу
	std::stringstream Stream;
    Stream << PathLogs << PATH_SEPARATOR << FilePrefix << "_" << CurrentYear <<
		(CurrentMonth < 10 ? '0' + std::to_string(CurrentMonth) : std::to_string(CurrentMonth)) <<
		(CurrentDay < 10 ? '0' + std::to_string(CurrentDay) : std::to_string(CurrentDay)) <<
		SYMBOL_POINT << EXTENSION_LOG;
	PathFile = Stream.str();
}
//-----------------------------------------------------------------------------
ISDateTime ISLogger::GetCurrentDateTime()
{
    ISDateTime DT;
#ifdef WIN32
    SYSTEMTIME ST;
    GetLocalTime(&ST);
    DT.Day = ST.wDay;
    DT.Month = ST.wMonth;
    DT.Year = ST.wYear;
    DT.Hour = ST.wHour;
    DT.Minute = ST.wMinute;
    DT.Second = ST.wSecond;
    DT.Milliseconds = ST.wMilliseconds;
#else
    struct timeval TV;
    gettimeofday(&TV, NULL);
    int Millisecond = lrint(TV.tv_usec / 1000.0);
    if (Millisecond >= 1000)
    {
        Millisecond -= 1000;
        ++TV.tv_sec;
    }
    struct tm *TM = localtime(&TV.tv_sec);
    DT.Day = TM->tm_mday;
    DT.Month = TM->tm_mon + 1;
    DT.Year = TM->tm_year + 1900;
    DT.Hour = TM->tm_hour;
    DT.Minute = TM->tm_min;
    DT.Second = TM->tm_sec;
    DT.Milliseconds = Millisecond;
#endif
    return DT;
}
//-----------------------------------------------------------------------------
std::string ISLogger::GetCurrentDirectory()
{
    std::string DirPath;
#ifdef WIN32
    // Windows
#else
    char *Char = NULL;
    Char = getcwd(Char, MAX_PATH);
    if (Char)
    {
        DirPath = std::string(Char);
        free(Char);
    }
#endif
    return DirPath;
}
//-----------------------------------------------------------------------------
