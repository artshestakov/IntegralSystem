#include "ISLogger.h"
#include "ISAlgorithm.h"
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
	//�������� ������� ���� � ����� � ���������� ������� ����
	SYSTEMTIME CurrentDate = ISAlgorithm::GetCurrentDate();
	CurrentDay = CurrentDate.wDay;

	if (!CreateLogDirectory(CurrentDate)) //������ ��� �������� ����������
	{
		return false;
	}

	std::string path_file = GetPathFile(CurrentDate);
	File.open(path_file.c_str(), std::ios::out);
	if (!File.is_open()) //�� ������� ������� ����
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
void ISLogger::Log(bool is_format, ISNamespace::LogMessageType message_type, const std::string &component, const std::string &message)
{
	if (!IsRunning)
	{
		//ISDEBUG_E("Logger is not initialized");
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
				CURRENT_THREAD_ID(), message_type_string.c_str(), message.c_str());
		}
		else //��������� �� ������
		{
			snprintf(buffer, LOGGER_MESSAGE_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s][%s] %s",
                Day, Month, Year % 100, Hour, Minute, Second, Millisecond,
				CURRENT_THREAD_ID(), message_type_string.c_str(), component.c_str(), message.c_str());
		}
		string_complete = buffer;
	}
	else //�������������� �� ����� - ���������� ��������� "��� ����"
	{
		string_complete = message;
	}

	CRITICAL_SECTION_LOCK(&CriticalSection);
#ifdef DEBUG //� ���������� ������ ������� ������ � �������
	//ISDEBUG_L(QString::fromStdString(string_complete));
#ifdef WIN32 //��� Windows ������� ������ � ������� Visual Studio
	OutputDebugString((string_complete + '\n').c_str());
#endif
#endif
	Array[LastIndex++] = string_complete;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateLogDirectory(const SYSTEMTIME &ST)
{
	//���������� ������� ����� � ���
	CurrentMonth = ST.wMonth;
	CurrentYear = ST.wYear;

	//��������� ���� � ������� �����
    PathLogsDir = ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Logs" + PATH_SEPARATOR + std::to_string(CurrentYear) + PATH_SEPARATOR + std::to_string(CurrentMonth) + PATH_SEPARATOR;
	
	if (!ISAlgorithm::DirExist(PathLogsDir)) //���� ����� � ������� ������� �� ���������� - ������ �
	{
		if (!ISAlgorithm::DirCreate(PathLogsDir, ErrorString)) //������ �������� �����
		{
			ErrorString = "Error create directory \"" + PathLogsDir + "\": " + ErrorString;
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
std::string ISLogger::GetPathFile(const SYSTEMTIME &ST) const
{
	return PathLogsDir + ISAlgorithm::GetApplicationName() + '_' +
		std::to_string(ST.wDay) + std::to_string(ST.wMonth) + std::to_string(ST.wYear) + ".log";
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (IsRunning)
	{
		ISSleep(LOGGER_TIMEOUT);
        CRITICAL_SECTION_LOCK(&CriticalSection);
		if (LastIndex) //���� � ������� ���� ���������
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				//std::string message_string = Array[i] + "\n";
				//size_t message_size = message_string.size();
				//if (File.write(message_string.c_str(), message_size) == -1) //�� ������� ���������� ������ � ����
				{
					//ISDEBUG_E("Logger: not write to file. Error: " + File.errorString());
				}
				Array[i].clear(); //������� ������� ������
			}
			LastIndex = 0;

			//if (!File.flush()) //�� ������� �������� ���������� � ���� �������������
			{
				//ISDEBUG_E("Logger: not flushing. Error: " + File.errorString());
			}
		}
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//���� �������� ����� ��� ��� - ������ ����������� �����
		SYSTEMTIME CurrentDate = ISAlgorithm::GetCurrentDate();
		if (CurrentMonth != CurrentDate.wMonth || CurrentYear != CurrentDate.wYear)
		{
			//�������� ������� ���������� ���������� ���� �� ���������
			while (!CreateLogDirectory(CurrentDate))
			{
				//ISDEBUG_E(ErrorString);
				ISSleep(LOGGER_TIMEOUT);
			}
		}

		//���� �������� ���� - ��������� ������� ���� � ��������� �����
		if (CurrentDay != CurrentDate.wDay)
		{
			File.close();

			bool is_opened = false;
			while (!is_opened) //�������� ������� ����
			{
				std::string path_file = GetPathFile(CurrentDate);
				File.open(path_file);
				is_opened = File.is_open();
				if (is_opened) //���� ��� ������� ������ - ���������� ������� ����
				{
					CurrentDay = CurrentDate.wDay;
				}
				else //���� �� ������� �������, �������� ������� ��� ��� ��� ����� �������
				{
					//ISDEBUG_E("Logger: not open file \"" + path_file + "\". Error: " + File.errorString());
					ISSleep(1000);
				}
			}
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
