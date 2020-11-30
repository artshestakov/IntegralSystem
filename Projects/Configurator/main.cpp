#include "ISNamespace.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISConsole.h"
#include "ISDatabase.h"
#include "ISCore.h"
#include "ISSystem.h"
#include "ISDebug.h"
#include "ISMetaDataHelper.h"
#include "ISVersionInfo.h"
#include "ISQuery.h"
#include "ISAlgorithm.h"
#include "ISLocalization.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
#include "CGConfigurator.h"
#include "CGConfiguratorCreate.h"
#include "CGConfiguratorUpdate.h"
#include "CGConfiguratorDelete.h"
#include "CGConfiguratorService.h"
#include "CGConfiguratorShow.h"
//-----------------------------------------------------------------------------
static QString QC_DATABASE = "CREATE DATABASE %1 WITH OWNER = %2 ENCODING = 'UTF8'";
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

	//Чтение конфигурационного файла
	if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_SERVER))
	{
		ISDEBUG_E("Error init config file: " + ISConfig::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	//Проверяем валидность конфигурационного файла
	if (!ISConfig::Instance().IsValid())
	{
		ISDEBUG_E("Config file is not valid: " + ISConfig::Instance().GetErrorString());
		return false;
	}

	DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	DBLogin = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);

	//Загрузка трансляций QT
	if (!ISLocalization::Instance().LoadTraslatorQT())
	{
		ISDEBUG_W("Not load translator: " + ISLocalization::Instance().GetErrorString());
	}

	QString ErrorString;
	if (!InitConfiguratorScheme(ErrorString))
	{
		ISDEBUG_E("Error init configurator schema: " + ErrorString);
		return EXIT_FAILURE;
	}

	if (!CreateDatabase())
	{
		return EXIT_FAILURE;
	}

	if (!ISMetaData::Instance().Initialize(CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION), true, true))
	{
		ISDEBUG_E("Initialize meta data: " + ISMetaData::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	bool Result = true;
	ISVectorString ArgumentsCMD = CoreArralication.arguments().toVector().toStdVector();
	ArgumentsCMD.erase(ArgumentsCMD.begin());
	if (ArgumentsCMD.size() == 0)
	{
		ISDEBUG_L(QString("Configurator [Version %1] %2 %3").arg(ISVersionInfo::Instance().ToStringVersion()).arg(ISVersionInfo::Instance().Info.Configuration).arg(ISVersionInfo::Instance().Info.Platform));
		ISDEBUG_L("Welcome to the Configurator.");
		ISDEBUG_L("DBHost: " + DBHost);
		ISDEBUG_L("DBName: " + DBName);
		ISDEBUG_L("DBPort: " + QString::number(DBPort));
		ISDEBUG_L("Configuration: " + CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION));
		ISDEBUG_L("Enter the \"help\" command to get help");
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

	//Отключаемся от БД и выходим
	ISDatabase::Instance().DisconnectAll();
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
		ISDEBUG_E(QString("Not connected to system database \"%1\": %2").arg(SYSTEM_DATABASE_NAME).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	Result = ISDatabase::Instance().CheckExistDatabase(CONNECTION_SYSTEM, DBName, Exist);
	if (Result) //Проверка прошла успешно - анализируем
	{
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
				Result ?
					ISDEBUG_L("The \"" + DBName + "\" database was created successfully. It is recommended that you run the \"update database\" command") :
					ISDEBUG_L("Error creating database \"" + DBName + "\": " + SqlError.databaseText());
				if (Result) //Если БД была создана - подключаемся к ней
				{
					Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
				}
			}
			else //Пользователь отказался
			{
				ISDEBUG_W("You have refused to create a database");
			}
		}
	}
	else //Не удалось проверить наличие БД
	{
		ISDEBUG_E(QString("Checking exist database \"%1\": %2").arg(DBName).arg(ISDatabase::Instance().GetErrorString()));
	}

	//Отключаемся от системной БД и выходим
	ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	return Result;
}
//-----------------------------------------------------------------------------
void InterpreterMode(bool &IsRunning)
{
	ISDEBUG();
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
			ISDEBUG_L("command not found");
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
			ReturnValue ?
				ISDEBUG_L("Command \"" + Argument + "\" executed with " + QString::number(ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint)) + " msec") :
				ISDEBUG_L("Command \"" + Argument + "\" executed with error " + Configurator.GetErrorString());
		}
		else
		{
			ISDEBUG_E("Command \"" + Argument + "\" not executed.");
		}
		Result = ReturnValue;
	}
	else
	{
		ISDEBUG_L("Command \"" + Argument + "\" not found");
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool Execute(const QString &Argument, const QString &SubArgument)
{
	QString ClassName = GetClassName(Argument);
	if (ClassName.isEmpty())
	{
		ISDEBUG_E("Not found class name with argument: " + Argument);
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
				ISDEBUG_L(QString("Command \"%1 %2\" executed with %3 msec").arg(Argument).arg(SubArgument).arg(ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint)));
			}
			else
			{
				ISDEBUG_E(QString("Command \"%1 %2\" executed with error: %3").arg(Argument).arg(SubArgument).arg(CommandBase->GetErrorString()));
			}
		}
		else
		{
			ISDEBUG_E(QString("Command \"%1 %2\" not executed.").arg(Argument).arg(SubArgument));
		}
		Result = ReturnValue;
	}
	else
	{
		ISDEBUG_E("Command \"" + SubArgument + "\" not found");
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
