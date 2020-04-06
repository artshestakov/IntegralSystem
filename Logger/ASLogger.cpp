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
	if (!(GetModuleFileNameA(NULL, buffer, MAX_PATH) > 0)) //Не удалось получить путь к исполняемому файлу
	{
		ErrorString = "Error getting module file name.";
		return false;
	}

	std::string temp = std::string(buffer);
	size_t pos = temp.rfind('\\');

	//Если префикс файла не указан - получаем его из имени исполняемого файла, иначе - используем тот что указан
	FilePrefix = prefix.empty() ? temp.substr(pos + 1, temp.size() - pos - 5) : prefix;

	//Получаем путь к директории с логами
	PathDirLogs = temp.substr(0, pos + 1) + "Logs\\";

	if (!CreateLogDirectory()) //Ошибка при создании директорий
	{
		return false;
	}

	File.open(PathFile.c_str(), std::ios::app);
	if (!File.is_open()) //Не удалось открыть файл
	{
		ErrorString = "Error open file \"" + PathFile + "\": " + std::string(strerror(errno));
		return false;
	}

	IsRunning = true;
	std::thread(&ASLogger::Worker, this).detach();
	return true;
}
//-----------------------------------------------------------------------------
void ASLogger::Shutdown()
{
	//Останавливаем логгер
	EnterCriticalSection(&CriticalSection);
	IsRunning = false;
	LeaveCriticalSection(&CriticalSection);

	//Ждём когда он остановится и закрываем файл
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
	if (type_message == MessageType::MT_Lite) //Если добавляется упрощенное сообщение (без типа, метки времени и т.д.)
	{
		string_complete = string_message;
	}
	else //Стандартное сообщение
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
bool ASLogger::CreateLogDirectory()
{
	//Создаём корневую папку для логов
	if (!CheckExistDir(PathDirLogs)) //Папка не существует - создаём её
	{
		if (CreateDirectoryA(PathDirLogs.c_str(), NULL) == FALSE) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathDirLogs + '"';
			return false;
		}
	}

	//Получаем текущую дату и время
	SYSTEMTIME st;
	GetSystemTime(&st);

	//Создаём папку с текущим годом
	PathDirCurrent = PathDirLogs + std::to_string(st.wYear) + '\\';
	if (!CheckExistDir(PathDirCurrent)) //Папка не существует - создаём её
	{
		if (CreateDirectoryA(PathDirCurrent.c_str(), NULL) == FALSE) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathDirCurrent + '"';
			return false;
		}
	}

	//Создаём папку с текущим месяцем
	PathDirCurrent += std::to_string(st.wMonth) + '\\';
	if (!CheckExistDir(PathDirCurrent))
	{
		if (CreateDirectoryA(PathDirCurrent.c_str(), NULL) == FALSE) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathDirCurrent + '"';
			return false;
		}
	}

	//Формируем путь к текущему лог-файлу
	char buffer[MAX_PATH];
	sprintf(buffer, "%s%s_%02d.%02d.%02d.log", PathDirCurrent.c_str(), FilePrefix.c_str(), st.wDay, st.wMonth, st.wYear);
	PathFile = buffer;
	return true;
}
//-----------------------------------------------------------------------------
bool ASLogger::CheckExistDir(const std::string &path_dir)
{
	DWORD Attributes = GetFileAttributesA(path_dir.c_str());
	return (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
}
//-----------------------------------------------------------------------------
void ASLogger::Worker()
{
	while (IsRunning)
	{
		Sleep(50);
		EnterCriticalSection(&CriticalSection);
		if (LastIndex) //Если в очереди есть сообщения
		{
			for (size_t i = 0; i < LastIndex; ++i)
			{
				File << Array[i] << std::endl;
			}
			LastIndex = 0;
		}
		LeaveCriticalSection(&CriticalSection);
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
