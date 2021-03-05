#include "ISAlgorithm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetLastErrorString()
{
	std::string ErrorString;
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::ConsoleSetEncoding(unsigned int CodePage, std::string &ErrorString)
{
#ifdef WIN32
	if (SetConsoleOutputCP(CodePage) == FALSE) //Ќе удалось установить кодовую страницу
	{
		ErrorString = ISAlgorithm::GetLastErrorString();
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
		ErrorString = GetLastErrorString();
		return false;
	}
	return true;
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
