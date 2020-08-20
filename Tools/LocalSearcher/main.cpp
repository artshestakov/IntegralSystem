#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>
#ifdef WIN32
#include <windows.h>
#endif
//-----------------------------------------------------------------------------
#define RESULT_FILE_NAME "SourceCode.txt"
//-----------------------------------------------------------------------------
std::vector<std::string> VectorLocalFiles; //Файлы локализации
std::vector<std::string> VectorDirs; //Директории с исходным кодом
std::vector<std::string> VectorLocalKeys; //Ключи локализации
std::vector<std::string> VectorSourceFiles; //Файлы с исходным кодом
std::string SourceCode; //Исходный код
//-----------------------------------------------------------------------------
void Usage(); //Вывод инструкции в консоль
bool ReadLocalFiles(); //Чтение файлов локализации
bool ReadLocalFile(const std::string &FilePath); //Чтение файла локализации
bool ReadDirs(); //Чтение всех директорий
bool ReadDir(const std::string &DirPath); //Чтение директории
bool JoinSource(); //Объединение содержимого файлов с исходным кодом в один единый текст
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	if (argc < 2) //Если аргументы не указаны
	{
		std::cout << "ERROR: Not specified arguments." << std::endl;
		Usage();
		return EXIT_FAILURE;
	}

	std::string current_mode;
	for (int i = 1; i < argc; ++i) //Обходим все аргументы (начиная с первого, потому что под нулевым путь к исполняемому файлу)
	{
		std::string current_argument = argv[i]; //Текущий аргумент
		if (current_argument.front() == '-') //Если текущий аргумент начинается с тирэ, то считаем его режимом
		{
			current_mode = current_argument;
			continue;
		}

		if (current_mode == "-L") //Обхходим перечисление файлов локализации
		{
			VectorLocalFiles.push_back(current_argument);
		}
		else if (current_mode == "-D") //Обходим перечисение папок с исходным кодом
		{
			VectorDirs.push_back(current_argument);
		}
	}

	if (!ReadLocalFiles()) //При чтении файлов локализации возникла ошибка - выходим из программы
	{
		return EXIT_FAILURE;
	}

	if (!ReadDirs()) //При чтении директорий возникла ошибка - выходим из программы
	{
		return EXIT_FAILURE;
	}

	if (!JoinSource()) //При объединении исходного кода в единый текст возникла ошибка
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
void Usage()
{
	std::cout << "Utility for search not actual localization." << std::endl;
	std::cout << "Usage: LocalSearcher";
#ifdef WIN32
	std::cout << ".exe";
#endif
	std::cout << " -L [LOCALIZATION_FILE_1] [LOCALIZATION_FILE_2] [LOCALIZATION_FILE_N] -D [DIR_SOURCE_CODE_1] [DIR_SOURCE_CODE_2] [DIR_SOURCE_CODE_N]" << std::endl;
}
//-----------------------------------------------------------------------------
bool ReadLocalFiles()
{
	bool result = true;
	for (const std::string &file_path : VectorLocalFiles) //Обходим все файлы локализации
	{
		result = ReadLocalFile(file_path);
		if (!result) //Не удалось причитать файл - выходим
		{
			break;
		}
	}
	return result;
}
//-----------------------------------------------------------------------------
bool ReadLocalFile(const std::string &FilePath)
{
	std::ifstream file(FilePath);
	bool res = file.is_open();
	if (res) //Файл успешно открыт
	{
		std::string line;
		size_t line_number = 0;
		while (std::getline(file, line)) //Читаем файл построчно
		{
			++line_number;
			if (line.empty()) //Если текущая строка пустая - переходим к следующей
			{
				continue;
			}

			while (line[0] == ' ') //Удаляем пробелы в начале строки
			{
				line.erase(0, 1);
			}

			if (line.substr(0, 6) == "<Local") //Нашли локализацию
			{
				size_t pos1 = line.find('"', 0); //Ищем индекс первой кавычки
				if (pos1 == std::string::npos) //Не нашли - переходим к следующей строке
				{
					std::cout << "WARNING: Invalid string. File: " << FilePath << " Line: " << line_number << std::endl;
					continue;
				}
				
				size_t pos2 = line.find('"', ++pos1); //Идем индекс второй кавычки
				if (pos2 == std::string::npos) //Не нашли - переходим к следующей строке
				{
					std::cout << "WARNING: Invalid string. File: " << FilePath << " Line: " << line_number << std::endl;
					continue;
				}
				VectorLocalKeys.push_back(line.substr(pos1, pos2 - pos1));
			}
		}
		file.close();
	}
	else
	{
		std::cout << "ERROR: error open file " << FilePath << ": " << strerror(errno) << std::endl;
	}
	return res;
}
//-----------------------------------------------------------------------------
bool ReadDirs()
{
	bool result = true;
	for (const std::string &dir_path : VectorDirs) //Обходим все директории с исходным кодом
	{
		result = ReadDir(dir_path); //Не удалось прочитать директорию - выходим
		if (!result)
		{
			break;
		}
	}
	return result;
}
//-----------------------------------------------------------------------------
bool ReadDir(const std::string &DirPath)
{
	std::string dir_path = DirPath;
	if (dir_path.substr(dir_path.size() - 2, 2) != "\\*") //Если последние два символа пути не равны \* - дополняем
	{
		dir_path += "\\*";
	}

	WIN32_FIND_DATA find_data;
	HANDLE handle = FindFirstFile(dir_path.c_str(), &find_data);
	bool result = handle != INVALID_HANDLE_VALUE;
	if (result)
	{
		do
		{
			std::string file_path = DirPath + '\\' + find_data.cFileName;
			size_t pos = file_path.rfind('.'); //Ищем последнюю точку в пути, чтобы определить расширение файла
			if (pos != std::string::npos && (file_path.substr(pos + 1) == "cpp" || file_path.substr(pos + 1) == "h"))
			{
				VectorSourceFiles.push_back(file_path);
			}
		} while (FindNextFile(handle, &find_data));
		FindClose(handle);
	}
	return result;
}
//-----------------------------------------------------------------------------
bool JoinSource()
{
	std::ofstream FileResult(RESULT_FILE_NAME);
	bool result = true;
	for (const std::string &source_path : VectorSourceFiles)
	{
		std::ifstream file(source_path);
		result = file.is_open();
		if (result) //Файл успешно открыт
		{
			std::string line;
			while (std::getline(file, line)) //Читаем файл построчно
			{
				FileResult << line << std::endl;
			}
			file.close();
		}
		else
		{
			std::cout << "ERROR: error open file " << source_path << ": " << strerror(errno) << std::endl;
			break;
		}
	}
	FileResult.close();
	return result;
}
//-----------------------------------------------------------------------------
