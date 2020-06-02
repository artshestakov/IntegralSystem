#include "ISTcpServerWorker.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
quint16 Port = 0; //Прослушиваемый порт
const char *Login = NULL; //Логин
const char *Password = NULL; //Пароль
//-----------------------------------------------------------------------------
bool ParseArgs(char **argv); //Парсинг аргументов коммандной строки
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	QCoreApplication Application(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, "Server", ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		return EXIT_FAILURE;
	}

    Result = ParseArgs(argv);
	if (!Result)
	{
		return EXIT_FAILURE;
	}

	Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
		CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
		argv[2], argv[3]);
	if (!Result) //Не удалось подключиться к БД
	{
		ISLOGGER_E(ISDatabase::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	ISTcpServerWorker TcpServerWorker;
	Result = TcpServerWorker.Run(static_cast<quint16>(Port));
	if (Result) //Сервер запущен - сообщаем об этом карату
	{
		ISLOGGER_I(QString("Worker is started. Port: %1 Login: %2 Password: %3").arg(Port).arg(Login).arg(Password));
		Result = Application.exec() == EXIT_SUCCESS ? true : false;
	}
	else //Не удалось запустить сервер
	{
		ISLOGGER_E(TcpServerWorker.GetErrorString());
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
bool ParseArgs(char **argv)
{
	//Проверка порта
	if (!argv[1])
	{
		ISLOGGER_E("Port not specified");
		return false;
	}

	Port = std::atoi(argv[1]);
	if (Port == 0)
	{
		ISLOGGER_E("Port invalid value");
		return false;
	}

	//Проверка логина
	Login = argv[2];
	if (!Login)
	{
		ISLOGGER_E("Login not specified");
		return false;
	}

	//Проверка пароля
	Password = argv[3];
	if (!Password)
	{
		ISLOGGER_E("Password not specified");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
