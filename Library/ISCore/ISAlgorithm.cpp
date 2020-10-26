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
	QString Result;

	//Получаем размер строки и обходим её
	size_t StringSize = strlen(FunctionName);
	for (size_t i = 0; i < StringSize; ++i)
	{
		if (FunctionName[i] == ':') //Если попался символ двоеточия - вытаскиваем подстроку
		{
			Result = QString::fromLatin1(FunctionName, (int)i);
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
