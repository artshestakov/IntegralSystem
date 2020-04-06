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

	//Получаем путь к папке приложения
	PathApplicationDir = temp.substr(0, pos + 1);

	//Получаем текущую дату и время
	SYSTEMTIME st;
	GetSystemTime(&st);

	//Запоминаем текущий день
	CurrentDay = st.wDay;

	if (!CreateLogDirectory(st)) //Ошибка при создании директорий
	{
		return false;
	}

	std::string path_file = GetPathFile(st);
	File.open(path_file.c_str(), std::ios::app);
	if (!File.is_open()) //Не удалось открыть файл
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
bool ASLogger::CreateLogDirectory(const SYSTEMTIME &st)
{
	//Запоминаем текущий месяц и год
	CurrentMonth = st.wMonth;
	CurrentYear = st.wYear;

	//Формируем путь к текущей папке
	PathLogsDir = PathApplicationDir + "Logs\\" + std::to_string(CurrentYear) + '\\' + std::to_string(CurrentMonth) + '\\';

	if (PathFileExistsA(PathLogsDir.c_str()) == FALSE) //Если папка с текущим месяцем не существует - создаём её
	{
		if (SHCreateDirectoryExA(NULL, PathLogsDir.c_str(), NULL) != ERROR_SUCCESS) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathLogsDir + '"';
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
std::string ASLogger::GetPathFile(const SYSTEMTIME &st) const
{
	char buffer[MAX_PATH];
	sprintf(buffer, "%s%s_%02d.%02d.%02d.log", PathLogsDir.c_str(), FilePrefix.c_str(), st.wDay, st.wMonth, st.wYear);
	return buffer;
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

		SYSTEMTIME st;
		GetSystemTime(&st);

		//Если сменился месяц или год - создаём недостающие папки
		if (CurrentMonth != st.wMonth || CurrentYear != st.wYear)
		{
			//Пытаемся создать недосающие директории пока не получится
			while (!CreateLogDirectory(st))
			{
				std::cerr << ErrorString << std::endl;
				Sleep(1000);
			}
		}

		//Если сменился день - закрываем текущий файл и открываем новый
		if (CurrentDay != st.wDay)
		{
			File.close();

			bool is_opened = false;
			while (!is_opened)
			{
				std::string path_file = GetPathFile(st);
				File.open(path_file.c_str(), std::ios::app);
				is_opened = File.is_open();
				if (is_opened) //Файл был успешно открыт - запоминаем текущий день
				{
					CurrentDay = st.wDay;
				}
				else //Файл не удалось открыть пытаемся сделать жто ещё раз через секунду
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
