#include "ISCaratApplication.h"
#include "ISConstants.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "ISVersionInfo.h"
#include "ISDebug.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int &argc, char **argv)
	: QCoreApplication(argc, argv),
	ErrorString(NO_ERROR_STRING)
{
	
}
//-----------------------------------------------------------------------------
ISCaratApplication::~ISCaratApplication()
{

}
//-----------------------------------------------------------------------------
QString ISCaratApplication::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Init()
{
	if (!ISLogger::Instance().Initialize()) //Не удалось запустить логгер
	{
		std::cout << ISLogger::Instance().GetErrorString().toStdString() << std::endl;
		return false;
	}

	//Установим кодировку для консольного приложения под Debug-Win32
#if defined(WIN32) && defined(DEBUG)
	if (SetConsoleOutputCP(65001) == FALSE)
	{
		ISLOGGER_W("Startup", "Error changed console encoding");
	}
#endif

	if (!ISSystem::CreateDir(QCoreApplication::applicationDirPath() + "/Temp", ErrorString)) //Не удалось создать папку для временных файлов
	{
		ISLOGGER_E("Startup", "Not create temp dir: " + ErrorString);
		return false;
	}

	if (!ISConfig::Instance().Initialize("Server"))
	{
		ISLOGGER_E("ISConfig", "Not initialize: " + ISConfig::Instance().GetErrorString());
		return false;
	}

	//Загрузка локализации ядра
	if (!ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CARAT))
	{
		ISLOGGER_E("ISLocalization", QString("Not init localization file \"%1\": %2").arg(LOCALIZATION_FILE_CARAT).arg(ISLocalization::Instance().GetErrorString()));
		return false;
	}

	//Проверяем наличие сервера в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISLOGGER_E("ISConfig", "server not specified");
		return false;
	}

	//Проверяем наличие порта в конфигурационном файле
	if (CONFIG_INT(CONST_CONFIG_CONNECTION_PORT) == 0)
	{
		ISLOGGER_E("ISConfig", "port not specified");
		return false;
	}

	//Проверяем наличие имени базы данных в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE).isEmpty())
	{
		ISLOGGER_E("ISConfig", "database name not specified");
		return false;
	}

	//Проверяем наличие логина в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty())
	{
		ISLOGGER_E("ISConfig", "login not specified");
		return false;
	}

	//Проверяем наличие пароля в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).isEmpty())
	{
		ISLOGGER_E("ISConfig", "password not specified");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Run(const QStringList &Arguments)
{
	QString Argument = Arguments.front();
	if (Argument == "--help" || Argument == "-h")
	{
		Help();
	}
	else if (Argument == "--version" || Argument == "-v")
	{
		Version();
	}
	else if (Argument == "--shutdown" || Argument == "-s")
	{
		SendShutdown();
	}
	else if (Argument == "--conf-reset")
	{
		ConfigReset();
	}
	else
	{
		std::cout << "Argument \"" << Argument.toStdString() << "\" not support" << std::endl;
		Help();
	}
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Run()
{
    ISLOGGER(QString("Starting service...\n"
		"Version: %1 (%2 %3)\n"
		"Branch: %4 (%5)\n"
		"Host name: %6\n"
		"Domain name: %7\n"
		"OS: %8\n"
		"Main ThreadID: %9\n"
		"PID: %10").
		arg(ISVersionInfo::Instance().ToString()).
		arg(ISVersionInfo::Instance().Info.Configuration).
		arg(ISVersionInfo::Instance().Info.Platform).
		arg(ISVersionInfo::Instance().Info.Branch).
		arg(ISVersionInfo::Instance().Info.Hash).
		arg(QSysInfo::machineHostName()).
		arg(QHostInfo::localDomainName()).
		arg(QSysInfo::prettyProductName()).
		arg(CURRENT_THREAD_ID()).
		arg(GET_PID()));

	if (CONFIG_BOOL(CONST_CONFIG_CONTROLLER_INCLUDE))
	{
		Controller = new ISCaratController(this);
		if (Controller->Start())
		{
			connect(Controller, &ISCaratController::Shutdown, this, &ISCaratApplication::Shutdown);
		}
		else
		{
			ISLOGGER_E("ISCaratController", "starting failed");
			return false;
		}
	}
	
	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE))
	{
		TcpServer = new ISTcpServer(this);
		if (!TcpServer->Run())
		{
			ISLOGGER_E("ISTcpServer", "starting failed");
			return false;
		}
	}

	if (CONFIG_BOOL(CONST_CONFIG_AMI_INCLUDE))
	{
		Asterisk = new ISAsterisk(this);
		if (!Asterisk->Start())
		{
			ISLOGGER_E("ISAsterisk", "starting failed");
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Shutdown()
{
	ISLOGGER_I(__CLASS__, "Shutdown...");

	if (CONFIG_BOOL(CONST_CONFIG_CONTROLLER_INCLUDE))
	{
		Controller->Stop();
	}

	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE))
	{
		TcpServer->Stop();
	}
	emit Quit();
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Help()
{
#ifdef WIN32
	std::cout << "Usage: Carat [argument]" << std::endl;
#else
	std::cout << "Usage: ./Carat [argument]" << std::endl;
#endif
	std::cout << std::endl;
	std::cout << "Arguments:" << std::endl;
	std::cout << "  -h, --help\t\tshow this help and exit" << std::endl;
	std::cout << "  -v, --version\t\tshow version and exit" << std::endl;
	std::cout << "  -s, --shutdown\tshutdown service" << std::endl;
	std::cout << std::endl;
#ifdef WIN32
	std::cout << "Example: Carat.exe (service mode)" << std::endl;
#else
	std::cout << "Example: ./Carat (service mode)" << std::endl;
#endif
	std::cout << "* No arguments needed to start in service mode" << std::endl;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Version()
{
	std::cout << "Carat (" << ISVersionInfo::Instance().ToStdString() << ") " << ISVersionInfo::Instance().Info.Configuration.toStdString() << " " << ISVersionInfo::Instance().Info.Platform.toStdString() << std::endl;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::SendShutdown()
{
	SendCommand(CARAT_LOCAL_API_SHUTDOWN);
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ConfigReset()
{
	//Формируем пути
	QFileInfo FileInfo(ISConfig::Instance().GetConfigPath());
	QString PathCurrentFile = FileInfo.filePath(),
		PathOldFile = FileInfo.path() + "/" + FileInfo.fileName() + '.' + EXTENSION_OLD;

	QFile File(PathOldFile);

	//Если копия старого файла уже существует - удаляем её
	if (File.exists())
	{
		if (!File.remove()) //Не получилось удалить старый файл
		{
			ISDEBUG_E("Error delete old file: " + File.errorString());
			return;
		}
	}

	//Установим имя файла как текущий
	File.setFileName(PathCurrentFile);

	//Делаем копию текущего файла
	if (!File.copy(FileInfo.path() + "/" + FileInfo.fileName() + SYMBOL_POINT + EXTENSION_OLD)) //При создании копии возникла ошибка
	{
		ISDEBUG_L("Error save old file: " + File.errorString());
		return;
	}

	//Удаляем текущий файл
	if (!File.remove()) //Не получилось удалить его
	{
		ISDEBUG_L("Error remove old file: " + File.errorString());
		return;
	}

	if (!ISConfig::Instance().ReInitialize("Server"))
	{
		ISDEBUG_L("Error init new file: " + ISConfig::Instance().GetErrorString());
		return;
	}
}
//-----------------------------------------------------------------------------
void ISCaratApplication::SendCommand(const QByteArray &ByteArray)
{
	QTcpSocket TcpSocket;

	//Подключаемся к Карату
	std::cout << "Connecting..." << std::endl;
	TcpSocket.connectToHost(QHostAddress::LocalHost, CONFIG_INT(CONST_CONFIG_CONTROLLER_PORT));
	if (!TcpSocket.waitForConnected(1000))
	{
		std::cout << "Error: " << TcpSocket.errorString().toStdString() << std::endl;
		return;
	}

	//Посылаем данные
	std::cout << "Send: " << ByteArray.toStdString() << std::endl;
    if (TcpSocket.write(ByteArray) == -1) //Не удалось послать данные
    {
        std::cout << "Error sending: " << TcpSocket.errorString().toStdString() << std::endl;
        return;
    }

    if (!TcpSocket.flush())
    {
        std::cout << "Error flushing: " << TcpSocket.errorString().toStdString() << std::endl;
        return;
    }

	//Ждём ответа
    std::cout << "Wait answer..." << std::endl;
	while (true)
	{
		ISSleep(1);
		ISSystem::ProcessEvents();

		if (TcpSocket.bytesAvailable() > 0) //Дождались ответа - выводим в консоль и выходим из функции
		{
			QString Answer = TcpSocket.readAll();
			ISSystem::RemoveLastSymbolLoop(Answer, '\n');
			std::cout << "Answer: " << Answer.toStdString() << std::endl;
			break;
		}
	}
}
//-----------------------------------------------------------------------------
