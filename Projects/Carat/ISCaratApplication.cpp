#include "ISCaratApplication.h"
#include "ISConstants.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "ISVersionInfo.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISQueryPool.h"
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
		ISDEBUG_L(ISLogger::Instance().GetErrorString());
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
	else if (Argument == "--conf-create")
	{
		//Выходим из функции, т.к. конфиг был проинициализирован выше в функции Init()
		return;
	}
	else if (Argument == "--conf-reset")
	{
		ConfigReset();
	}
	else
	{
		ISDEBUG_L("Argument \"" + Argument + "\" not support");
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

	//Выполняем проверки заполнения секции Connection в конфигурационном файле
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER).isEmpty())
	{
		ISLOGGER_E("ISConfig", "server not specified");
		return false;
	}
	if (CONFIG_INT(CONST_CONFIG_CONNECTION_PORT) == 0)
	{
		ISLOGGER_E("ISConfig", "port not specified");
		return false;
	}
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE).isEmpty())
	{
		ISLOGGER_E("ISConfig", "database name not specified");
		return false;
	}
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN).isEmpty())
	{
		ISLOGGER_E("ISConfig", "login not specified");
		return false;
	}
	if (CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD).isEmpty())
	{
		ISLOGGER_E("ISConfig", "password not specified");
		return false;
	}

	//Инициализация пула запросов
	if (!ISQueryPool::Instance().Start(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
		CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD)))
	{
		ISLOGGER_E("ISQueryPool", ISQueryPool::Instance().GetErrorString());
		return false;
	}

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
	ISDEBUG_L("Usage: Carat [argument]");
#else
	ISDEBUG_L("Usage: ./Carat [argument]");
#endif
	ISDEBUG();
	ISDEBUG_L("Arguments:");
	ISDEBUG_L("  --help\t\tshow this help and exit");
	ISDEBUG_L("  --version\t\tshow version and exit");
	ISDEBUG_L("  --shutdown\t\tshutdown service");
	ISDEBUG_L("  --conf-create\t\tcreate config file");
	ISDEBUG_L("  --conf-reset\t\treset config file");
	ISDEBUG();
#ifdef WIN32
	ISDEBUG_L("Example: Carat.exe (service mode)");
#else
	ISDEBUG_L("Example: ./Carat (service mode)");
#endif
	ISDEBUG_L("* No arguments needed to start in service mode");
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Version()
{
	ISDEBUG_L("Carat (" + ISVersionInfo::Instance().ToString() + ") " + ISVersionInfo::Instance().Info.Configuration + " " + ISVersionInfo::Instance().Info.Platform);
}
//-----------------------------------------------------------------------------
void ISCaratApplication::SendShutdown()
{
	SendCommand(CARAT_LOCAL_API_SHUTDOWN);
}
//-----------------------------------------------------------------------------
void ISCaratApplication::ConfigCreate()
{

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
	ISDEBUG_L("Connecting...");
	TcpSocket.connectToHost(QHostAddress::LocalHost, CONFIG_INT(CONST_CONFIG_CONTROLLER_PORT));
	if (!TcpSocket.waitForConnected(1000))
	{
		ISDEBUG_L("Error: " + TcpSocket.errorString());
		return;
	}

	//Посылаем данные
	ISDEBUG_L("Send: " + ByteArray);
    if (TcpSocket.write(ByteArray) == -1) //Не удалось послать данные
    {
		ISDEBUG_L("Error sending: " + TcpSocket.errorString());
        return;
    }

    if (!TcpSocket.flush())
    {
		ISDEBUG_L("Error flushing: " + TcpSocket.errorString());
        return;
    }

	//Ждём ответа
	ISDEBUG_L("Wait answer...");
	while (true)
	{
		ISSleep(1);
		ISSystem::ProcessEvents();
		if (TcpSocket.bytesAvailable() > 0) //Дождались ответа - выводим в консоль и выходим из функции
		{
			QString Answer = TcpSocket.readAll();
			ISSystem::RemoveLastSymbolLoop(Answer, '\n');
			ISDEBUG_L("Answer: " + Answer);
			break;
		}
	}
}
//-----------------------------------------------------------------------------
