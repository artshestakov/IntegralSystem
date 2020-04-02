#include "ASLogger.h"
#include <windows.h>
//-----------------------------------------------------------------------------
ASLogger::ASLogger()
	: ErrorString("No error."),
	Running(false)
{

}
//-----------------------------------------------------------------------------
ASLogger::~ASLogger()
{

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
bool ASLogger::Initialize(const std::string &Prefix)
{
	char Buffer[MAX_PATH];
	if (!(GetModuleFileNameA(NULL, Buffer, MAX_PATH) > 0)) //Не удалось получить путь к исполняемому файлу
	{
		ErrorString = "Error getting module file name.";
		return false;
	}

	std::string Temp = std::string(Buffer);
	size_t Pos = Temp.rfind('\\');

	//Если префикс файла не указан - получаем его из имени исполняемого файла, иначе - используем тот что указан
	FilePrefix = Prefix.empty() ? FilePrefix = Temp.substr(Pos + 1, Temp.size() - Pos - 5) : Prefix;

	//Получаем путь к директории с логами
	PathDirLogs = Temp.substr(0, Pos + 1) + "Logs\\";

	if (!CreateLogDirectory()) //Ошибка при создании директорий
	{
		return false;
	}

	File.open(PathFile.c_str(), std::ios::app);
	if (!File.is_open())
	{
		ErrorString = "Error open file \"" + PathFile + "\": " + std::string(strerror(errno));
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void ASLogger::Shutdown()
{
	File.close();
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
	SYSTEMTIME ST;
	GetSystemTime(&ST);

	//Создаём папку с текущим годом
	PathDirCurrent = PathDirLogs + std::to_string(ST.wYear) + '\\';
	if (!CheckExistDir(PathDirCurrent)) //Папка не существует - создаём её
	{
		if (CreateDirectoryA(PathDirCurrent.c_str(), NULL) == FALSE) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathDirCurrent + '"';
			return false;
		}
	}

	//Создаём папку с текущим месяцем
	PathDirCurrent += std::to_string(ST.wMonth) + '\\';
	if (!CheckExistDir(PathDirCurrent))
	{
		if (CreateDirectoryA(PathDirCurrent.c_str(), NULL) == FALSE) //Ошибка создания папки
		{
			ErrorString = "Error create directory \"" + PathDirCurrent + '"';
			return false;
		}
	}

	//Формируем путь к текущему лог-файлу
	char Buffer[MAX_PATH];
	sprintf(Buffer, "%s%s_%02d.%02d.%02d.log", PathDirCurrent.c_str(), FilePrefix.c_str(), ST.wDay, ST.wMonth, ST.wYear);
	PathFile = std::string(Buffer);
	return true;
}
//-----------------------------------------------------------------------------
void ASLogger::Worker()
{

}
//-----------------------------------------------------------------------------
bool ASLogger::CheckExistDir(const std::string &PathDir)
{
	DWORD Attributes = GetFileAttributesA(PathDir.c_str());
	return (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
}
//-----------------------------------------------------------------------------
