#include <iostream>
#include <cstdlib>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#endif
//-----------------------------------------------------------------------------
void Usage();
std::string GetDate();
std::string GetTime();
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	if (argc != 4) //Неверно введены аргументы командной строки
	{
		std::cout << "Invalid arguments!" << std::endl;
		Usage();
		return EXIT_FAILURE;
	}

	//Объявляем строковый поток
	std::stringstream StringStream;

	//Устанавливаем переменную среды для пароля
	StringStream << "PGPASSWORD=" << argv[3];
#ifdef WIN32
	_putenv(StringStream.str().c_str());
#else
    putenv((char *)StringStream.str().c_str());
#endif
	StringStream.str(std::string());

	//Формируем имя файла бекапа и скрипта для ролей
	StringStream << argv[1] << argv[2] << '_' << GetDate() << '_' << GetTime() << ".";
	std::string FileName = StringStream.str();
	StringStream.str(std::string());

	//Формируем команду для создания бекапа
	StringStream << "pg_dump";
#ifdef WIN32
	StringStream << ".exe";
#endif
	StringStream << " -h 127.0.0.1 ";
	StringStream << "-p 5432 ";
	StringStream << "-U postgres ";
	StringStream << "-F c ";
	StringStream << "-Z 9 ";
	StringStream << "--inserts ";
	StringStream << "--column-inserts ";
	StringStream << "--role=postgres ";
	StringStream << "-v ";
	StringStream << "-f " << FileName << "dmp";
	StringStream << ' ' << argv[2];
	std::cout << StringStream.str();
	
	//Вызываем команду создания бекапа
	int Result = system(StringStream.str().c_str());
    std::cout << std::endl;
	if (Result) //Если команда выполнилась с ошибкой - выходим
	{
		return Result;
	}

	//Очищаем буфер и формируем команду бекапа ролей
	StringStream.str(std::string());
	StringStream << "pg_dumpall";
#ifdef WIN32
	StringStream << ".exe";
#endif
	StringStream << " -h 127.0.0.1 ";
	StringStream << "-p 5432 ";
	StringStream << "-U postgres ";
	StringStream << "-r ";
	StringStream << "--role=postgres ";
	StringStream << "-v ";
	StringStream << "-f " << FileName << "sql";

	//Вызываем команду бекапа ролей
	Result = system(StringStream.str().c_str());
    std::cout << std::endl;
	return Result;
}
//-----------------------------------------------------------------------------
void Usage()
{
	std::cout << "Utility for creating database backup with pg_dump." << std::endl;
	std::cout << "Usage: BackuperDB";
#ifdef WIN32
	std::cout << ".exe";
#endif
	std::cout << " [PATH_BACKUP_DIR] [DB_NAME] [DB_PASSWORD]" << std::endl;
	std::cout << "Example:" << std::endl;
	std::cout << "BackuperDB";
#ifdef WIN32
	std::cout << ".exe C:\\Backup\\";
#else
	std::cout << " /home/user/";
#endif
	std::cout << " testing_db 12345678" << std::endl;
}
//-----------------------------------------------------------------------------
std::string GetDate()
{
	char Buffer[11];
#ifdef WIN32
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	sprintf(Buffer, "%02d.%02d.%02d\0", ST.wDay, ST.wMonth, ST.wYear);
#else
	time_t Time = time(NULL);
	struct tm TM = *localtime(&Time);
    sprintf(Buffer, "%02d.%02d.%02d", TM.tm_mday, TM.tm_mon + 1, TM.tm_year + 1900);
#endif
	return std::string(Buffer, strlen(Buffer));
}
//-----------------------------------------------------------------------------
std::string GetTime()
{
	char Buffer[6];
#ifdef WIN32
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	sprintf(Buffer, "%02d.%02d\0", ST.wHour, ST.wMinute);
#else
	time_t Time = time(NULL);
	struct tm TM = *localtime(&Time);
    sprintf(Buffer, "%02d:%02d", TM.tm_hour, TM.tm_min);
#endif
	return std::string(Buffer, strlen(Buffer));
}
//-----------------------------------------------------------------------------
