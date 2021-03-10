#include "ISAlgorithm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetClassName(const std::string &FunctionName)
{
	std::string Result(FunctionName);
	size_t Index = 0;

#ifndef WIN32 //���� �������� ������ ��� Linux - ��������� ��� ����
	Index = Result.find('('); //���� ������������� ������
	if (Index != NPOS) //���� ������ ����� - ������� ��� ��� ����� ��
	{
		Result.erase(Index, Result.size() - Index);
	}

    while ((Index = Result.find(' ')) != NPOS)
	{
		Result.erase(0, ++Index);
	}
#endif

	Index = Result.find(':');
	if (Index != NPOS)
	{
		Result.erase(Index, Result.size() - Index);
	}
	return Result;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetLastErrorS()
{
    std::string ErrorString = NO_ERROR_STRING;
#ifdef WIN32
    DWORD ErrorID = GetLastError();
	if (ErrorID != 0) //��� ������ �������
	{
		LPSTR Buffer = nullptr;
		size_t MessageSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&Buffer, 0, NULL);
		ErrorString = std::string(Buffer, MessageSize - 2);
		LocalFree(Buffer);
	}
#else
    ErrorString = strerror(errno);
#endif
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::ConsoleSetEncoding(unsigned int CodePage, std::string &ErrorString)
{
#ifdef WIN32
	if (SetConsoleOutputCP(CodePage) == FALSE) //�� ������� ���������� ������� ��������
	{
		ErrorString = ISAlgorithm::GetLastErrorS();
		return false;
	}
	return true;
#else //���������� ��� Linux �� ����������
    IS_UNUSED(CodePage);
    IS_UNUSED(ErrorString);
	return true;
#endif
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::DirExist(const std::string &DirPath)
{
#ifdef WIN32
	DWORD Attributes = GetFileAttributes(DirPath.c_str());
	return (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat Stat;
    return stat(DirPath.c_str(), &Stat) == 0 && S_ISDIR(Stat.st_mode);
#endif
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
#ifdef WIN32
	if (SHCreateDirectoryExA(NULL, DirPath.c_str(), NULL) != ERROR_SUCCESS)
	{
		ErrorString = GetLastErrorS();
		return false;
	}
#else
    ISVectorString VectorString = StringSplit(DirPath, PATH_SEPARATOR);
    std::string TempPath;
    TempPath.push_back(PATH_SEPARATOR);

    for (const std::string &String : VectorString)
    {
        TempPath += String + PATH_SEPARATOR;
        if (!DirExist(TempPath))
        {
            if (mkdir(TempPath.c_str(), 0777) != 0)
            {
                ErrorString = GetLastErrorS();
                return false;
            }
        }
    }
#endif
	return true;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::FileExist(const std::string &FilePath)
{
#ifdef WIN32
	return PathFileExists(FilePath.c_str()) == TRUE;
#else
    return access(FilePath.c_str(), F_OK) == 0;
#endif
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetApplicationPath()
{
	std::string Path;
    char Buffer[MAX_PATH] = { 0 };
#ifdef WIN32

	if (GetModuleFileName(GetModuleHandle(NULL), Buffer, sizeof(Buffer)) > 0)
	{
		Path = Buffer;
	}
#else
    char Temp[20] = { 0 };
    sprintf(Temp, "/proc/%d/exe", CURRENT_PID());
    size_t Size = readlink(Temp, Buffer, 1024);
    Path = std::string(Buffer, Size);
#endif
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
#ifdef WIN32
	if (DeleteFile(FilePath.c_str()) == FALSE)
	{
		ErrorString = GetLastErrorS();
		return false;
	}
#else
    if (remove(FilePath.c_str()) != 0)
    {
        ErrorString = GetLastErrorS();
        return false;
    }
#endif
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
			Path.erase(0, ++Pos);
		}

#ifdef WIN32 //��� Windows ������� ����������
		Pos = Path.find('.');
		if (Pos != NPOS) //����� � �������� ���� - ������� ����������
		{
			Path.erase(Pos, Path.size() - Pos);
		}
#endif
	}
	return Path;
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::ParseArgs(int argc, char **argv)
{
	ISVectorString VectorString;
	if (argc > 1) //���� ���������� ������ ������
	{
		VectorString.resize(argc - 1);

		//�������� ���� � �������, ����� �� ����� ���� � ������������ ����� ����������
		for (int i = 1; i < argc; ++i)
		{
			VectorString[i - 1] = argv[i];
		}
	}
	return VectorString;
}
//-----------------------------------------------------------------------------
ISDateTime ISAlgorithm::GetCurrentDate()
{
#ifdef WIN32
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	return{ ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds };
#else
	struct timeval TimeValue;
	gettimeofday(&TimeValue, NULL);
	struct tm *ST = localtime(&TimeValue.tv_sec);
    return{ (unsigned short)ST->tm_mday, (unsigned short)(ST->tm_mon + 1), (unsigned short)(ST->tm_year + 1900),
                (unsigned short)ST->tm_hour, (unsigned short)ST->tm_min, (unsigned short)ST->tm_sec, (unsigned short)(TimeValue.tv_usec / 1000) };
#endif
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::StringSplit(const std::string &String, char Separator)
{
    ISVectorString VectorString;
    if (!String.empty()) //���� ������ �� ������ - �����������
    {
        size_t Pos = 0, LastPos = 0;
        while ((Pos = String.find(Separator, LastPos)) != NPOS)
        {
            if (Pos != 0)
            {
                VectorString.emplace_back(String.substr(LastPos, Pos - LastPos));
                LastPos = ++Pos;
            }
            else
            {
                ++LastPos;
            }
        }

        if (Pos == NPOS)
        {
            size_t Size = String.size();
            if (LastPos < Size)
            {
                VectorString.emplace_back(String.substr(LastPos, Size - LastPos));
            }
        }
    }
    return VectorString;
}
//-----------------------------------------------------------------------------
