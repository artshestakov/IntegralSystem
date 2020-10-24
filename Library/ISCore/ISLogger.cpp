#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISSystem.h"
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
void ISLogger::Log(MessageType message_type, const QString &component, const QString &string)
{
	QString string_complete = QDateTime::currentDateTime().toString(FORMAT_DATE_TIME_V9) + SYMBOL_SPACE + QString::number(GET_CURRENT_THREAD_ID());
	switch (message_type)
	{
	case MessageType::MT_Null: string_complete.clear(); break;
	case MessageType::MT_Lite: string_complete.clear(); break;
	case MessageType::MT_Debug: string_complete += " [Debug]"; break;
	case MessageType::MT_Info: string_complete += " [Info]"; break;
	case MessageType::MT_Warning: string_complete += " [Warning]"; break;
	case MessageType::MT_Error: string_complete += " [Error]"; break;
	case MessageType::MT_Trace: string_complete += " [Trace]"; break;
	case MessageType::MT_Assert: string_complete += " [Assert]"; break;
	}
	if (!component.isEmpty())
	{
		string_complete += '[' + component + ']';
	}
	string_complete += ' ' + string;

#ifdef DEBUG //� ���������� ������ ������� ������ � �������
	OutputToConsole(string_complete);
#else //� �������� ������ ������� � ������� ������ � ������ ���� ������� ������������
	if (ISSystem::IsConfigurator())
	{
		OutputToConsole(string_complete);
	}
#endif
	CRITICAL_SECTION_LOCK(&CriticalSection);
	Array[LastIndex] = string_complete;
	++LastIndex;
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
        CRITICAL_SECTION_LOCK(&CriticalSection);
		if (LastIndex) //���� � ������� ���� ���������
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				File << Array[i].toStdString() << std::endl;
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
