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
	//�������� ������� ���� � ����� � ���������� ������� ����
	QDate CurrentDate = QDate::currentDate();
	CurrentDay = CurrentDate.day();

	if (!CreateLogDirectory(CurrentDate)) //������ ��� �������� ����������
	{
		return false;
	}

	QString path_file = GetPathFile(CurrentDate);
	File.setFileName(path_file);
	if (!File.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) //�� ������� ������� ����
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
    if (IsRunning) //���� ������ ��� ������� - �������������
    {
        //������������� ������
        CRITICAL_SECTION_LOCK(&CriticalSection);
        IsRunning = false;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

        //��� ����� �� ����������� � ��������� ����
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
	if (is_format) //���� ��������� ����� �������������
	{
		//�������� ��������� ��� ���������
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

        //�������� ������� ���� � �����
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
        //��������� ��������� ���������
		char buffer[LOGGER_MESSAGE_SIZE] = { 0 };
		if (component.empty()) //���� ��������� ������
		{
			snprintf(buffer, LOGGER_MESSAGE_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s] %s",
                Day, Month, Year % 100, Hour, Minute, Second, Millisecond,
				CURRENT_THREAD_ID(), message_type_string.c_str(), string.toStdString().c_str());
		}
		else //��������� �� ������
		{
			snprintf(buffer, LOGGER_MESSAGE_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s][%s] %s",
                Day, Month, Year % 100, Hour, Minute, Second, Millisecond,
				CURRENT_THREAD_ID(), message_type_string.c_str(), component.c_str(), string.toStdString().c_str());
		}
		string_complete = buffer;
	}
	else //�������������� �� ����� - ���������� ��������� "��� ����"
	{
		string_complete = string.toStdString();
	}

	CRITICAL_SECTION_LOCK(&CriticalSection);
#ifdef DEBUG //� ���������� ������ ������� ������ � �������
    std::cout << string_complete << std::endl;
#ifdef WIN32 //��� Windows ������� ������ � ������� Visual Studio
	OutputDebugString((string_complete + '\n').c_str());
#endif
#endif
	Array[LastIndex++] = string_complete;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISLoggerOld::CreateLogDirectory(const QDate &Date)
{
	//���������� ������� ����� � ���
	CurrentMonth = Date.month();
	CurrentYear = Date.year();

	//��������� ���� � ������� �����
    PathLogsDir = QCoreApplication::applicationDirPath() + "/Logs/" + QString::number(CurrentYear) + '/' + QString("%1").arg(CurrentMonth, 2, 'g', -1, '0') + '/';
	
	QDir Dir;
	if (!Dir.exists(PathLogsDir)) //���� ����� � ������� ������� �� ���������� - ������ �
	{
		if (!Dir.mkpath(PathLogsDir)) //������ �������� �����
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
		if (LastIndex) //���� � ������� ���� ���������
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				std::string message_string = Array[i] + "\n";
				size_t message_size = message_string.size();
				if (File.write(message_string.c_str(), message_size) == -1) //�� ������� ���������� ������ � ����
				{
					std::cout << "Logger: not write to file. Error: " << File.errorString().toStdString() << std::endl;
				}
				Array[i].clear(); //������� ������� ������
			}
			LastIndex = 0;

			if (!File.flush()) //�� ������� �������� ���������� � ���� �������������
			{
                std::cout << "Logger: not flushing. Error: " << File.errorString().toStdString() << std::endl;
			}
		}
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//���� �������� ����� ��� ��� - ������ ����������� �����
		QDate CurrentDate = QDate::currentDate();
		if (CurrentMonth != CurrentDate.month() || CurrentYear != CurrentDate.year())
		{
			//�������� ������� ���������� ���������� ���� �� ���������
			while (!CreateLogDirectory(CurrentDate))
			{
                std::cout << ErrorString.toStdString() << std::endl;
				ISSleep(LOGGER_TIMEOUT);
			}
		}

		//���� �������� ���� - ��������� ������� ���� � ��������� �����
		if (CurrentDay != CurrentDate.day())
		{
			File.close();

			bool is_opened = false;
			while (!is_opened) //�������� ������� ����
			{
				QString path_file = GetPathFile(CurrentDate);
				File.setFileName(path_file);
				is_opened = File.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
				if (is_opened) //���� ��� ������� ������ - ���������� ������� ����
				{
					CurrentDay = CurrentDate.day();
				}
				else //���� �� ������� �������, �������� ������� ��� ��� ��� ����� �������
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
