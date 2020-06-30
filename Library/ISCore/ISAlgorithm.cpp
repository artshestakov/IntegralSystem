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
double ISAlgorithm::PrepareDouble(double Double, size_t Precision)
{
	char Char[MAX_PATH];
	sprintf(Char, "%.*f", Precision, Double); //Конвертируем дробное число в строку.
	while (true) //Обрезаем возможные нули в конце
	{
		size_t Index = strlen(Char) - 1; //Индекс последнего символа
		if (Char[Index] == '0') //Если последний символ является нулем - обрезаем его
		{
			Char[Index] = '\0';
		}
		else if (Char[Index] == SYMBOL_POINT) //Если последний символ является точкой - обрезаем его и выходим
		{
			Char[Index] = '\0';
			break;
		}
		else //Иначе выходим из цикла
		{
			break;
		}
	}
	return atof(Char); //Преобразовываем строку обратно в число с плавающей запятой и возвращаем
}
//-----------------------------------------------------------------------------
