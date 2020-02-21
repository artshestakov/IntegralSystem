#include "GLLogger.h"
//-----------------------------------------------------------------------------
GLLogger::GLLogger()
	: ErrorString("No error."),
	Array(NULL),
	LastPosition(0),
	Running(false),
	File(NULL)
{
	
}
//-----------------------------------------------------------------------------
GLLogger::~GLLogger()
{
	while (Running)
	{
		Mutex.lock();
		//bool IsEmpty = Queue.empty();
		Mutex.unlock();
	}
	
	//Running = false;
	//fclose(File);
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
	Array = (char **)malloc(sizeof(char *) * ARRAY_MAX_SIZE + 1);
	Running = Array ? true : false;
	if (Running)
	{
		memset(Array, 0, ARRAY_MAX_SIZE);

		Running = CreateDirs();
		if (Running)
		{
			SYSTEMTIME ST;
			GetLocalTime(&ST);

			char FileName[MAX_PATH];
			sprintf(FileName, "G:\\%02d.log", ST.wDay);

			File = fopen(FileName, "a");
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
	else
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
	}
}
//-----------------------------------------------------------------------------
bool GLLogger::CreateDirs()
{
	return true;
}
//-----------------------------------------------------------------------------
