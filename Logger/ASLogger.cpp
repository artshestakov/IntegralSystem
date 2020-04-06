#include "ASLogger.h"
//-----------------------------------------------------------------------------
ASLogger::ASLogger()
	: ErrorString("No error."),
	LastIndex(0),
	IsRunning(false),
	IsFinished(false)
{

}
//-----------------------------------------------------------------------------
ASLogger::~ASLogger()
{
	Shutdown();
}
//-----------------------------------------------------------------------------
ASLogger& ASLogger::Instance()
{
	static ASLogger Logger;
	return Logger;
}
//-----------------------------------------------------------------------------
std::string ASLogger::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ASLogger::Initialize(const std::string &prefix)
{
	InitializeCriticalSection(&CriticalSection);

	char buffer[MAX_PATH];
	if (!(GetModuleFileNameA(NULL, buffer, MAX_PATH) > 0)) //�� ������� �������� ���� � ������������ �����
	{
		ErrorString = "Error getting module file name.";
		return false;
	}

	std::string temp = std::string(buffer);
	size_t pos = temp.rfind('\\');

	//���� ������� ����� �� ������ - �������� ��� �� ����� ������������ �����, ����� - ���������� ��� ��� ������
	FilePrefix = prefix.empty() ? temp.substr(pos + 1, temp.size() - pos - 5) : prefix;

	//�������� ���� � ����� ����������
	PathCurrentDirectory = temp.substr(0, pos + 1);

	//�������� ������� ���� � �����
	SYSTEMTIME st;
	GetSystemTime(&st);

	if (!CreateLogDirectory(st)) //������ ��� �������� ����������
	{
		return false;
	}

	std::string path_file = GetPathFile(st);
	File.open(path_file.c_str(), std::ios::app);
	if (!File.is_open()) //�� ������� ������� ����
	{
		ErrorString = "Error open file \"" + path_file + "\": " + std::string(strerror(errno));
		return false;
	}

	IsRunning = true;
	std::thread(&ASLogger::Worker, this).detach();
	return true;
}
//-----------------------------------------------------------------------------
void ASLogger::Shutdown()
{
	//������������� ������
	EnterCriticalSection(&CriticalSection);
	IsRunning = false;
	LeaveCriticalSection(&CriticalSection);

	//��� ����� �� ����������� � ��������� ����
	while (!IsFinished)
	{
		Sleep(50);
	}
	File.close();
}
//-----------------------------------------------------------------------------
void ASLogger::Log(MessageType type_message, const std::string &string_message, const char *source_name, int source_line)
{
	std::string string_complete;
	if (type_message == MessageType::MT_Lite) //���� ����������� ���������� ��������� (��� ����, ����� ������� � �.�.)
	{
		string_complete = string_message;
	}
	else //����������� ���������
	{
		SYSTEMTIME st;
		GetSystemTime(&st);

		char buffer[MAX_PATH];
		sprintf(buffer, "%02d.%02d.%d %02d:%02d:%02d.%03d %d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentThreadId());

		std::stringstream string_stream;
		string_stream << buffer << " [";
		switch (type_message)
		{
		case MessageType::MT_Debug: string_stream << "Debug"; break;
		case MessageType::MT_Info: string_stream << "Debug"; break;
		case MessageType::MT_Warning: string_stream << "Debug"; break;
		case MessageType::MT_Error: string_stream << "Debug"; break;
		}
		string_stream << "][" << source_name << ':' << source_line << "] " << string_message;
		string_complete = string_stream.str();
	}

	EnterCriticalSection(&CriticalSection);
	Array[LastIndex] = string_complete;
	++LastIndex;
	LeaveCriticalSection(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ASLogger::CreateLogDirectory(const SYSTEMTIME &system_time)
{
	//���������� ������� ����
	CurrentDay = system_time.wDay;
	CurrentMonth = system_time.wMonth;
	CurrentYear = system_time.wYear;

	//��������� ���� � ������� �����
	PathDirCurrent = PathCurrentDirectory + "Logs\\" + std::to_string(CurrentYear) + '\\' + std::to_string(CurrentMonth) + '\\';

	if (PathFileExistsA(PathDirCurrent.c_str()) == FALSE) //���� ����� � ������� ������� �� ���������� - ������ �
	{
		if (SHCreateDirectoryExA(NULL, PathDirCurrent.c_str(), NULL) != ERROR_SUCCESS) //������ �������� �����
		{
			ErrorString = "Error create directory \"" + PathDirCurrent + '"';
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
std::string ASLogger::GetPathFile(const SYSTEMTIME &system_time) const
{
	char buffer[MAX_PATH];
	sprintf(buffer, "%s%s_%02d.%02d.%02d.log", PathDirCurrent.c_str(), FilePrefix.c_str(), system_time.wDay, system_time.wMonth, system_time.wYear);
	return buffer;
}
//-----------------------------------------------------------------------------
void ASLogger::Worker()
{
	while (IsRunning)
	{
		Sleep(50);
		EnterCriticalSection(&CriticalSection);
		if (LastIndex) //���� � ������� ���� ���������
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				File << Array[i] << std::endl;
			}
			LastIndex = 0;
		}
		LeaveCriticalSection(&CriticalSection);

		SYSTEMTIME st;
		GetSystemTime(&st);

		//���� �������� ���� - ��������� ������� ���� � ��������� �����
		if (CurrentDay != st.wDay)
		{
			File.close();

			bool is_opened = false;
			while (!is_opened)
			{
				std::string path_file = GetPathFile(st);
				File.open(path_file.c_str(), std::ios::app);
				is_opened = File.is_open();
				if (!is_opened) //���� ���� �� ������� ������� �������� ������� ��� ��� ��� ����� �������
				{
					std::cerr << "Error open file \"" + path_file + "\": " + std::string(strerror(errno)) << std::endl;
					Sleep(1000);
				}
			}
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
