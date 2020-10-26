#include "ISCaratApplication.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	QObject::connect(&CaratApplication, &ISCaratApplication::Quit, &CaratApplication, &ISCaratApplication::quit, Qt::QueuedConnection);
	if (!CaratApplication.Init()) //При инициализации сервера произошла ошибка - выходим
	{
		return EXIT_FAILURE;
	}

	//Получаем аргументы запуска и удаляем первый (путь к исполняемому файлу)
	QStringList Arguments = CaratApplication.arguments();
	Arguments.removeFirst();

	if (!Arguments.isEmpty()) //Если аргументы запуска есть
	{
		return CaratApplication.Run(Arguments);
	}

	//Аргуменов запуска не было, запускаемся в режиме сервера
	if (!CaratApplication.Run())
	{
		return EXIT_FAILURE;
	}
	return CaratApplication.exec();
}
//-----------------------------------------------------------------------------
