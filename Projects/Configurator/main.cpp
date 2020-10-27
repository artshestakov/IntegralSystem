#include "CGConfigurator.h"
#include "ISNamespace.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISConsole.h"
#include "ISDatabase.h"
#include "ISCore.h"
#include "ISSystem.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISMetaDataHelper.h"
#include "ISConsole.h"
#include "ISVersionInfo.h"
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
//-----------------------------------------------------------------------------
static QString QC_DATABASE = "CREATE DATABASE %1 WITH OWNER = %2 ENCODING = 'UTF8'";
//-----------------------------------------------------------------------------
static QString QS_DATABASE = "SELECT datname FROM pg_database ORDER BY datname";
//-----------------------------------------------------------------------------
static QString QS_DATABASE_DESCRIPTION = PREPARE_QUERY("SELECT description "
													   "FROM pg_shdescription "
													   "JOIN pg_database ON objoid = pg_database.oid "
													   "WHERE datname = current_database()");
//-----------------------------------------------------------------------------
std::vector<CGSection*> Arguments;
QString DBHost, DBName, DBLogin, DBPassword;
int DBPort = 0;
//-----------------------------------------------------------------------------
bool InitConfiguratorScheme(QString &ErrorString); //Инициализация схемы конфигуратора
bool CreateDatabase(); //Проверка существования БД
void InterpreterMode(bool &IsRunning); //Режим интерпретатора
bool Execute(const QString &Argument); //Выполнить одиночную команду
bool Execute(const QString &Argument, const QString &SubArgument); //Выполнить двойную команду
QString GetClassName(const QString &Argument); //Получить имя класса
QStringList ParseInputCommand(const QString &Command); //Парсинг введенной команды
void FillConfig(); //Заполнение конфигурационного файла
bool GetDatabaseList(QStringList &StringList); //Получить список баз на сервере
bool ExistConfiguration(const QString &ConfigurationName, bool &Exist, QString &ErrorString); //Проверка наличия таакой конфигурации
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //Регистрация мета-типов
    qRegisterMetaType<CGConfiguratorCreate*>("CGConfiguratorCreate");
    qRegisterMetaType<CGConfiguratorUpdate*>("CGConfiguratorUpdate");
    qRegisterMetaType<CGConfiguratorDelete*>("CGConfiguratorDelete");
    qRegisterMetaType<CGConfiguratorService*>("CGConfiguratorService");
    qRegisterMetaType<CGConfiguratorShow*>("CGConfiguratorShow");

	QCoreApplication CoreArralication(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, "Server", ErrorString);
	if (!Result)
	{
		ISLOGGER_E("", ErrorString);
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	//Загрузка трансляций QT
	ISLocalization::Instance().LoadTraslatorQT();
	
	//Загрузка локализации ядра
	Result = ISLocalization::Instance().LoadResourceFile(LOCALIZATION_FILE_CONFIGURATOR);
	if (!Result)
	{
		ISLOGGER_E("ISLocalization", QString("Error init localization file \"%1\": %2").arg(LOCALIZATION_FILE_INTEGRAL_SYSTEM).arg(ISLocalization::Instance().GetErrorString()));
		return EXIT_FAILURE;
	}

	Result = InitConfiguratorScheme(ErrorString);
	if (!Result)
	{
		ISLOGGER_E("", ErrorString);
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
		ISLOGGER_E("", "Error getting configuration name: " + ErrorString);
		return EXIT_FAILURE;
	}

	Result = ISMetaData::Instance().Initialize(ConfigurationName, true, true);
	if (!Result)
	{
		ISLOGGER_E("", "Initialize meta data: " + ISMetaData::Instance().GetErrorString());
		ISConsole::Pause();
		return EXIT_FAILURE;
	}

	ISVectorString ArgumentsCMD = CoreArralication.arguments().toVector().toStdVector();
	ArgumentsCMD.erase(ArgumentsCMD.begin());
	if (ArgumentsCMD.size() == 0)
	{
		ISLOGGER_L(QString("Configurator [Version %1] %2 %3").arg(ISVersionInfo::Instance().ToString()).arg(ISVersionInfo::Instance().Info.Configuration).arg(ISVersionInfo::Instance().Info.Platform));
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
	else if (ArgumentsCMD.size() == 1)
	{
		Result = Execute(ArgumentsCMD[0].toLower());
	}
	else if (ArgumentsCMD.size() == 2)
	{
		Result = Execute(ArgumentsCMD[0].toLower(), ArgumentsCMD[1].toLower());
	}
	ISCore::ExitApplication();
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
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
		ISLOGGER_E("", QString("Not connected to system database \"%1\": %2").arg(SYSTEM_DATABASE_NAME).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	Result = ISDatabase::Instance().CheckExistDatabase(CONNECTION_SYSTEM, DBName, Exist);
	if (!Result) //Удалось проверить наличие БД
	{
		ISLOGGER_E("", QString("Checking exist database \"%1\": %2").arg(DBName).arg(ISDatabase::Instance().GetErrorString()));
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
			ISLOGGER_W("", "You have refused to create a database");
		}
	}

	//Отключаемся от системной БД
	ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	
	if (!Result) //На каком-то из этапов создания БД возникла ошибка - выходим
	{
		return Result;
	}

	//Запрашиваем описание базы данных
	ISQuery qSelectDescriptionDB(QS_DATABASE_DESCRIPTION);
	qSelectDescriptionDB.SetShowLongQuery(false);
	Result = qSelectDescriptionDB.Execute();
	if (!Result)
	{
		ISLOGGER_E("", "Not getting database description: " + qSelectDescriptionDB.GetErrorString());
		return Result;
	}

	QString ConfigurationName; //Имя конфигурации
	if (!qSelectDescriptionDB.First()) //Не удалось перейти на первую строку выборки - запрашиваем имя конфигурации
	{
		while (true)
		{
			//Запрашиваем название конфигурации
			ConfigurationName = ISConsole::GetString("Input configuration name (from file Configuration.xml): ");
			Result = !ConfigurationName.isEmpty();
			if (!Result) //Если название не ввели - выходим с ошибкой
			{
				ISLOGGER_W("", "Configuration name is empty.");
				return Result;
			}

			//Проверяем наличие такой конфигурации
			QString ErrorString;
			Result = ExistConfiguration(ConfigurationName, Exist, ErrorString);
			if (!Result) //Не удалось проверить наличие такой конфигурации
			{
				ISLOGGER_W("", "Not checking configuration name: " + ErrorString);
				return Result;
			}

			if (Exist) //Если конфигурация найдена - выходим из цикла и переходим к созданию функции
			{
				break;
			}
			else //Конфигурация не найдена - переходим к очередной итерации цикла
			{
				ISLOGGER_W("", "Not found configuration: " + ConfigurationName);
			}
		}

		//Комментируем БД
		ISQuery qCommentDB;
		Result = qCommentDB.Execute(QString("COMMENT ON DATABASE %1 IS '%2'").arg(DBName).arg(QString(ISSystem::VariantMapToJsonString(
		{
			{ "ConfigurationName", ConfigurationName }
		}))));
		if (!Result)
		{
			ISLOGGER_E("", "Not updating database description: " + qCommentDB.GetErrorString());
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
				QProcess::startDetached(QCoreApplication::applicationFilePath());
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
		bool ReturnValue = true;
		ISTimePoint TimePoint = ISAlgorithm::GetTick();
		Result = QMetaObject::invokeMethod(&Configurator, Argument.toUtf8().data(), Q_RETURN_ARG(bool, ReturnValue));
		if (Result)
		{
			if (ReturnValue)
			{
				ISLOGGER_L("Command \"" + Argument + "\" executed with " + QString::number(ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint)) + " msec");
			}
			else
			{
				ISLOGGER_L("Command \"" + Argument + "\" executed with error " + Configurator.GetErrorString());
			}
		}
		else
		{
			ISLOGGER_E("", "Command \"" + Argument + "\" not executed.");
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
	QString ClassName = GetClassName(Argument);
	if (ClassName.isEmpty())
	{
		ISLOGGER_E("", "Not found class name with argument: " + Argument);
		return false;
	}

	CGConfiguratorBase *CommandBase = ISAlgorithm::CreatePointer<CGConfiguratorBase *>(ClassName);
	bool Result = ISSystem::CheckExistSlot(CommandBase, SubArgument);
	if (Result)
	{
		bool ReturnValue = true;
		ISTimePoint TimePoint = ISAlgorithm::GetTick();
		Result = QMetaObject::invokeMethod(CommandBase, SubArgument.toLocal8Bit().constData(), Q_RETURN_ARG(bool, ReturnValue));
		if (Result)
		{			
			if (ReturnValue)
			{
				ISLOGGER_L(QString("Command \"%1 %2\" executed with %3 msec").arg(Argument).arg(SubArgument).arg(ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint)));
			}
			else
			{
				ISLOGGER_E("", QString("Command \"%1 %2\" executed with error: %3").arg(Argument).arg(SubArgument).arg(CommandBase->GetErrorString()));
			}
		}
		else
		{
			ISLOGGER_E("", QString("Command \"%1 %2\" not executed.").arg(Argument).arg(SubArgument));
		}
		Result = ReturnValue;
	}
	else
	{
		ISLOGGER_E("", "Command \"" + SubArgument + "\" not found");
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
				ISLOGGER_E("", "Error getting database list: " + qSelect.GetErrorString());
			}
		}
		ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	}
	else
	{
		ISLOGGER_E("", "Not connected to system database: " + ISDatabase::Instance().GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ExistConfiguration(const QString &ConfigurationName, bool &Exist, QString &ErrorString)
{
	QFile File(PATH_CONFIGURATIONS_SCHEME);
	bool Result = File.open(QIODevice::ReadOnly); //Читаем файл с описанием конфигураций
	if (Result) //Файл успешно открыт
	{
		QString Content = File.readAll();
		File.close();

		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode DomNode = DomElement.firstChild();
		while (!DomNode.isNull()) //Обходим каждую конфигурацию и ищем нужную
		{
			QDomNamedNodeMap DomNamedNodeMap = DomNode.attributes();
			Exist = DomNamedNodeMap.namedItem("Name").nodeValue() == ConfigurationName;
			if (Exist) //Нашли - выходим из цикла поиска
			{
				break;
			}
			//Не нашли - переходим к следующей
			DomNode = DomNode.nextSibling();
		}
	}
	else //Не удалось открыть файл с описанием конфигураций
	{
		ErrorString = QString("Error open file %1: %2").arg(File.errorString()).arg(PATH_CONFIGURATIONS_SCHEME);
	}
	return Result;
}
//-----------------------------------------------------------------------------
