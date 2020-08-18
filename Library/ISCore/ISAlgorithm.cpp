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
std::string ISAlgorithm::PrepareDouble(double Double, size_t Precision)
{
	char Char[DBL_DECIMAL_DIG];
	sprintf(Char, "%.*f", (int)Precision, Double); //Конвертируем дробное число в строку.
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
	//return atof(Char); //Преобразовываем строку обратно в число с плавающей запятой и возвращаем
	return std::string((const char *)Char);
}
//-----------------------------------------------------------------------------
void ISAlgorithm::PrepareStringDouble(QString &String, size_t Precision)
{
	int Index = 0;
	while (!String.isEmpty())
	{
		QChar Char = String[Index]; //Получаем текущий символ

		//Если текущий символ не цифра, не точка и не запятая - удаляем этот символ
		if (Char.isDigit() || Char == SYMBOL_POINT || Char == SYMBOL_COMMA)
		{
			//continue;
		}
		else if (Char == '\0') //Конец строки - можно выходить из цикла
		{
			break;
		}
		else //Попался символ отличный от цифры, точки или запятой - удаляем его и переходим на начало итерации
		{
			String.remove(Index, 1);
			//Index = 0;
			continue;
		}
		++Index;
	}

	//Заменяем все запятые на точки
	String.replace(SYMBOL_COMMA, SYMBOL_POINT);

	//Если количество запятых больше одной - удаляем все оставшиеся
	while (String.count(SYMBOL_POINT) > 1)
	{
		int Pos = String.indexOf(SYMBOL_POINT);
		String.remove(String.indexOf(SYMBOL_POINT, Pos + 1), 1);
	}

	//Обрезаем количество цифр после точки
	int Pos = String.indexOf(SYMBOL_POINT);
	if (Pos != -1) //Нашли точку
	{
		Pos += 1 + Precision;
		if (Pos < String.size())
		{
			String.remove(Pos, String.size() - Pos);
		}
	}
}
//-----------------------------------------------------------------------------
