#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
#include "ISDebug.h"
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
	: ErrorString(NO_ERROR_STRING),
	LastIndex(0),
	IsRunning(false),
	IsFinished(false)
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
QString ISLogger::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLogger::Initialize()
{
	//�������� ������� ���� � ����� � ���������� ������� ����
	QDate CurrentDate = QDate::currentDate();
	CurrentDay = CurrentDate.day();

	if (!CreateLogDirectory(CurrentDate)) //������ ��� �������� ����������
	{
		return false;
	}

	QString path_file = GetPathFile(CurrentDate);
	File.open(path_file.toStdString().c_str(), std::ios::app);
	if (!File.is_open()) //�� ������� ������� ����
	{
		ErrorString = QString("Error open file \"%1\": %2").arg(path_file).arg(strerror(errno));
		return false;
	}

	IsRunning = true;
	std::thread(&ISLogger::Worker, this).detach();
	return true;
}
//-----------------------------------------------------------------------------
void ISLogger::Shutdown()
{
	//������������� ������
    CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);

	//��� ����� �� ����������� � ��������� ����
	while (!IsFinished)
	{
        ISSleep(LOGGER_TIMEOUT);
	}
	File.close();
}
//-----------------------------------------------------------------------------
void ISLogger::Log(bool is_format, MessageType message_type, const std::string &component, const QString &string)
{
	std::string string_complete;
	if (is_format) //���� ��������� ����� �������������
	{
		//�������� ��������� ��� ���������
		std::string message_type_string;
		switch (message_type)
		{
		case MessageType::MT_Unknown: break;
		case MessageType::MT_Debug: message_type_string = "Debug"; break;
		case MessageType::MT_Info: message_type_string = "Info"; break;
		case MessageType::MT_Warning: message_type_string = "Warning"; break;
		case MessageType::MT_Error: message_type_string = "Error"; break;
		case MessageType::MT_Critical: message_type_string = "Critical"; break;
		case MessageType::MT_Trace: message_type_string = "Trace"; break;
		case MessageType::MT_Assert: message_type_string = "Assert"; break;
		}

		//�������� ������� ���� � ����� � ��������� ��������� ���������
		SYSTEMTIME SystemTime;
		GetLocalTime(&SystemTime);

		char buffer[LOGGER_MESSAGE_SIZE];
		if (component.empty()) //���� ��������� ������
		{
			sprintf(buffer, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s] %s",
				SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,
				CURRENT_THREAD_ID(), message_type_string.c_str(), string.toStdString().c_str());
		}
		else //��������� �� ������
		{
			sprintf(buffer, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s][%s] %s",
				SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,
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
	ISDEBUG_L(QString::fromStdString(string_complete));
#endif
	Array[LastIndex++] = string_complete;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateLogDirectory(const QDate &Date)
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
QString ISLogger::GetPathFile(const QDate &Date) const
{
	return PathLogsDir + QCoreApplication::applicationName() + '_' + Date.toString(FORMAT_DATE_V2) + SYMBOL_POINT + EXTENSION_LOG;
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
				File << Array[i] << std::endl;
				Array[i].clear();
			}
			LastIndex = 0;
		}
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

		QDate CurrentDate = QDate::currentDate();

		//���� �������� ����� ��� ��� - ������ ����������� �����
		if (CurrentMonth != CurrentDate.month() || CurrentYear != CurrentDate.year())
		{
			//�������� ������� ���������� ���������� ���� �� ���������
			while (!CreateLogDirectory(CurrentDate))
			{
				std::cerr << ErrorString.toStdString() << std::endl;
				ISSleep(LOGGER_TIMEOUT);
			}
		}

		//���� �������� ���� - ��������� ������� ���� � ��������� �����
		if (CurrentDay != CurrentDate.day())
		{
			File.close();

			bool is_opened = false;
			while (!is_opened)
			{
				QString path_file = GetPathFile(CurrentDate);
				File.open(path_file.toStdString().c_str(), std::ios::app);
				is_opened = File.is_open();
				if (is_opened) //���� ��� ������� ������ - ���������� ������� ����
				{
					CurrentDay = CurrentDate.day();
				}
				else //���� �� ������� ������� �������� ������� ��� ��� ��� ����� �������
				{
					std::cerr << "Error open file \"" + path_file.toStdString() + "\": " + strerror(errno) << std::endl;
					ISSleep(LOGGER_TIMEOUT);
				}
			}
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
