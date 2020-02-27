#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
	: ErrorString(NO_ERROR_STRING),
	LastPosition(0),
	Running(false),
	CurrentYear(0),
	CurrentMonth(0),
	CurrentDay(0)
{
	
}
//-----------------------------------------------------------------------------
ISLogger::~ISLogger()
{
	//������������� ���������� �������
	Mutex.lock();
	Running = false;
	Mutex.unlock();

	//��� ���� ������� ������� �� ����� �������� � ����
	while (LastPosition)
	{
		Sleep(100);
	}

	File.close(); //��������� ����
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
	if (Running) //���� ������ ��� ��������, ���������� true
	{
		return Running;
	}

	//�������� ���� � ������������ �����
	char Temp[MAX_PATH];
	Running = GetModuleFileNameA(NULL, Temp, MAX_PATH) > 0 ? true : false;
	if (Running) //���� � ������������ ����� ������� �������
	{
		PathDirectory = &Temp[0];
		size_t Pos = PathDirectory.rfind('\\');
		PathDirectory.erase(Pos + 1, PathDirectory.size() - Pos - 1);
	}
	else //�� ������� �������� ����
	{
		ErrorString = "Error getting current module file path.";
		return Running;
	}

	Running = CreateDir();
	if (!Running) //��� �������� ���������� ��������� ������
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
	return Running;
}
//-----------------------------------------------------------------------------
void ISLogger::Log(MessageType Type, const std::string &String, const char *SourceName, int Line)
{
	if (LastPosition == ARRAY_MAX_SIZE && !Running) //���� �������� ���������� ������ ������� ������� ��� ���������� ������� ��� ����������� - ������� �� �������
	{
		return;
	}

	//�������� ������� ���� � �����
	SYSTEMTIME ST;
	GetLocalTime(&ST);

	//��������� ������ ������ ���� (����, �����, ������������� �������� ������)
	char Temp[MAX_PATH];
	sprintf(Temp, "%02d.%02d.%02d %02d:%02d:%02d.%03d %d", ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds, GetCurrentThreadId());
	
	//��������� ��������� ����� (��� ���������, ��� ����� � �������� �����, ����� ������)
	std::stringstream Stream;
	Stream << Temp << " [";
	switch (Type)
	{
	case MessageType::MT_Info: Stream << "INFO"; break;
	case MessageType::MT_Debug: Stream << "DEBUG"; break;
	case MessageType::MT_Warning: Stream << "WARNING"; break;
	case MessageType::MT_Error: Stream << "ERROR"; break;
	}
	Stream << "] [" << SourceName << ":" << Line << "] " << String << std::endl;
	
	Mutex.lock();
	Array[LastPosition++] = Stream.str();
	Mutex.unlock();
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (Running) //������ ������
	{
		DWORD
		Sleep(LOGGER_TIMEOUT); //��� 2 �������

		Mutex.lock(); //��������� �������
		if (LastPosition) //���� ������� �� ������
		{
			for (size_t i = 0; i < LastPosition; ++i) //������� �������
			{
				File << Array[i];
				Array[i].clear();
			}
			File.flush();
			LastPosition = 0;
		}
		Mutex.unlock(); //������������ �������

		//�������� ������� ���� � �����
		SYSTEMTIME ST;
		GetLocalTime(&ST);
		
		//���� �������� ���/�����/���� - ������ ����������� ����������, ��������� ������� ���� � ����� � ������������� ����� ����
		if (CurrentYear != ST.wYear || CurrentMonth != ST.wMonth || CurrentDay != ST.wDay)
		{
			bool Result = CurrentMonth != ST.wMonth;
			if (Result) //���� �������� ����� - ������ ����������� ����������
			{
				Result = CreateDir();
			}
			else //����� �� ������� - �������� ���������� �� ���������
			{
				Result = true;
			}

			if (Result) //����� ��� ������ ������ �������/������� ������� - ��������� ���� � ����� � ������������� ���
			{
				UpdateFilePath();
				File.close();
				File.open(PathFile, std::ios::app); //������������ (File.is_open() == false)?
			}
		}
	}
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateDir()
{
	//�������� ������� ���� � ����� � ��������� ���� � ����� Logs
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	PathLogs = PathDirectory + "Logs\\" + std::to_string(ST.wYear) + '\\' + (ST.wMonth < 10 ? '0' + std::to_string(ST.wMonth) : std::to_string(ST.wMonth)) + '\\';

	DWORD Attributes = GetFileAttributesA(PathLogs.c_str());
	bool Result = (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
	if (!Result) //���� ����� �� ���������� - ������ �
	{
		Result = SHCreateDirectoryExA(NULL, PathLogs.c_str(), NULL) == 0;
		if (!Result) //�� ������� ������� �����
		{
			ErrorString = "Error create dir " + PathLogs;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISLogger::UpdateFilePath()
{
	//�������� ������� ���� � �����
	SYSTEMTIME ST;
	GetLocalTime(&ST);

	//���������� ���, ����� � ����
	CurrentYear = ST.wYear;
	CurrentMonth = ST.wMonth;
	CurrentDay = ST.wDay;

	//��������� ���� � ���-�����
	std::stringstream Stream;
	Stream << PathLogs << "AuthServer_" << CurrentYear <<
		(CurrentMonth < 10 ? '0' + std::to_string(CurrentMonth) : std::to_string(CurrentMonth)) <<
		(CurrentDay < 10 ? '0' + std::to_string(CurrentDay) : std::to_string(CurrentDay)) <<
		".log";
	PathFile = Stream.str();
}
//-----------------------------------------------------------------------------
