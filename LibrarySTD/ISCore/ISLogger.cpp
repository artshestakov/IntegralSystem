#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
	: ErrorString(NO_ERROR_STRING),
	LastIndex(0),
	IsRunning(false),
	IsFinished(false),
	CurrentDay(0),
	CurrentMonth(0),
	CurrentYear(0)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISLogger::~ISLogger()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
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
	//Получаем текущую дату и время и запоминаем текущий день
	ISDateTime CurrentDate = ISAlgorithm::GetCurrentDate();
	CurrentDay = CurrentDate.Day;

	if (!CreateLogDirectory(CurrentDate)) //Ошибка при создании директорий
	{
		return false;
	}

	std::string path_file = GetPathFile(CurrentDate);
	File.open(path_file.c_str(), std::ios::out | std::ios::app);
	if (!File.is_open()) //Не удалось открыть файл
	{
		ErrorString = "Error open file \"" + path_file + "\": " + ISAlgorithm::GetLastErrorS();
		return false;
	}
	IsRunning = true;
	std::thread(&ISLogger::Worker, this).detach();
	return true;
}
//-----------------------------------------------------------------------------
void ISLogger::Shutdown()
{
    if (IsRunning) //Если логгер был запущен - останавливаем
    {
        //Останавливаем логгер
        CRITICAL_SECTION_LOCK(&CriticalSection);
        IsRunning = false;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

        //Ждём когда он остановится и закрываем файл
        while (!IsFinished)
        {
            ISSleep(1);
        }
        File.close();
    }
}
//-----------------------------------------------------------------------------
void ISLogger::Log(bool is_format, ISNamespace::LogMessageType message_type, const std::string &component, const std::string &message)
{
	if (!IsRunning)
	{
		ISDEBUG_W("Logger is not initialized");
		return;
	}

	std::string string_complete;
	if (is_format) //Если сообщение нужно форматировать
	{
		//Получаем строковый тип сообщения
		std::string message_type_string;
		switch (message_type)
		{
		case ISNamespace::LogMessageType::Unknown: break;
		case ISNamespace::LogMessageType::Debug: message_type_string = "Debug"; break;
		case ISNamespace::LogMessageType::Info: message_type_string = "Info"; break;
		case ISNamespace::LogMessageType::Warning: message_type_string = "Warning"; break;
		case ISNamespace::LogMessageType::Error: message_type_string = "Error"; break;
		case ISNamespace::LogMessageType::Critical: message_type_string = "Critical"; break;
		case ISNamespace::LogMessageType::Trace: message_type_string = "Trace"; break;
		case ISNamespace::LogMessageType::Assert: message_type_string = "Assert"; break;
		}

        //Получаем текущую дату и время
		ISDateTime DateTime = ISAlgorithm::GetCurrentDate();
        
        //Формируем заголовок сообщения
		char buffer[LOGGER_MESSAGE_SIZE] = { 0 };
		if (component.empty()) //Если компонент указан
		{
			snprintf(buffer, LOGGER_MESSAGE_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s] %s",
				DateTime.Day, DateTime.Month, DateTime.Year % 100, DateTime.Hour, DateTime.Minute, DateTime.Second, DateTime.Millisecond,
				CURRENT_THREAD_ID(), message_type_string.c_str(), message.c_str());
		}
		else //Компонент не указан
		{
			snprintf(buffer, LOGGER_MESSAGE_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s][%s] %s",
				DateTime.Day, DateTime.Month, DateTime.Year % 100, DateTime.Hour, DateTime.Minute, DateTime.Second, DateTime.Millisecond,
				CURRENT_THREAD_ID(), message_type_string.c_str(), component.c_str(), message.c_str());
		}
		string_complete = buffer;
	}
	else //Форматирование не нужно - записываем сообщение "как есть"
	{
		string_complete = message;
	}

	CRITICAL_SECTION_LOCK(&CriticalSection);
#ifdef DEBUG //В отладочной версии выводим строку в консоль
	ISDEBUG_L(string_complete);
#ifdef WIN32 //Для Windows выводим строку в консоль Visual Studio
	OutputDebugString((string_complete + '\n').c_str());
#endif
#endif
	Array[LastIndex++] = string_complete;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateLogDirectory(const ISDateTime &DT)
{
	//Запоминаем текущий месяц и год
	CurrentMonth = DT.Month;
	CurrentYear = DT.Year;

	//Формируем путь к текущей папке
	char Buffer[MAX_PATH];
	sprintf(Buffer, "%s%cLogs%c%d%c%02d%c", ISAlgorithm::GetApplicationDir().c_str(), PATH_SEPARATOR, PATH_SEPARATOR, CurrentYear, PATH_SEPARATOR, CurrentMonth, PATH_SEPARATOR);
    PathLogsDir = Buffer;
	
	if (!ISAlgorithm::DirExist(PathLogsDir)) //Если папка с текущим месяцем не существует - создаём её
	{
		if (!ISAlgorithm::DirCreate(PathLogsDir, ErrorString)) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathLogsDir + "\": " + ErrorString;
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
std::string ISLogger::GetPathFile(const ISDateTime &DT) const
{
	char Buffer[MAX_PATH];
	sprintf(Buffer, "%s%s_%02d.%02d.%d.log", PathLogsDir.c_str(), ISAlgorithm::GetApplicationName().c_str(), DT.Day, DT.Month, DT.Year);
	return Buffer;
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (IsRunning)
	{
		ISSleep(LOGGER_TIMEOUT);
        CRITICAL_SECTION_LOCK(&CriticalSection);
		if (LastIndex) //Если в очереди есть сообщения
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				File << Array[i] << std::endl;
				Array[i].clear(); //Очищаем текущую строку
			}
			LastIndex = 0;
			File.flush();
		}
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//Если сменился месяц или год - создаём недостающие папки
		ISDateTime CurrentDate = ISAlgorithm::GetCurrentDate();
		if (CurrentMonth != CurrentDate.Month || CurrentYear != CurrentDate.Year)
		{
			//Пытаемся создать недосающие директории пока не получится
			while (!CreateLogDirectory(CurrentDate))
			{
				ISDEBUG_E(ErrorString);
				ISSleep(LOGGER_TIMEOUT);
			}
		}

		//Если сменился день - закрываем текущий файл и открываем новый
		if (CurrentDay != CurrentDate.Day)
		{
			File.close();

			bool is_opened = false;
			while (!is_opened) //Пытаемся открыть файл
			{
				std::string path_file = GetPathFile(CurrentDate);
				File.open(path_file, std::ios::out | std::ios::app);
				is_opened = File.is_open();
				if (is_opened) //Файл был успешно открыт - запоминаем текущий день
				{
					CurrentDay = CurrentDate.Day;
				}
				else //Файл не удалось открыть, пытаемся сделать это ещё раз через секунду
				{
					ISDEBUG_E("Logger: not open file \"" + path_file + "\". Error: " + ErrorString);
					ISSleep(1000);
				}
			}
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
