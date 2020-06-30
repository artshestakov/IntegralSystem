#include "ISAlgorithm.h"
#include "ISConstants.h"
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
double ISAlgorithm::PrepareDouble(double Double, size_t Precision)
{
	char Char[MAX_PATH];
	sprintf(Char, "%.*f", Precision, Double); //������������ ������� ����� � ������.
	while (true) //�������� ��������� ���� � �����
	{
		size_t Index = strlen(Char) - 1; //������ ���������� �������
		if (Char[Index] == '0') //���� ��������� ������ �������� ����� - �������� ���
		{
			Char[Index] = '\0';
		}
		else if (Char[Index] == SYMBOL_POINT) //���� ��������� ������ �������� ������ - �������� ��� � �������
		{
			Char[Index] = '\0';
			break;
		}
		else //����� ������� �� �����
		{
			break;
		}
	}
	return atof(Char); //��������������� ������ ������� � ����� � ��������� ������� � ����������
}
//-----------------------------------------------------------------------------
