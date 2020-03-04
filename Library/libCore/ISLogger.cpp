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
	if (Running) //���� ������ ��� ��������, ���������� true
	{
		return Running;
	}

	EnableOutPrintf = OutPrintf;
	EnableOutFile = OutFile;

	//���� ������� �� ����� - ���������� ������� �� ���������, ����� - ��� ��� ��� �����
	FilePrefix = file_prefix.empty() ? LOGGER_NAME_DEFAULT : file_prefix;

	if (EnableOutFile) //���� ������� ����� � ���� - ��������� ��������������� ����������
	{
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
	if (LastPosition == LOGGER_ARRAY_SIZE || !Running) //���� �������� ���������� ������ ������� ������� ��� ���������� ������� ��� ����������� - ������� �� �������
	{
		return;
	}

	//�������� ������� ���� � �����
	SYSTEMTIME ST;
	GetLocalTime(&ST);

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
		//��������� ������ ������ ���� (����, �����, ������������� �������� ������)
		char Temp[MAX_PATH];
		itoa(ST.wYear, Year, 10); //�������������� ���� � ������
		sprintf(Temp, "%02d.%02d.%c%c %02d:%02d:%02d.%03d %d", ST.wDay, ST.wMonth, Year[2], Year[3], ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds, GetCurrentThreadId());

		//��������� ��������� ����� (��� ���������, ��� ����� � �������� �����, ����� ������)
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

		if (EnableOutPrintf) //���� �������� ����� ������ � ������� - �������
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

		if (EnableOutFile) //���� �������� ����� ������ � ���� - ��������� ������ ����� � �������� �����, ������� ������ � ��������� � �������
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
	if (!Running) //������� ���������� �� ���������� ������
	{
		return;
	}

	if (EnableOutFile) //���� ������� ����� � ����
	{
		//������������� ���������� �������
		Mutex.lock();
		Running = false;
		Mutex.unlock();

		//��� ���� ������ �� �����������
		while (!Finished)
		{
			Sleep(100);
		}
		File.close(); //��������� ����
	}
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
	while (true) //������ ������
	{
		Mutex.lock(); //��������� �������
		bool running = Running;
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

		if (!running)
		{
			break;
		}
		Sleep(LOGGER_TIMEOUT); //��� 2 �������
	}
	Finished = true;
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
	Stream << PathLogs << FilePrefix << "_" << CurrentYear <<
		(CurrentMonth < 10 ? '0' + std::to_string(CurrentMonth) : std::to_string(CurrentMonth)) <<
		(CurrentDay < 10 ? '0' + std::to_string(CurrentDay) : std::to_string(CurrentDay)) <<
		SYMBOL_POINT << EXTENSION_LOG;
	PathFile = Stream.str();
}
//-----------------------------------------------------------------------------
