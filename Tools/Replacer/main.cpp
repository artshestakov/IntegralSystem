#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
//-----------------------------------------------------------------------------
std::string FilePath; //Путь к файлу
std::string StringTemplate; //Поисковое значение (шаблон)
std::string StringReplace; //Заменяемое значение
std::string StringContent; //Содержимое файла
std::fstream File; //Поток файла
size_t ReplaceCount = 0; //Количество замен
//-----------------------------------------------------------------------------
void Using(); //Вывод инструкции в консоль
void Replace(std::string &String, const std::string &OldString, const std::string &NewString, size_t &Count); //Замена строки
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool Result = argc == 4;
	if (Result) //Количество аргументов правильное
	{
		FilePath = argv[1];
		StringTemplate = argv[2];
		StringReplace = argv[3];

		File.open(FilePath, std::ios::in);
		if (File.is_open()) //Файл открылся успешно
		{
			std::stringstream StringStream;
			std::string Line;
			while (std::getline(File, Line))
			{
				StringStream << Line << std::endl;
			}
			StringContent = StringStream.str();
			File.close();
		}
		else //Ошибка открытия файла
		{
			std::cout << "Error open file with read \"" << FilePath.c_str() << "\": " << strerror(errno) << std::endl;
		}
	}
	else //Количество аргументов не правильное
	{
		std::cout << "Error: invalid arguments!" << std::endl;
		Using();
	}

	//Если все предыдущие операции были выполнены успешно - производим замену по шаблону
	if (Result)
	{
		Replace(StringContent, StringTemplate, StringReplace, ReplaceCount);
	}

	//Если все предыдущие операции были выполнены успешно и количество замен отлично от нуля - записываем результат обратно в файл
	if (Result && ReplaceCount)
	{
		File.open(FilePath, std::ios::out);
		if (File.is_open())
		{
			File << StringContent;
			File.close();
		}
		else //Ошибка открытия файла
		{
			std::cout << "Error open file with write \"" << FilePath.c_str() << "\": " << strerror(errno) << std::endl;
		}
	}

	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
void Using()
{
	std::cout << "Utility for noting text in a file by template." << std::endl;
	std::cout << "Usage: Replacer";
#ifdef WIN32
	std::cout << ".exe";
#endif
	std::cout << " [FILE_PATH] [STRING_TEMPLATE] [STRING_REPLACE]" << std::endl;
}
//-----------------------------------------------------------------------------
void Replace(std::string &String, const std::string &OldString, const std::string &NewString, size_t &Count)
{
	ReplaceCount = 0;
	while (true)
	{
		size_t Pos = String.find(OldString, 0); //Ищём заменяемую строку
		if (Pos != std::string::npos) //Нашли - заменяем
		{
			String.replace(Pos, OldString.size(), NewString);
			++Count;
		}
		else
		{
			break;
		}
	}
}
//-----------------------------------------------------------------------------
