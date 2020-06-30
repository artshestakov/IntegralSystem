#include "ISLogger.h"
#include "ISDefinesCore.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
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
	DESTROY_CRITICAL_SECTION(&CriticalSection);
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
        ISSleep(LOGGER_TIMEOUT);
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
		case MessageType::MT_Trace: string_stream << "Trace"; break;
		case MessageType::MT_Info: string_stream << "Info"; break;
		case MessageType::MT_Warning: string_stream << "Warning"; break;
		case MessageType::MT_Error: string_stream << "Error"; break;
        case MessageType::MT_Assert: string_stream << "Assert"; break;
		}
        string_stream << "][" << ISAlgorithm::GetFileNameFromPath(source_name) << ':' << source_line << "] " << string_message.toStdString();
		string_complete = QString::fromStdString(string_stream.str());
	}

#ifdef DEBUG //� ���������� ������ ������� ������ � �������
	OutputToConsole(string_complete);
#else //� �������� ������ ������� � ������� ������ � ������ ���� ������� ������������
	if (ISSystem::IsConfigurator())
	{
		OutputToConsole(string_complete);
	}
#endif

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
    PathLogsDir = ISDefines::Core::PATH_APPLICATION_DIR + "/Logs/" + QString::number(CurrentYear) + '/' + QString("%1").arg(CurrentMonth, 2, 'g', -1, '0') + '/';
	
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
void ISLogger::OutputToConsole(const QString &String)
{
	std::cout << String.toStdString() << std::endl;
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (IsRunning)
	{
		ISSleep(LOGGER_TIMEOUT);
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
