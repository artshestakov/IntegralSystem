#include "ISCaratApplication.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "ISVersionInfo.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISConsole.h"
#include "ISConfigurations.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
void QtLog(QtMsgType MessageType, const QMessageLogContext &MessageLogContext, const QString &Message)
{
	IS_UNUSED(MessageType);
	IS_UNUSED(MessageLogContext);
	ISLOGGER_Q(Message);
}
//-----------------------------------------------------------------------------
ISCaratApplication::ISCaratApplication(int &argc, char **argv)
	: QCoreApplication(argc, argv),
	ErrorString(NO_ERROR_STRING),
	TcpServer(nullptr),
    Asterisk(nullptr),
    PathFileStop(QCoreApplication::applicationDirPath() + "/Temp/Carat.stop")
{
    connect(this, &ISCaratApplication::Stopped, this, &ISCaratApplication::Shutdown);
	qInstallMessageHandler(QtLog);
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
bool ISCaratApplication::Initialize()
{
	//Запускаем логгер
	if (!ISLogger::Instance().Initialize())
	{
		ISDEBUG_L(ISLogger::Instance().GetErrorString());
		return false;
	}

	//Установим кодировку для консольного приложения под Debug-Win32
	if (!ISConsole::InstallEncoding(65001))
	{
		ISLOGGER_W("Startup", "Error changed console encoding");
	}

	//Загрузка локализации ядра
	if (!ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CARAT))
	{
		ISLOGGER_E("ISLocalization", QString("Not init localization file \"%1\": %2").arg(LOCALIZATION_FILE_CARAT).arg(ISLocalization::Instance().GetErrorString()));
		return false;
	}

	//Создаём папку для временных файлов
	if (!ISSystem::CreateDir(QCoreApplication::applicationDirPath() + "/Temp", ErrorString))
	{
		ISLOGGER_E("Startup", "Not create temp dir: " + ErrorString);
		return false;
	}

    QFile File(PathFileStop);
    if (File.exists(PathFileStop))
    {
        if (!File.remove())
        {
            ISLOGGER_E(__CLASS__, "Not remove stop file: " + File.errorString());
            return false;
        }
    }

	//Инициализируем конфигурационный файл
	if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
	{
		ISLOGGER_E("ISConfig", "Not initialize: " + ISConfig::Instance().GetErrorString());
		return false;
	}

	//Проверяем валидность конфигурационного файла
	if (!ISConfig::Instance().IsValid())
	{
		ISLOGGER_E("ISConfig", "File is not valid: " + ISConfig::Instance().GetErrorString());
		return false;
	}

	//Выбираем активную конфигурацию
	QString ConfigurationName = CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION);
	if (!ISConfigurations::Instance().Set(ConfigurationName))
	{
		ISLOGGER_E(__CLASS__, "Not found configuration \"" + ConfigurationName + "\"");
		return false;
	}

	//Инициализируем мета-данные
	if (!ISMetaData::Instance().Initialize(ConfigurationName, false, false))
	{
		ISLOGGER_E("ISMetaData", "Not init meta data: " + ISMetaData::Instance().GetErrorString());
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Run(const ISVectorString &Arguments)
{
	QString Argument = Arguments.front();
	if (Argument == "--help" || Argument == "-h")
	{
        return Help();
	}
	else if (Argument == "--version" || Argument == "-v")
	{
        return Version();
	}
	else if (Argument == "--shutdown" || Argument == "-s")
	{
        return SendShutdown();
	}
    else if (Argument == "--conf-create" || Argument == "-cc")
	{
        return ConfigCreate();
	}
    else if (Argument == "--conf-reset" || Argument == "-cr")
	{
        return ConfigReset();
	}
	else
	{
		ISDEBUG_L("Argument \"" + Argument + "\" not support");
        (void)Help();
	}
    return false;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Run()
{
	if (!Initialize())
	{
		return false;
	}

    ISLOGGER(QString("Starting service...\n"
		"Version: %1 (%2 %3)\n"
		"Branch: %4 (%5)\n"
		"Host name: %6\n"
		"OS: %7\n"
		"Main thread: %8\n"
		"PID: %9\n"
		"Configuration: %10\n"
		"Database: %11").
		arg(ISVersionInfo::Instance().Info.Version).
		arg(ISVersionInfo::Instance().Info.Configuration).
		arg(ISVersionInfo::Instance().Info.Platform).
		arg(ISVersionInfo::Instance().Info.BranchName).
		arg(ISVersionInfo::Instance().Info.Hash).
		arg(QSysInfo::machineHostName()).
		arg(QSysInfo::prettyProductName()).
		arg(CURRENT_THREAD_ID()).
        arg(CURRENT_PID()).
		arg(CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION)).
		arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE)));

	//Подключаемся к БД
	if (!ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
		CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
		CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN), CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD)))
	{
		ISLOGGER_E(__CLASS__, "Not connected to db: " + ISDatabase::Instance().GetErrorString());
		return false;
	}

	//Если TCP-сервер включен - запускаем его
	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE))
	{
		TcpServer = new ISTcpServer(this);
		if (!TcpServer->Run())
		{
			ISLOGGER_E("ISTcpServer", "starting failed");
			return false;
		}
	}

	//Если модуль работы с AMI включен - запускаем его
	if (CONFIG_BOOL(CONST_CONFIG_AMI_INCLUDE))
	{
		Asterisk = new ISAsterisk(this);
		if (!Asterisk->Start())
		{
			ISLOGGER_E("ISAsterisk", "starting failed");
			return false;
		}
	}

    //Запускаем контроллер остановки сервиса
    if (!QtConcurrent::run(this, &ISCaratApplication::StopController).isStarted())
    {
        ISLOGGER_E(__CLASS__, "Not starting stop controller");
        return false;
    }
	return true;
}
//-----------------------------------------------------------------------------
void ISCaratApplication::Shutdown()
{
	ISLOGGER_I(__CLASS__, "Shutdown...");

	//Останавливаем TCP-сервер, если запущен
	if (CONFIG_BOOL(CONST_CONFIG_TCPSERVER_INCLUDE) && TcpServer)
	{
		TcpServer->Stop();
	}

	//Останавливаем модуль Asterisk, если запущен
	if (CONFIG_BOOL(CONST_CONFIG_AMI_INCLUDE) && Asterisk)
	{
		Asterisk->quit();
	}

	//Отключаемся от БД
	ISDatabase::Instance().Disconnect(CONNECTION_DEFAULT);
    quit(); //В конце вызываем остановку приложения
}
//-----------------------------------------------------------------------------
void ISCaratApplication::StopController()
{
    QFile File(PathFileStop);
    while (true)
    {
        ISSleep(1000);
        if (File.exists())
        {
            emit Stopped();
            break;
        }
    }
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Help()
{
#ifdef WIN32
    ISDEBUG_L("Usage: Carat.exe [argument]");
#else
	ISDEBUG_L("Usage: ./Carat [argument]");
#endif
	ISDEBUG();
	ISDEBUG_L("Arguments:");
    ISDEBUG_L("  -h,\t--help\t\tshow this help and exit");
    ISDEBUG_L("  -v,\t--version\t\tshow version and exit");
    ISDEBUG_L("  -s,\t--shutdown\t\tshutdown service");
    ISDEBUG_L("  -cc,\t--conf-create\t\tcreate config file");
    ISDEBUG_L("  -cr,\t--conf-reset\t\treset config file");
	ISDEBUG();
#ifdef WIN32
	ISDEBUG_L("Example: Carat.exe (service mode)");
#else
	ISDEBUG_L("Example: ./Carat (service mode)");
#endif
	ISDEBUG_L("* No arguments needed to start in service mode");
    return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::Version()
{
	ISDEBUG_L(QString("Carat (%1) %2 %3").arg(ISVersionInfo::Instance().Info.Version).arg(ISVersionInfo::Instance().Info.Configuration).arg(ISVersionInfo::Instance().Info.Platform));
    return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::SendShutdown()
{
    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISDEBUG_E("Not created config: " + ISConfig::Instance().GetErrorString());
        return false;
    }

    QFile File(PathFileStop);
    bool Result = File.open(QIODevice::WriteOnly);
    if (Result)
    {
        File.close();
    }
    else
    {
        ISDEBUG_E("Not created stop file: " + File.errorString());
    }
    return Result;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::ConfigCreate()
{
    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
    {
        ISDEBUG_E("Not created config: " + ISConfig::Instance().GetErrorString());
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISCaratApplication::ConfigReset()
{
    //Если конфигурационный файл не существует - выходим
    QString PathCurrentFile = QCoreApplication::applicationDirPath() + '/' + CONFIG_TEMPLATE_SERVER + SYMBOL_POINT + EXTENSION_INI;
    if (!QFile::exists(PathCurrentFile))
    {
        ISDEBUG_E("Config file \"" + PathCurrentFile + "\" not exist, reset impossible.");
        return false;
    }

	//Формируем пути
    QFileInfo FileInfo(PathCurrentFile);
    QString PathOldFile = FileInfo.path() + "/" + FileInfo.fileName() + '.' + EXTENSION_OLD;

	QFile File(PathOldFile);

	//Если копия старого файла уже существует - удаляем её
	if (File.exists())
	{
		if (!File.remove()) //Не получилось удалить старый файл
		{
			ISDEBUG_E("Error delete old file: " + File.errorString());
            return false;
		}
	}

	//Установим имя файла как текущий
	File.setFileName(PathCurrentFile);

	//Делаем копию текущего файла
	if (!File.copy(FileInfo.path() + "/" + FileInfo.fileName() + SYMBOL_POINT + EXTENSION_OLD)) //При создании копии возникла ошибка
	{
		ISDEBUG_L("Error save old file: " + File.errorString());
        return false;
	}

	//Удаляем текущий файл
	if (!File.remove()) //Не получилось удалить его
	{
		ISDEBUG_L("Error remove old file: " + File.errorString());
        return false;
	}

	if (!ISConfig::Instance().ReInitialize(CONFIG_TEMPLATE_SERVER))
	{
		ISDEBUG_L("Error init new file: " + ISConfig::Instance().GetErrorString());
        return false;
	}
    return true;
}
//-----------------------------------------------------------------------------
