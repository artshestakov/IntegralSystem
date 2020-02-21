#include "GLLogger.h"
//-----------------------------------------------------------------------------
GLLogger::GLLogger()
	: ErrorString("No error."),
	Running(false),
	File(NULL)
{
	List.resize(1000);
}
//-----------------------------------------------------------------------------
GLLogger::~GLLogger()
{
	while (Running)
	{
		Mutex.lock();
		bool IsEmpty = Queue.empty();
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
	return Running;
}
//-----------------------------------------------------------------------------
void GLLogger::Append(MessageType Type, const std::string &String, const char *SourceName, int Line)
{
	SYSTEMTIME ST;
	GetLocalTime(&ST);

	char Char[MAX_PATH];
	sprintf(Char, "%02d.%02d.%02d %02d:%02d:%02d.%03d", ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds);

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
	StringStream << "] [" << SourceName << "(" << Line << ")] " << String;
	
	Mutex.lock();
	Queue.push(StringStream.str());
	Mutex.unlock();
}
//-----------------------------------------------------------------------------
void GLLogger::Worker()
{
	while (Running) //Работа потока
	{
		std::this_thread::sleep_for(std::chrono::seconds(LOGGER_TIMEOUT));
		bool Writed = false; //Флаг записи

		Mutex.lock(); //Блокируем мьютекс
		while (!Queue.empty()) //Если очередь не пустая
		{
			std::string String = Queue.front() + "\n";
			Queue.pop();

			fwrite(String.c_str(), sizeof(char), String.size(), File);
			Writed = true;
		}

		if (Writed) //Если на текущей итерации была произведена запись в файл - flush'им данные
		{
			fflush(File);
		}
		
		Mutex.unlock(); //Разблокируем мьютекс
	}
}
//-----------------------------------------------------------------------------
bool GLLogger::CreateDirs()
{
	return true;
}
//-----------------------------------------------------------------------------
