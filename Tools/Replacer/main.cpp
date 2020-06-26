#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
//-----------------------------------------------------------------------------
std::string FilePath; //���� � �����
std::string StringTemplate; //��������� �������� (������)
std::string StringReplace; //���������� ��������
std::string StringContent; //���������� �����
std::fstream File; //����� �����
size_t ReplaceCount = 0; //���������� �����
//-----------------------------------------------------------------------------
void Using(); //����� ���������� � �������
void Replace(std::string &String, const std::string &OldString, const std::string &NewString, size_t &Count); //������ ������
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool Result = argc == 4;
	if (Result) //���������� ���������� ����������
	{
		FilePath = argv[1];
		StringTemplate = argv[2];
		StringReplace = argv[3];

		File.open(FilePath, std::ios::in);
		if (File.is_open()) //���� �������� �������
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
		else //������ �������� �����
		{
			std::cout << "Error open file with read \"" << FilePath.c_str() << "\": " << strerror(errno) << std::endl;
		}
	}
	else //���������� ���������� �� ����������
	{
		std::cout << "Error: invalid arguments!" << std::endl;
		Using();
	}

	//���� ��� ���������� �������� ���� ��������� ������� - ���������� ������ �� �������
	if (Result)
	{
		Replace(StringContent, StringTemplate, StringReplace, ReplaceCount);
	}

	//���� ��� ���������� �������� ���� ��������� ������� � ���������� ����� ������� �� ���� - ���������� ��������� ������� � ����
	if (Result && ReplaceCount)
	{
		File.open(FilePath, std::ios::out);
		if (File.is_open())
		{
			File << StringContent;
			File.close();
		}
		else //������ �������� �����
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
		size_t Pos = String.find(OldString, 0); //���� ���������� ������
		if (Pos != std::string::npos) //����� - ��������
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
