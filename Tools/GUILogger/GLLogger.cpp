#include "GLLogger.h"
//-----------------------------------------------------------------------------
GLLogger::GLLogger()
	: ErrorString("No error."),
	Array(NULL),
	LastPosition(0),
	Running(false),
	PathDirectory(NULL),
	PathFile(NULL),
	File(NULL),
	CurrentYear(0),
	CurrentMonth(0),
	CurrentDay(0)
{
	
}
//-----------------------------------------------------------------------------
GLLogger::~GLLogger()
{

}
//-----------------------------------------------------------------------------
GLLogger& GLLogger::Instance()
{
	static GLLogger Logger;
	return Logger;
}
//-----------------------------------------------------------------------------
std::string GLLogger::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool GLLogger::Initialize()
{
	PathDirectory = (char *)malloc(MAX_PATH + 1);
	Running = PathDirectory ? true : false;
	if (!Running) //������ ��������� ������
	{
		ErrorString = "Error malloc path directory.";
		return Running;
	}

	//�������� ���� � ������������ �����
	Running = GetModuleFileNameA(NULL, PathDirectory, MAX_PATH) > 0 ? true : false;
	if (!Running) //�� ������� �������� ����
	{
		ErrorString = "Not getting current module file path.";
		return Running;
	}

	//�������� ��� �� ������������ �����. �������, ��� ���� � ������� ���������� � ��� ����.
	*(strrchr(PathDirectory, '\\') + 1) = 0;

	Running = CreateDirs();

	Array = (char **)malloc(sizeof(char *) * ARRAY_MAX_SIZE + 1);
	Running = Array ? true : false;
	if (Running) //���� ������ �������� �������
	{
		memset(Array, 0, ARRAY_MAX_SIZE);

		Running = CreateDirs();
		if (Running)
		{
			UpdateFilePath();
			File = fopen(PathFile, "a");
			Running = File ? true : false;
			if (Running)
			{
				std::thread(&GLLogger::Worker, this).detach();
			}
			else
			{
				ErrorString = strerror(errno);
			}
		}
	}
	else //������ ��������� ������
	{
		ErrorString = "Error malloc";
	}
	return Running;
}
//-----------------------------------------------------------------------------
void GLLogger::Append(MessageType Type, const std::string &String, const char *SourceName, int Line)
{
	//�������� ������� ���� � �����
	SYSTEMTIME ST;
	GetLocalTime(&ST);

	//��������� ������ ������ ���� (����, �����, ������������� �������� ������)
	char Char[MAX_PATH];
	sprintf(Char, "%02d.%02d.%02d %02d:%02d:%02d.%03d %d", ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds, GetCurrentThreadId());

	//��������� ��������� ����� (��� ���������, ��� ����� � �������� �����, ����� ������)
	std::stringstream StringStream;
	StringStream << Char << " [";
	switch (Type)
	{
	case MessageType::MT_Unknown: StringStream << "UNKNOWN"; break;
	case MessageType::MT_Info: StringStream << "INFO"; break;
	case MessageType::MT_Debug: StringStream << "DEBUG"; break;
	case MessageType::MT_Warning: StringStream << "WARNING"; break;
	case MessageType::MT_Error: StringStream << "ERROR"; break;
	}
	StringStream << "] [" << SourceName << ":" << Line << "] " << String << std::endl;
	std::string Message = StringStream.str();
	
	Mutex.lock(); //���������
	Array[LastPosition] = (char *)malloc(sizeof(char) * Message.size() + 1);
	if (Array[LastPosition]) //���� ��������� ������ ������ ������� - ���������� ��������� � ������
	{
		strcpy(Array[LastPosition++], Message.c_str());
	}
	Mutex.unlock(); //������������
}
//-----------------------------------------------------------------------------
void GLLogger::Worker()
{
	while (Running) //������ ������
	{
		std::this_thread::sleep_for(std::chrono::seconds(LOGGER_TIMEOUT)); //��� 2 �������
		Mutex.lock(); //��������� �������
		while (LastPosition) //���� ������� �� ������
		{
			for (size_t i = 0; i < LastPosition; ++i)
			{
				size_t StringSize = strlen(Array[i]);
				fwrite(Array[i], sizeof(char), StringSize, File);
				free(Array[i]);
				memset(Array[i], 0, StringSize);
			}
			LastPosition = 0;
			fflush(File);
		}
		Mutex.unlock(); //������������ �������

		//�������� ������� ���� � �����
		SYSTEMTIME ST;
		GetLocalTime(&ST);
		
		if (CurrentYear != ST.wYear || CurrentMonth != ST.wMonth || CurrentDay != ST.wDay)
		{

		}
	}
}
//-----------------------------------------------------------------------------
bool GLLogger::CreateDirs()
{
	//��������� ���� � ����� Logs
	char PathLogs[MAX_PATH];
	strcpy(PathLogs, PathDirectory);
	strcat(PathLogs, "Logs\\");

	bool Result = CreateDirectoryA(PathLogs, NULL) == TRUE ? true : false;
	if (Result) //���� ����� ������� �������
	{
		char PathYear[MAX_PATH];
		strcpy(PathYear, PathDirectory);
		strcat(PathYear, "");
	}
	else
	{

	}
	return Result;
}
//-----------------------------------------------------------------------------
bool GLLogger::UpdateFilePath()
{
	if (PathFile)
	{
		free(PathFile);
	}

	PathFile = (char *)malloc(sizeof(char) * MAX_PATH + 1);
	bool Result = PathFile ? true : false;
	if (Result)
	{
		



		//�������� ������� ���� � �����
		SYSTEMTIME ST;
		GetLocalTime(&ST);

		//����������
		CurrentYear = ST.wYear;
		CurrentMonth = ST.wMonth;
		CurrentDay = ST.wDay;

		//��������� ����� "���"
		char YearString[6];
		itoa(CurrentYear, YearString, 10);
		YearString[4] = '\\';
		YearString[5] = '\0';
		strcat(PathFile, YearString);


	}
	return Result;
}
//-----------------------------------------------------------------------------
