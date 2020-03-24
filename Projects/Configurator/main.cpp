#include "StdAfx.h"
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
#include "ISLocalization.h"
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
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	RegisterMetatype();
	QCoreApplication CoreArralication(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, ErrorString);
	if (!Result)
	{
		ISLOGGER_ERROR(ErrorString);
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	Result = InitConfiguratorScheme(ErrorString);
	if (!Result)
	{
		ISLOGGER_ERROR(ErrorString);
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	FillConfig();
	if (DBLogin.isEmpty() || DBPassword.isEmpty())
	{
		ISLOGGER_ERROR("Not specified server or password in config file");
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

    Result = CreateDatabase();
	if (!Result)
	{
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	Result = ISMetaData::GetInstanse().Initialize(ISMetaDataHelper::GetConfigurationName(), true, true);
	if (!Result)
	{
		ISLOGGER_ERROR("Initialize meta data: " + ISMetaData::GetInstanse().GetErrorString());
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CORE);

	ISVectorString Arguments = CoreArralication.arguments().toVector().toStdVector();
	Arguments.erase(Arguments.begin());
	if (Arguments.size() == 0)
	{
		ISLOGGER_UNKNOWN(QString("Configurator [Version %1]").arg(ISVersion::Instance().ToString()));
		ISLOGGER_UNKNOWN("Welcome to the Configurator.");
		ISLOGGER_UNKNOWN("DBHost: " + DBHost);
		ISLOGGER_UNKNOWN("DBName: " + DBName);
		ISLOGGER_UNKNOWN("DBPort: " + QString::number(DBPort));
		ISLOGGER_UNKNOWN("Enter the \"help\" command to get help");
		
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
		ISLOGGER_ERROR(QString("Not connected to system database \"%1\": %2").arg(SYSTEM_DATABASE_NAME).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	Result = ISDatabase::Instance().CheckExistDatabase(CONNECTION_SYSTEM, DBName, Exist);
	if (!Result) //Удалось проверить наличие БД
	{
		ISLOGGER_ERROR(QString("Checking exist database \"%1\": %2").arg(DBName).arg(ISDatabase::Instance().GetErrorString()));
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
			Result ? ISLOGGER_UNKNOWN("The \"" + DBName + "\" database was created successfully. It is recommended that you run the \"update database\" command")
				: ISLOGGER_UNKNOWN("Error creating database \"" + DBName + "\": " + SqlError.databaseText());
			if (Result) //Если БД была создана - подключаемся к ней
			{
				Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
			}
		}
		else //Пользователь отказался
		{
			ISLOGGER_WARNING("You have refused to create a database");
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
		ISLOGGER_ERROR("Not checked configuration function: " + qFunction.GetErrorString());
		return Result;
	}

	if (!qFunction.ReadColumn("count").toBool()) //Если функция не существует - запрашиваем имя конфигурации и создаём функцию
	{
		//Запрашиваем название конфигурации
		QString ConfigurationName = ISConsole::GetString("Input configuration name (from file Configuration.xml): ");
		Result = !ConfigurationName.isEmpty();
		if (!Result) //Если название не ввели - выходим с ошибкой
		{
			ISLOGGER_WARNING("Configuration name is empty.");
			return Result;
		}

		//Создаём функцию
		Result = qFunction.Execute("CREATE OR REPLACE FUNCTION get_configuration_name() RETURNS VARCHAR AS $$ BEGIN RETURN '" + ConfigurationName + "'; END; $$ LANGUAGE plpgsql IMMUTABLE");
		if (!Result)
		{
			ISLOGGER_ERROR(qFunction.GetErrorString());
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
void InterpreterMode(bool &IsRunning)
{
	ISLOGGER_EMPTY();
	QString Command = ISConsole::GetString("Enter command (press Enter or Return to exit): ");
	IsRunning = !Command.isEmpty();
	if (IsRunning)
	{
		QStringList StringList = ParseInputCommand(Command);
		if (StringList.count() == 1)
		{
			if (StringList.at(0).toLower() == "exit" || StringList.at(0) == "quit") //Выход
			{
				IsRunning = false;
				return;
			}
			else if (StringList.at(0) == "restart") //Перезапуск
			{
				QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_FILE);
				IsRunning = false;
				return;
			}
			Execute(StringList.at(0));
		}
		else if (StringList.count() == 2)
		{
			Execute(StringList.at(0), StringList.at(1));
		}
		else
		{
			ISLOGGER_UNKNOWN("command not found");
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
				ISLOGGER_UNKNOWN("Command \"" + Argument + "\" executed with " + QString::number(CountingTime.Elapsed()) + " msec");
			}
			else
			{
				ISLOGGER_UNKNOWN("Command \"" + Argument + "\" executed with error " + Configurator.GetErrorString());
			}
		}
		else
		{
			ISLOGGER_ERROR("Command \"" + Argument + "\" not executed.");
		}
		Result = ReturnValue;
	}
	else
	{
		ISLOGGER_UNKNOWN("Command \"" + Argument + "\" not found");
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool Execute(const QString &Argument, const QString &SubArgument)
{
	CGConfiguratorBase *CommandBase = nullptr;
	int ObjectType = QMetaType::type(GetClassName(Argument).toLocal8Bit().constData());
	bool Result = ObjectType > 0 ? true : false;
	if (Result)
	{
		const QMetaObject *MetaObject = QMetaType::metaObjectForType(ObjectType);
		Result = MetaObject ? true : false;
		if (Result)
		{
			CommandBase = dynamic_cast<CGConfiguratorBase*>(MetaObject->newInstance());
			Result = CommandBase ? true : false;
			if (Result)
			{
				Result = ISSystem::CheckExistSlot(CommandBase, SubArgument);
				if (Result)
				{
					ISCountingTime CountingTime;
					bool ReturnValue = true;
					Result = QMetaObject::invokeMethod(CommandBase, SubArgument.toLocal8Bit().constData(), Q_RETURN_ARG(bool, ReturnValue));
					if (Result)
					{
						if (ReturnValue)
						{
							ISLOGGER_UNKNOWN(QString("Command \"%1 %2\" executed with %3 msec").arg(Argument).arg(SubArgument).arg(CountingTime.Elapsed()));
						}
						else
						{
							ISLOGGER_UNKNOWN(QString("Command \"%1 %2\" executed with error: %3").arg(Argument).arg(SubArgument).arg(CommandBase->GetErrorString()));
						}
					}
					else
					{
						ISLOGGER_ERROR(QString("Command \"%1 %2\" not executed.").arg(Argument).arg(SubArgument));
					}
					Result = ReturnValue;
				}
				else
				{
					ISLOGGER_UNKNOWN("Command \"" + SubArgument + "\" not found");
				}
				delete CommandBase;
			}
			else
			{
				ISLOGGER_UNKNOWN("Class \"" + Argument + "\" not found");
			}
		}
		else
		{
			ISLOGGER_UNKNOWN("Class \"" + Argument + "\" not found");
		}
	}
	else
	{
		ISLOGGER_UNKNOWN("Class \"" + Argument + "\" not found");
	}
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
			ISLOGGER_UNKNOWN("Host is empty!");
		}
	}

	//Получаем порт
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	while (!DBPort)
	{
		DBPort = ISConsole::GetInt("Enter the port: ");
		if (!DBPort)
		{
			ISLOGGER_UNKNOWN("Port is empty or null!");
		}
	}

	//Получаем имя БД
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	while (DBName.isEmpty())
	{
		DBName = ISConsole::GetString("Enter the database name: ");
		if (DBName.isEmpty())
		{
			ISLOGGER_UNKNOWN("Database name is empty!");
		}
	}

	//Получаем логин
	DBLogin = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	while (DBLogin.isEmpty())
	{
		DBLogin = ISConsole::GetString("Enter the login: ");
		if (DBLogin.isEmpty())
		{
			ISLOGGER_UNKNOWN("Login is empty!");
		}
	}

	//Получаем пароль
	DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	while (DBPassword.isEmpty())
	{
		DBPassword = ISConsole::GetString("Enter the password: ");
		if (DBPassword.isEmpty())
		{
			ISLOGGER_UNKNOWN("Password is empty!");
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
