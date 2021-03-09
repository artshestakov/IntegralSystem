#include "ISAlgorithm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetLastErrorS()
{
	std::string ErrorString = NO_ERROR_STRING;DWORD ErrorID = GetLastError();
	if (ErrorID != 0) //Код ошибки валиден
	{
		LPSTR Buffer = nullptr;
		size_t MessageSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&Buffer, 0, NULL);
		ErrorString = std::string(Buffer, MessageSize - 2);
		LocalFree(Buffer);
	}
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::ConsoleSetEncoding(unsigned int CodePage, std::string &ErrorString)
{
#ifdef WIN32
	if (SetConsoleOutputCP(CodePage) == FALSE) //Не удалось установить кодовую страницу
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}
	return true;
#else
	return true;
#endif
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::DirExist(const std::string &DirPath)
{
	DWORD Attributes = GetFileAttributes(DirPath.c_str());
	return (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::DirCreate(const std::string &DirPath)
{
	std::string ErrorString = NO_ERROR_STRING;
	return DirCreate(DirPath, ErrorString);
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::DirCreate(const std::string &DirPath, std::string &ErrorString)
{
	if (CreateDirectory(DirPath.c_str(), NULL) == FALSE)
	{
		ErrorString = GetLastErrorS();
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::FileExist(const std::string &FilePath)
{
	return PathFileExists(FilePath.c_str()) == TRUE;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetApplicationPath()
{
	std::string Path;
	char Buffer[MAX_PATH] = { 0 };
	if (GetModuleFileName(GetModuleHandle(NULL), Buffer, sizeof(Buffer)) > 0)
	{
		Path = Buffer;
	}
	return Path;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::FileDelete(const std::string &FilePath)
{
	std::string ErrorString;
	return FileDelete(FilePath, ErrorString);
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::FileDelete(const std::string &FilePath, std::string &ErrorString)
{
	if (DeleteFile(FilePath.c_str()) == FALSE)
	{
		ErrorString = GetLastErrorS();
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetApplicationDir()
{
	std::string Path = ISAlgorithm::GetApplicationPath();
	if (!Path.empty())
	{
		size_t Pos = Path.rfind(PATH_SEPARATOR);
		if (Pos != NPOS)
		{
			Path.erase(Pos, Path.size() - Pos);
		}
	}
	return Path;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetApplicationName()
{
	std::string Path = ISAlgorithm::GetApplicationPath();
	if (!Path.empty())
	{
		size_t Pos = Path.rfind(PATH_SEPARATOR);
		if (Pos != NPOS)
		{
			Path.erase(0, Pos);
		}
	}
	return Path;
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::ParseArgs(int argc, char **argv)
{
	ISVectorString VectorString;
	if (argc > 1) //Если аргументов больше одного
	{
		VectorString.resize(argc - 1);

		//Начинаем цикл с единицы, чтобы не брать путь к исполняемому файлу приложения
		for (int i = 1; i < argc; ++i)
		{
			VectorString[i - 1] = argv[i];
		}
	}
	return VectorString;
}
//-----------------------------------------------------------------------------
SYSTEMTIME ISAlgorithm::GetCurrentDate()
{
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	return ST;
}
//-----------------------------------------------------------------------------
