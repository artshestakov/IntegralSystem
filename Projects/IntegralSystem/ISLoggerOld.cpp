#include "ISLoggerOld.h"
#include "ISAlgorithm.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISLoggerOld::ISLoggerOld()
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
ISLoggerOld::~ISLoggerOld()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISLoggerOld& ISLoggerOld::Instance()
{
	static ISLoggerOld Logger;
	return Logger;
}
//-----------------------------------------------------------------------------
QString ISLoggerOld::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLoggerOld::Initialize()
{
	//Получаем текущую дату и время и запоминаем текущий день
	QDate CurrentDate = QDate::currentDate();
	CurrentDay = CurrentDate.day();

	if (!CreateLogDirectory(CurrentDate)) //Ошибка при создании директорий
	{
		return false;
	}

	QString path_file = GetPathFile(CurrentDate);
	File.setFileName(path_file);
	if (!File.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) //Не удалось открыть файл
	{
		ErrorString = QString("Error open file \"%1\": %2").arg(path_file).arg(strerror(errno));
		return false;
	}
	IsRunning = true;
	std::thread(&ISLoggerOld::Worker, this).detach();
	return true;
}
//-----------------------------------------------------------------------------
void ISLoggerOld::Shutdown()
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
void ISLoggerOld::Log(bool is_format, ISNamespace::LogMessageType message_type, const std::string &component, const QString &string)
{
	if (!IsRunning)
	{
		ISDEBUG_E("Logger is not initialized");
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
		//case ISNamespace::LogMessageType::QT: message_type_string = "QT"; break;
		}

        //Получаем текущую дату и время
        unsigned int Day = 0, Month = 0, Year = 0, Hour = 0, Minute = 0, Second = 0, Millisecond = 0;
#ifdef WIN32
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);
        Day = SystemTime.wDay;
        Month = SystemTime.wMonth;
        Year = SystemTime.wYear;
        Hour = SystemTime.wHour;
        Minute = SystemTime.wMinute;
        Second = SystemTime.wSecond;
        Millisecond = SystemTime.wMilliseconds;
#else
        struct timeval TimeValue;
        gettimeofday(&TimeValue, NULL);

        struct tm *SystemTime = localtime(&TimeValue.tv_sec);
        Day = SystemTime->tm_mday;
        Month = SystemTime->tm_mon + 1;
        Year = SystemTime->tm_year + 1900;
        Hour = SystemTime->tm_hour;
        Minute = SystemTime->tm_min;
        Second = SystemTime->tm_sec;
        Millisecond = (unsigned int)(TimeValue.tv_usec / 1000);
#endif
        //Формируем заголовок сообщения
		char buffer[LOGGER_MESSAGE_SIZE] = { 0 };
		if (component.empty()) //Если компонент указан
		{
			snprintf(buffer, LOGGER_MESSAGE_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s] %s",
                Day, Month, Year % 100, Hour, Minute, Second, Millisecond,
				CURRENT_THREAD_ID(), message_type_string.c_str(), string.toStdString().c_str());
		}
		else //Компонент не указан
		{
			snprintf(buffer, LOGGER_MESSAGE_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s][%s] %s",
                Day, Month, Year % 100, Hour, Minute, Second, Millisecond,
				CURRENT_THREAD_ID(), message_type_string.c_str(), component.c_str(), string.toStdString().c_str());
		}
		string_complete = buffer;
	}
	else //Форматирование не нужно - записываем сообщение "как есть"
	{
		string_complete = string.toStdString();
	}

	CRITICAL_SECTION_LOCK(&CriticalSection);
#ifdef DEBUG //В отладочной версии выводим строку в консоль
    std::cout << string_complete << std::endl;
#ifdef WIN32 //Для Windows выводим строку в консоль Visual Studio
	OutputDebugString((string_complete + '\n').c_str());
#endif
#endif
	Array[LastIndex++] = string_complete;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISLoggerOld::CreateLogDirectory(const QDate &Date)
{
	//Запоминаем текущий месяц и год
	CurrentMonth = Date.month();
	CurrentYear = Date.year();

	//Формируем путь к текущей папке
    PathLogsDir = QCoreApplication::applicationDirPath() + "/Logs/" + QString::number(CurrentYear) + '/' + QString("%1").arg(CurrentMonth, 2, 'g', -1, '0') + '/';
	
	QDir Dir;
	if (!Dir.exists(PathLogsDir)) //Если папка с текущим месяцем не существует - создаём её
	{
		if (!Dir.mkpath(PathLogsDir)) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathLogsDir + "\"";
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
QString ISLoggerOld::GetPathFile(const QDate &Date) const
{
	return PathLogsDir + QCoreApplication::applicationName() + '_' + Date.toString(FORMAT_DATE_V2) + SYMBOL_POINT + EXTENSION_LOG;
}
//-----------------------------------------------------------------------------
void ISLoggerOld::Worker()
{
	while (IsRunning)
	{
		ISSleep(LOGGER_TIMEOUT);
        CRITICAL_SECTION_LOCK(&CriticalSection);
		if (LastIndex) //Если в очереди есть сообщения
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				std::string message_string = Array[i] + "\n";
				size_t message_size = message_string.size();
				if (File.write(message_string.c_str(), message_size) == -1) //Не удалось произвести запись в файл
				{
					std::cout << "Logger: not write to file. Error: " << File.errorString().toStdString() << std::endl;
				}
				Array[i].clear(); //Очищаем текущую строку
			}
			LastIndex = 0;

			if (!File.flush()) //Не удалось сбросить информацию в файл принудительно
			{
                std::cout << "Logger: not flushing. Error: " << File.errorString().toStdString() << std::endl;
			}
		}
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//Если сменился месяц или год - создаём недостающие папки
		QDate CurrentDate = QDate::currentDate();
		if (CurrentMonth != CurrentDate.month() || CurrentYear != CurrentDate.year())
		{
			//Пытаемся создать недосающие директории пока не получится
			while (!CreateLogDirectory(CurrentDate))
			{
                std::cout << ErrorString.toStdString() << std::endl;
				ISSleep(LOGGER_TIMEOUT);
			}
		}

		//Если сменился день - закрываем текущий файл и открываем новый
		if (CurrentDay != CurrentDate.day())
		{
			File.close();

			bool is_opened = false;
			while (!is_opened) //Пытаемся открыть файл
			{
				QString path_file = GetPathFile(CurrentDate);
				File.setFileName(path_file);
				is_opened = File.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
				if (is_opened) //Файл был успешно открыт - запоминаем текущий день
				{
					CurrentDay = CurrentDate.day();
				}
				else //Файл не удалось открыть, пытаемся сделать это ещё раз через секунду
				{
                    std::cout << "Logger: not open file \"" << path_file.toStdString() << "\". Error: " << File.errorString().toStdString() << std::endl;
					ISSleep(1000);
				}
			}
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
