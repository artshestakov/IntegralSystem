#include "ISUSBSearcher.h"
#include <iostream>
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	//������ ��������� �� ������ �����������
	system("chcp 1251");

	//�������� ���������� �� �����������
	std::string ErrorString;
	bool Result = FindDevice(ErrorString);
	if (!Result) //������ ��������� - ���������
	{
		std::cout << "Error: " << ErrorString << std::endl;
	}
	system("PAUSE"); //�������� ����� �������
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
