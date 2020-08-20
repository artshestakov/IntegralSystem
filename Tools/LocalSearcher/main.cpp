#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#ifdef WIN32
#include <windows.h>
#endif
//-----------------------------------------------------------------------------
std::vector<std::string> VectorLocalFiles; //����� �����������
std::vector<std::string> VectorDirs; //���������� � �������� �����
std::vector<std::string> VectorLocalKeys; //����� �����������
std::vector<std::string> VectorSourceFiles; //����� � �������� �����
std::map<std::string, std::string> Map; //����� � �������� �����
//-----------------------------------------------------------------------------
void Usage(); //����� ���������� � �������
bool ReadLocalFiles(); //������ ������ �����������
bool ReadLocalFile(const std::string &FilePath); //������ ����� �����������
bool ReadDirs(); //������ ���� ����������
bool ReadDir(const std::string &DirPath); //������ ����������
bool JoinSource(); //����������� ����������� ������ � �������� ����� � ���� ������ �����
void Search(); //����� �� ������������ ������ �����������
void ShowDuplicates(); //������� � ������� ���������
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	if (argc < 2) //���� ��������� �� �������
	{
		std::cout << "ERROR. Not specified arguments." << std::endl;
		Usage();
		return EXIT_FAILURE;
	}

	std::string current_mode;
	for (int i = 1; i < argc; ++i) //������� ��� ��������� (������� � �������, ������ ��� ��� ������� ���� � ������������ �����)
	{
		std::string current_argument = argv[i]; //������� ��������
		if (current_argument.front() == '-') //���� ������� �������� ���������� � ����, �� ������� ��� �������
		{
			current_mode = current_argument;
			continue;
		}

		if (current_mode == "-L") //�������� ������������ ������ �����������
		{
			VectorLocalFiles.push_back(current_argument);
		}
		else if (current_mode == "-D") //������� ����������� ����� � �������� �����
		{
			VectorDirs.push_back(current_argument);
		}
	}

	if (ReadLocalFiles()) //������ ������ ����������� ������ ������� - ��������� ������
	{
		std::sort(VectorLocalKeys.begin(), VectorLocalKeys.end());
	}
	else //��� ������ ������ ����������� �������� ������ - ������� �� ���������
	{
		return EXIT_FAILURE;
	}

	if (!ReadDirs()) //��� ������ ���������� �������� ������ - ������� �� ���������
	{
		return EXIT_FAILURE;
	}

	if (!JoinSource()) //��� ����������� ��������� ���� � ������ ����� �������� ������
	{
		return EXIT_FAILURE;
	}

	Search();
	ShowDuplicates();
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
	for (const std::string &file_path : VectorLocalFiles) //������� ��� ����� �����������
	{
		result = ReadLocalFile(file_path);
		if (!result) //�� ������� ��������� ���� - �������
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
	if (res) //���� ������� ������
	{
		std::string line;
		size_t line_number = 0;
		while (std::getline(file, line)) //������ ���� ���������
		{
			++line_number;
			if (line.empty()) //���� ������� ������ ������ - ��������� � ���������
			{
				continue;
			}

			while (line[0] == ' ') //������� ������� � ������ ������
			{
				line.erase(0, 1);
			}

			if (line.substr(0, 6) == "<Local") //����� �����������
			{
				size_t pos1 = line.find('"', 0); //���� ������ ������ �������
				if (pos1 == std::string::npos) //�� ����� - ��������� � ��������� ������
				{
					std::cout << "WARNING. Invalid string. File: " << FilePath << " Line: " << line_number << std::endl;
					continue;
				}
				
				size_t pos2 = line.find('"', ++pos1); //���� ������ ������ �������
				if (pos2 == std::string::npos) //�� ����� - ��������� � ��������� ������
				{
					std::cout << "WARNING. Invalid string. File: " << FilePath << " Line: " << line_number << std::endl;
					continue;
				}
				VectorLocalKeys.push_back(line.substr(pos1, pos2 - pos1));
			}
		}
		file.close();
	}
	else
	{
		std::cout << "ERROR. error open file " << FilePath << ": " << strerror(errno) << std::endl;
	}
	return res;
}
//-----------------------------------------------------------------------------
bool ReadDirs()
{
	bool result = true;
	for (const std::string &dir_path : VectorDirs) //������� ��� ���������� � �������� �����
	{
		result = ReadDir(dir_path); //�� ������� ��������� ���������� - �������
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
	if (dir_path.substr(dir_path.size() - 2, 2) != "\\*") //���� ��������� ��� ������� ���� �� ����� \* - ���������
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
			size_t pos = file_path.rfind('.'); //���� ��������� ����� � ����, ����� ���������� ���������� �����
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
	bool result = true;
	for (const std::string &source_path : VectorSourceFiles)
	{
		std::ifstream file(source_path);
		result = file.is_open();
		if (result) //���� ������� ������
		{
			std::string line, content;
			while (std::getline(file, line)) //������ ���� ���������
			{
				content += line + '\n';
			}
			file.close();
			Map[source_path] = content;
		}
		else
		{
			std::cout << "ERROR. error open file " << source_path << ": " << strerror(errno) << std::endl;
			break;
		}
	}
	return result;
}
//-----------------------------------------------------------------------------
void Search()
{
	size_t not_found = 0;
	for (const std::string &key_local : VectorLocalKeys) //������� ��� ����� �����������
	{
		std::string key = "LANG(\"" + key_local + "\")";
		size_t founded = 0;
		for (const auto &map_item : Map) //������� ����� ��������� ����
		{
			if (map_item.second.find(key) != std::string::npos)
			{
				++founded;
			}
		}

		if (!founded)
		{
			std::cout << "WARNING. not found local key: " << key_local << std::endl;
			++not_found;
		}
	}
	
	if (not_found)
	{
		std::cout << "Total not found: " << not_found << std::endl;
	}
}
//-----------------------------------------------------------------------------
void ShowDuplicates()
{
	for (size_t i = 0; i < VectorLocalKeys.size(); ++i)
	{
		if (i != VectorLocalKeys.size() - 1)
		{
			if (VectorLocalKeys[i] == VectorLocalKeys[i + 1])
			{
				std::cout << "WARNING: key duplicate: " << VectorLocalKeys[i] << std::endl;
			}
		}
	}
}
//-----------------------------------------------------------------------------
