#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetFileNameFromPath(const std::string &FilePath)
{
	size_t Pos = FilePath.rfind(PATH_SEPARATOR);
	if (Pos != std::string::npos)
	{
		return std::string(FilePath).erase(0, Pos + 1);
	}
	return FilePath;
}
//-----------------------------------------------------------------------------
qint64 ISAlgorithm::GetFileSize(const std::string &FilePath)
{
	std::string ErrorString;
	return ISAlgorithm::GetFileSize(FilePath, ErrorString);
}
//-----------------------------------------------------------------------------
qint64 ISAlgorithm::GetFileSize(const std::string &FilePath, std::string &ErrorString)
{
	FILE *File = fopen(FilePath.c_str(), "r");
	if (File)
	{
		fseek(File, 0L, SEEK_END);
        qint64 FileSize = ftell(File);
		fclose(File);
		return FileSize;
	}
	else
	{
		ErrorString = strerror(errno);
	}
	return -1;
}
//-----------------------------------------------------------------------------
ISTimePoint ISAlgorithm::GetTick()
{
	return std::chrono::steady_clock::now();
}
//-----------------------------------------------------------------------------
long long ISAlgorithm::GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T2).count();
}
//-----------------------------------------------------------------------------
QString ISAlgorithm::GetClassName(const char *FunctionName)
{
    QString Result(FunctionName);
    int Index = 0;

#ifndef WIN32 //Если работаем сейчас под Linux - исключаем имя типа
    Index = Result.indexOf(SYMBOL_SPACE);
    if (Index != -1)
    {
        Result.remove(0, ++Index);
    }
#endif

    Index = Result.indexOf(':');
    if (Index != -1)
    {
        Result.chop(Result.size() - Index);
    }
	return Result;
}
//-----------------------------------------------------------------------------
void ISAlgorithm::RemoveBeginSymbolLoop(QString &String, char Char)
{
	if (!String.isEmpty())
	{
		while (String[0] == Char)
		{
			String.remove(0, 1);
		}
	}
}
//-----------------------------------------------------------------------------
void ISAlgorithm::RemoveLastSymbolLoop(QString &String, char Char)
{
	if (!String.isEmpty())
	{
		while (String.back() == Char)
		{
			String.chop(1);
		}
	}
}
//-----------------------------------------------------------------------------
