#include "CGConfigurator.h"
#include "ISNamespace.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISConsole.h"
#include "ISDatabase.h"
#include "ISCore.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
#include "ISLogger.h"
#include "ISMetaDataHelper.h"
#include "ISConsole.h"
#include "ISVersion.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
#include "ISLocalization.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
#include "CGConfiguratorCreate.h"
#include "CGConfiguratorUpdate.h"
#include "CGConfiguratorDelete.h"
#include "CGConfiguratorService.h"
#include "CGConfiguratorShow.h"
#include "CGConfiguratorFIAS.h"
//-----------------------------------------------------------------------------
static QString QC_DATABASE = "CREATE DATABASE %1 WITH OWNER = %2 ENCODING = 'UTF8'";
//-----------------------------------------------------------------------------
static QString QS_DATABASE = "SELECT datname FROM pg_database ORDER BY datname";
//-----------------------------------------------------------------------------
static QString QS_FUNCTION = "SELECT COUNT(*) "
							 "FROM information_schema.routines "
							 "WHERE routines.routine_name = 'get_configuration_name'";
//-----------------------------------------------------------------------------
std::vector<CGSection*> Arguments;
QString DBHost, DBName, DBLogin, DBPassword;
int DBPort = 0;
//-----------------------------------------------------------------------------
void RegisterMetatype(); //Регистрация мета-типов
bool InitConfiguratorScheme(QString &ErrorString); //Инициализация схемы конфигуратора
bool CreateDatabase(); //Проверка существования БД
void InterpreterMode(bool &IsRunning); //Режим интерпретатора
bool Execute(const QString &Argument); //Выполнить одиночную команду
bool Execute(const QString &Argument, const QString &SubArgument); //Выполнить двойную команду
QString GetClassName(const QString &Argument); //Получить имя класса
QStringList ParseInputCommand(const QString &Command); //Парсинг введенной команды
void FillConfig(); //Заполнение конфигурационного файла
bool GetDatabaseList(QStringList &StringList); //Получить список баз на сервере
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	RegisterMetatype();
	QCoreApplication CoreArralication(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, "Server", ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	//Загрузка трансляций QT
	if (!ISLocalization::Instance().LoadTraslatorQT())
	{
		ISLOGGER_W(ISLocalization::Instance().GetErrorString());
	}

	//Загрузка локализации ядра
	Result = ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CONFIGURATOR);
	if (!Result)
	{
		ISLOGGER_E(QString("Error init localization file \"%1\": %2").arg(LOCALIZATION_FILE_CORE).arg(ISLocalization::Instance().GetErrorString()));
		return EXIT_FAILURE;
	}

	Result = InitConfiguratorScheme(ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	FillConfig();
    Result = CreateDatabase();
	if (!Result)
	{
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	QString ConfigurationName = ISMetaDataHelper::GetConfigurationName(ErrorString);
	if (ConfigurationName.isEmpty())
	{
		ISLOGGER_E("Error getting configuration name: " + ErrorString);
		return EXIT_FAILURE;
	}

	Result = ISMetaData::Instance().Initialize(ConfigurationName, true, true);
	if (!Result)
	{
		ISLOGGER_E("Initialize meta data: " + ISMetaData::Instance().GetErrorString());
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	ISVectorString Arguments = CoreArralication.arguments().toVector().toStdVector();
	Arguments.erase(Arguments.begin());
	if (Arguments.size() == 0)
	{
		ISLOGGER_L(QString("Configurator [Version %1] %2 %3").arg(ISVersion::Instance().ToString()).arg(ISVersion::Instance().Info.Configuration).arg(ISVersion::Instance().Info.Platform));
		ISLOGGER_L("Welcome to the Configurator.");
		ISLOGGER_L("DBHost: " + DBHost);
		ISLOGGER_L("DBName: " + DBName);
		ISLOGGER_L("DBPort: " + QString::number(DBPort));
		ISLOGGER_L("Configuration: " + ConfigurationName);
		ISLOGGER_L("Enter the \"help\" command to get help");
		
		while (Result)
		{
			InterpreterMode(Result);
		}
	}
	else if (Arguments.size() == 1)
	{
		Result = Execute(Arguments[0].toLower());
	}
	else if (Arguments.size() == 2)
	{
		Result = Execute(Arguments[0].toLower(), Arguments[1].toLower());
	}
	ISCore::ExitApplication();
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
void RegisterMetatype()
{
	qRegisterMetaType<CGConfiguratorCreate*>("CGConfiguratorCreate");
	qRegisterMetaType<CGConfiguratorUpdate*>("CGConfiguratorUpdate");
	qRegisterMetaType<CGConfiguratorDelete*>("CGConfiguratorDelete");
	qRegisterMetaType<CGConfiguratorService*>("CGConfiguratorService");
	qRegisterMetaType<CGConfiguratorShow*>("CGConfiguratorShow");
	qRegisterMetaType<CGConfiguratorFIAS*>("CGConfiguratorFIAS");
}
//-----------------------------------------------------------------------------
bool InitConfiguratorScheme(QString &ErrorString)
{
	QFile FileScheme(PATH_CONFIGURATOR_SCHEME);
	bool Result = FileScheme.open(QIODevice::ReadOnly);
	if (Result)
	{
		QString Content = FileScheme.readAll();
		FileScheme.close();

		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode NodeSections = DomElement.firstChild();
		while (!NodeSections.isNull()) //Обход разделов
		{
			CGSection *Section = new CGSection
			{
				NodeSections.attributes().namedItem("Name").nodeValue(),
				NodeSections.attributes().namedItem("ClassName").nodeValue(),
				std::vector<CGSectionItem*>()
			};
			Arguments.emplace_back(Section);

			QDomNode NodeFunctions = NodeSections.firstChild();
			while (!NodeFunctions.isNull()) //Обход подразделов
			{
				Section->Items.emplace_back(new CGSectionItem
				{
					NodeFunctions.attributes().namedItem("FunctionName").nodeValue(),
					NodeFunctions.attributes().namedItem("FunctionLocalName").nodeValue(),
					NodeFunctions.attributes().namedItem("Description").nodeValue()
				});
				NodeFunctions = NodeFunctions.nextSibling();
			}
			NodeSections = NodeSections.nextSibling();
		}
	}
	else
	{
		ErrorString = FileScheme.errorString();
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool CreateDatabase()
{
	bool Result = ISDatabase::Instance().Connect(CONNECTION_SYSTEM, DBHost, DBPort, SYSTEM_DATABASE_NAME, DBLogin, DBPassword), Exist = true;
	if (!Result) //Не удалось подключиться к системной БД
	{
		ISLOGGER_E(QString("Not connected to system database \"%1\": %2").arg(SYSTEM_DATABASE_NAME).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	Result = ISDatabase::Instance().CheckExistDatabase(CONNECTION_SYSTEM, DBName, Exist);
	if (!Result) //Удалось проверить наличие БД
	{
		ISLOGGER_E(QString("Checking exist database \"%1\": %2").arg(DBName).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	if (Exist) //БД существует - подключаемся к существующей
	{
		Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
	}
	else //БД не существует - создаём её
	{
		//Спрашиваем у пользоваеля, создавать БД?
		Result = ISConsole::Question("Database \"" + DBName + "\" not exist! Create?");
		if (Result) //Создаем БД
		{
			QSqlError SqlError = ISDatabase::Instance().GetDB(CONNECTION_SYSTEM).exec(QC_DATABASE.arg(DBName).arg(DBLogin)).lastError(); //Исполнение запроса на создание базы данных
			Result = SqlError.type() == QSqlError::NoError;
			Result ? ISLOGGER_L("The \"" + DBName + "\" database was created successfully. It is recommended that you run the \"update database\" command")
				: ISLOGGER_L("Error creating database \"" + DBName + "\": " + SqlError.databaseText());
			if (Result) //Если БД была создана - подключаемся к ней
			{
				Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
			}
		}
		else //Пользователь отказался
		{
			ISLOGGER_W("You have refused to create a database");
		}
	}

	//Отключаемся от системной БД
	ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	
	if (!Result) //На каком-то из этапов создания БД возникла ошибка - выходим
	{
		return Result;
	}

	//Проверяем наличие функции для получения конфигурации базы
	ISQuery qFunction(QS_FUNCTION);
	qFunction.SetShowLongQuery(false);
	Result = qFunction.ExecuteFirst();
	if (!Result)
	{
		ISLOGGER_E("Not checked configuration function: " + qFunction.GetErrorString());
		return Result;
	}

	if (!qFunction.ReadColumn("count").toBool()) //Если функция не существует - запрашиваем имя конфигурации и создаём функцию
	{
		//Запрашиваем название конфигурации
		QString ConfigurationName = ISConsole::GetString("Input configuration name (from file Configuration.xml): ");
		Result = !ConfigurationName.isEmpty();
		if (!Result) //Если название не ввели - выходим с ошибкой
		{
			ISLOGGER_W("Configuration name is empty.");
			return Result;
		}

		//Создаём функцию
		Result = qFunction.Execute("CREATE OR REPLACE FUNCTION get_configuration_name() RETURNS VARCHAR AS $$ BEGIN RETURN '" + ConfigurationName + "'; END; $$ LANGUAGE plpgsql IMMUTABLE");
		if (!Result)
		{
			ISLOGGER_E(qFunction.GetErrorString());
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void InterpreterMode(bool &IsRunning)
{
	ISLOGGER_N();
	QString Command = ISConsole::GetString("Enter command (press Enter or Return to exit): ");
	IsRunning = !Command.isEmpty();
	if (IsRunning)
	{
		QStringList StringList = ParseInputCommand(Command);
		if (StringList.count() == 1)
		{
			if (StringList.front().toLower() == "exit" || StringList.front() == "quit") //Выход
			{
				IsRunning = false;
				return;
			}
			else if (StringList.front() == "restart") //Перезапуск
			{
				QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_FILE);
				IsRunning = false;
				return;
			}
			Execute(StringList.front());
		}
		else if (StringList.count() == 2)
		{
			Execute(StringList.front(), StringList.at(1));
		}
		else
		{
			ISLOGGER_L("command not found");
		}
	}
}
//-----------------------------------------------------------------------------
bool Execute(const QString &Argument)
{
	CGConfigurator Configurator(Arguments);
	bool Result = ISSystem::CheckExistSlot(&Configurator, Argument);
	if (Result)
	{
		ISCountingTime CountingTime;
		bool ReturnValue = true;
		Result = QMetaObject::invokeMethod(&Configurator, Argument.toUtf8().data(), Q_RETURN_ARG(bool, ReturnValue));
		if (Result)
		{
			if (ReturnValue)
			{
				ISLOGGER_L("Command \"" + Argument + "\" executed with " + QString::number(CountingTime.Elapsed()) + " msec");
			}
			else
			{
				ISLOGGER_L("Command \"" + Argument + "\" executed with error " + Configurator.GetErrorString());
			}
		}
		else
		{
			ISLOGGER_E("Command \"" + Argument + "\" not executed.");
		}
		Result = ReturnValue;
	}
	else
	{
		ISLOGGER_L("Command \"" + Argument + "\" not found");
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool Execute(const QString &Argument, const QString &SubArgument)
{
	CGConfiguratorBase *CommandBase = ISAlgorithm::CreatePointer<CGConfiguratorBase *>(GetClassName(Argument));
	bool Result = ISSystem::CheckExistSlot(CommandBase, SubArgument);
	if (Result)
	{
		ISCountingTime CountingTime;
		bool ReturnValue = true;
		Result = QMetaObject::invokeMethod(CommandBase, SubArgument.toLocal8Bit().constData(), Q_RETURN_ARG(bool, ReturnValue));
		if (Result)
		{
			if (ReturnValue)
			{
				ISLOGGER_L(QString("Command \"%1 %2\" executed with %3 msec").arg(Argument).arg(SubArgument).arg(CountingTime.Elapsed()));
			}
			else
			{
				ISLOGGER_L(QString("Command \"%1 %2\" executed with error: %3").arg(Argument).arg(SubArgument).arg(CommandBase->GetErrorString()));
			}
		}
		else
		{
			ISLOGGER_E(QString("Command \"%1 %2\" not executed.").arg(Argument).arg(SubArgument));
		}
		Result = ReturnValue;
	}
	else
	{
		ISLOGGER_L("Command \"" + SubArgument + "\" not found");
	}
	delete CommandBase;
	return Result;
}
//-----------------------------------------------------------------------------
QString GetClassName(const QString &Argument)
{
	for (CGSection *Section : Arguments)
	{
		if (Section->Name.toLower() == Argument)
		{
			return Section->ClassName;
		}
	}
	return QString();
}
//-----------------------------------------------------------------------------
QStringList ParseInputCommand(const QString &Command)
{
	QStringList StringList;
	for (const QString &String : Command.split(SYMBOL_SPACE))
	{
		if (String.length())
		{
			StringList.append(String.toLower());
		}
	}
	return StringList;
}
//-----------------------------------------------------------------------------
void FillConfig()
{
	//Получаем сервер
	DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	while (DBHost.isEmpty())
	{
		DBHost = ISConsole::GetString("Enter the host: ");
		if (DBHost.isEmpty())
		{
			ISLOGGER_L("Host is empty!");
		}
	}

	//Получаем порт
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	while (!DBPort)
	{
		DBPort = ISConsole::GetInt("Enter the port: ");
		if (!DBPort)
		{
			ISLOGGER_L("Port is empty or null!");
		}
	}

	//Получаем логин
	DBLogin = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	while (DBLogin.isEmpty())
	{
		DBLogin = ISConsole::GetString("Enter the login: ");
		if (DBLogin.isEmpty())
		{
			ISLOGGER_L("Login is empty!");
		}
	}

	//Получаем пароль
	DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	while (DBPassword.isEmpty())
	{
		DBPassword = ISConsole::GetString("Enter the password: ");
		if (DBPassword.isEmpty())
		{
			ISLOGGER_L("Password is empty!");
		}
	}

	//Получаем имя БД
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	if (DBName.isEmpty()) //Если база данных не указана
	{
		if (ISConsole::Question("Show database list?")) //Предлагаем показать список всех баз на сервере
		{
			QStringList StringList;
			if (GetDatabaseList(StringList)) //Если удалось запросить список баз - выводим его в консоль
			{
				for (int i = 0; i < StringList.size(); ++i)
				{
					ISLOGGER_L(QString("%1. %2").arg(i + 1).arg(StringList[i]));
				}
			}
		}

		while (DBName.isEmpty())
		{
			DBName = ISConsole::GetString("Enter the database name: ");
			if (DBName.isEmpty())
			{
				ISLOGGER_L("Database name is empty!");
			}
		}
	}

	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_SERVER, DBHost);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PORT, DBPort);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_DATABASE, DBName);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_LOGIN, DBLogin);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PASSWORD, DBPassword);
	ISConfig::Instance().SaveForce();
}
//-----------------------------------------------------------------------------
bool GetDatabaseList(QStringList &StringList)
{
	bool Result = ISDatabase::Instance().Connect(CONNECTION_SYSTEM, DBHost, DBPort, SYSTEM_DATABASE_NAME, DBLogin, DBPassword);
	if (Result)
	{
		{
			ISQuery qSelect(ISDatabase::Instance().GetDB(CONNECTION_SYSTEM), QS_DATABASE);
			Result = qSelect.Execute();
			if (Result)
			{
				while (qSelect.Next())
				{
					StringList.push_back(qSelect.ReadColumn("datname").toString());
				}
			}
			else
			{
				ISLOGGER_E("Error getting database list: " + qSelect.GetErrorString());
			}
		}
		ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	}
	else
	{
		ISLOGGER_E("Not connected to system database: " + ISDatabase::Instance().GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
