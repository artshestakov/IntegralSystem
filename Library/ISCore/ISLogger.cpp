#include "ISLogger.h"
#include "ISDefinesCore.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#define INIT_CRITICAL_SECTION(CRITICAL_SECTION) InitializeCriticalSection(CRITICAL_SECTION)
#define LOCK_CRITICAL_SECTION(CRITICAL_SECTION) EnterCriticalSection(CRITICAL_SECTION)
#define UNLOCK_CRITICAL_SECTION(CRITICAL_SECTION) LeaveCriticalSection(CRITICAL_SECTION)
#define GET_CURRENT_THREAD_ID GetCurrentThreadId
#else
#define INIT_CRITICAL_SECTION(CRITICAL_SECTION) pthread_mutex_init(CRITICAL_SECTION, NULL)
#define LOCK_CRITICAL_SECTION(CRITICAL_SECTION) pthread_mutex_lock(CRITICAL_SECTION)
#define UNLOCK_CRITICAL_SECTION(CRITICAL_SECTION) pthread_mutex_unlock(CRITICAL_SECTION)
#define GET_CURRENT_THREAD_ID pthread_self
#endif
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
	: ErrorString(NO_ERROR_STRING),
	LastIndex(0),
	IsRunning(false),
	IsFinished(false)
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
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLogger::Initialize()
{
    INIT_CRITICAL_SECTION(&CriticalSection);

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
    LOCK_CRITICAL_SECTION(&CriticalSection);
	IsRunning = false;
    UNLOCK_CRITICAL_SECTION(&CriticalSection);

	//��� ����� �� ����������� � ��������� ����
	while (!IsFinished)
	{
        ISSleep(50);
	}
	File.close();
}
//-----------------------------------------------------------------------------
void ISLogger::Log(MessageType type_message, const QString &string_message, const char *source_name, int source_line)
{
	QString string_complete;
	if (type_message == MessageType::MT_Lite || type_message == MessageType::MT_Null) //���� ����������� ���������� ��������� (��� ����, ����� ������� � �.�.)
	{
		string_complete = string_message;
	}
	else //����������� ���������
	{
		std::stringstream string_stream;
        string_stream << QDateTime::currentDateTime().toString(FORMAT_DATE_TIME_V9).toStdString() << SYMBOL_SPACE << GET_CURRENT_THREAD_ID() << " [";

		switch (type_message)
		{
		case MessageType::MT_Null: break;
		case MessageType::MT_Lite: break;
		case MessageType::MT_Debug: string_stream << "Debug"; break;
		case MessageType::MT_Info: string_stream << "Info"; break;
		case MessageType::MT_Warning: string_stream << "Warning"; break;
		case MessageType::MT_Error: string_stream << "Error"; break;
		}
		string_stream << "][" << source_name << ':' << source_line << "] " << string_message.toStdString();
		string_complete = QString::fromStdString(string_stream.str());
	}

	std::cout << string_complete.toStdString() << std::endl;
    LOCK_CRITICAL_SECTION(&CriticalSection);
    Array[LastIndex] = string_complete;
    ++LastIndex;
    UNLOCK_CRITICAL_SECTION(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateLogDirectory(const QDate &Date)
{
	//���������� ������� ����� � ���
	CurrentMonth = Date.month();
	CurrentYear = Date.year();

	//��������� ���� � ������� �����
	PathLogsDir = ISDefines::Core::PATH_APPLICATION_DIR + "/Logs/" + QString::number(CurrentYear) + '/' + QString::number(CurrentMonth) + '/';
	
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
	return PathLogsDir + ISDefines::Core::APPLICATION_NAME + '_' + Date.toString(FORMAT_DATE_V2) + SYMBOL_POINT + EXTENSION_LOG;
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (IsRunning)
	{
		ISSleep(50);
        LOCK_CRITICAL_SECTION(&CriticalSection);
		if (LastIndex) //���� � ������� ���� ���������
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				File << Array[i].toStdString() << std::endl;
				Array[i].clear();
			}
			LastIndex = 0;
		}
        UNLOCK_CRITICAL_SECTION(&CriticalSection);

		QDate CurrentDate = QDate::currentDate();

		//���� �������� ����� ��� ��� - ������ ����������� �����
		if (CurrentMonth != CurrentDate.month() || CurrentYear != CurrentDate.year())
		{
			//�������� ������� ���������� ���������� ���� �� ���������
			while (!CreateLogDirectory(CurrentDate))
			{
				std::cerr << ErrorString.toStdString() << std::endl;
				ISSleep(50);
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
					ISSleep(50);
				}
			}
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
