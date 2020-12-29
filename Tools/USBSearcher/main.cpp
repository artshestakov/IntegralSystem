#include "ISUSBSearcher.h"
#include <iostream>
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	//Меняем кодировку на случай абракадабры
	system("chcp 1251");

	//Получаем информацию об устройствах
	std::string ErrorString;
	bool Result = FindDevice(ErrorString);
	if (!Result) //Ошибка получения - рапортуем
	{
		std::cout << "Error: " << ErrorString << std::endl;
	}
	system("PAUSE"); //Тормознём перед выходом
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
