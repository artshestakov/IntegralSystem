#include "StdAfx.h"
#include "CGConfigurator.h"
#include "ISNamespace.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISCommandLine.h"
#include "ISDatabase.h"
#include "ISCore.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISQueryException.h"
#include "CGSection.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
#include "ISLogger.h"
#include "ISMetaDataHelper.h"
//-----------------------------------------------------------------------------
#include "CGConfiguratorCreate.h"
#include "CGConfiguratorUpdate.h"
#include "CGConfiguratorDelete.h"
#include "CGConfiguratorService.h"
#include "CGConfiguratorShow.h"
#include "CGConfiguratorFIAS.h"
//-----------------------------------------------------------------------------
QVector<CGSection*> Arguments;
//-----------------------------------------------------------------------------
void RegisterMetatype(); //Регистрация мета-типов
bool InitConfiguratorScheme(QString &ErrorString); //Инициализация схемы конфигуратора
ISNamespace::ConsoleArgumentType CheckArguments(); //Проверка аргументов
std::pair<bool, bool> CheckExistDatabase(const QString &DBName); //Проверка существования БД
void InterpreterMode(bool &IsRunning); //Режим интерпретатора
bool Execute(const QString &Argument); //Выполнить одиночную команду
bool Execute(const QString &Argument, const QString &SubArgument); //Выполнить двойную команду
QString GetClassName(const QString &Argument); //Получить имя класса
void ProgressMessage(const QString &Message);
QStringList ParseInputCommand(const QString &Command); //Парсинг введенной команды
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
		return EXIT_FAILURE;
	}

	Result = InitConfiguratorScheme(ErrorString);
	if (!Result)
	{
		ISLOGGER_ERROR(ErrorString);
		return EXIT_FAILURE;
	}

	ISNamespace::ConsoleArgumentType ArgumentType = CheckArguments();
	if (ArgumentType == ISNamespace::CAT_Unknown)
	{
		ISLOGGER_WARNING("Invalid arguments");
		ISCommandLine::Pause();
		return EXIT_FAILURE;
	}

	//Параметры подключения
	QString DBServer = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	int DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	QString DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	QString DBLogin = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	QString DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);

	if (DBLogin.isEmpty() || DBPassword.isEmpty())
	{
		ISLOGGER_ERROR("Not specified server or password in config file");
		ISCommandLine::Pause();
		return EXIT_FAILURE;
	}

    std::pair<bool, bool> ResultExist = CheckExistDatabase(DBName);
	if (ResultExist.second) //Если БД существует - подключаемся к ней и инициализируем мета-данные
	{
        QString ErrorString;
		ResultExist.first = ISDatabase::GetInstance().ConnectToDefaultDB(DBLogin, DBPassword, ErrorString);
		if (ResultExist.first)
		{
			if (!ISMetaData::GetInstanse().Initialize(ISMetaDataHelper::GetConfigurationName(), true, true))
			{
				ISLOGGER_ERROR("Initialize meta data: " + ISMetaData::GetInstanse().GetErrorString());
			}
		}
	}

    if (!ResultExist.first)
    {
        ISCommandLine::Pause();
        return EXIT_FAILURE;
    }

	if (ArgumentType == ISNamespace::CAT_Interpreter)
	{
		ISLOGGER_EMPTY();
		ISLOGGER_UNKNOWN("Welcome to the IntegralSystem database. DBName: " + DBName + " DBHost: " + DBServer);
		ISLOGGER_UNKNOWN("Enter the \"help\" command to get help");
		ISLOGGER_UNKNOWN("Press Enter or Return to exit");
		
		if (!ResultExist.second)
		{
			ISLOGGER_WARNING("Database \"" + DBName + "\" not exist. Run the \"create database\" command.");
		}

		while (Result)
		{
			InterpreterMode(Result);
		}
		return EXIT_SUCCESS;
	}
	else if (ArgumentType == ISNamespace::CAT_OneArgument)
	{
		Result = Execute(CoreArralication.arguments().at(1).toLower());
	}
	else if (ArgumentType == ISNamespace::CAT_Standart)
	{
		Result = Execute(CoreArralication.arguments().at(1).toLower(), CoreArralication.arguments().at(2).toLower());
	}
	
	if (Result)
	{
		return EXIT_SUCCESS;
	}
	
	ISCommandLine::Pause();
	return EXIT_FAILURE;
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
			CGSection *Section = new CGSection();
			Section->SetName(NodeSections.attributes().namedItem("Name").nodeValue());
			Section->SetClassName(NodeSections.attributes().namedItem("ClassName").nodeValue());
			Arguments.append(Section);

			QDomNode NodeFunctions = NodeSections.firstChild();
			while (!NodeFunctions.isNull()) //Обход подразделов
			{
				CGSectionItem *SectionItem = new CGSectionItem();
				SectionItem->SetFunction(NodeFunctions.attributes().namedItem("FunctionName").nodeValue());
				SectionItem->SetLocalName(NodeFunctions.attributes().namedItem("FunctionLocalName").nodeValue());
				SectionItem->SetDescription(NodeFunctions.attributes().namedItem("Description").nodeValue());
				Section->AddItem(SectionItem);
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
ISNamespace::ConsoleArgumentType CheckArguments()
{
	switch (QCoreApplication::arguments().size())
	{
	case 1: return ISNamespace::CAT_Interpreter; break; //Интерпретатор
	case 2: return ISNamespace::CAT_OneArgument; break; //Один аргумент
	case 3: return ISNamespace::CAT_Standart; break; //Два аргумента
	}
	return ISNamespace::CAT_Unknown;
}
//-----------------------------------------------------------------------------
std::pair<bool, bool> CheckExistDatabase(const QString &DBName)
{
	QString ErrorString;
	bool Connected = false, Exist = false;
	Connected = ISDatabase::GetInstance().ConnectToSystemDB(ErrorString);
	if (Connected)
	{
		Exist = ISDatabase::GetInstance().CheckExistDatabase(DBName);
		ISDatabase::GetInstance().DisconnectFromSystemDB();
	}
	else
	{
		ISLOGGER_ERROR(ErrorString);
	}
	return std::make_pair(Connected, Exist);
}
//-----------------------------------------------------------------------------
void InterpreterMode(bool &IsRunning)
{
	ISLOGGER_EMPTY();
	ISLOGGER_UNKNOWN("Enter command: ");

	std::string InputCommand;
	std::getline(std::cin, InputCommand);
	QString Command = QString::fromStdString(InputCommand);
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
				QProcess::startDetached(DEFINES_CORE.PATH_APPLICATION_FILE);
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
		try
		{
			ISCountingTime CountingTime;
			Result = QMetaObject::invokeMethod(&Configurator, Argument.toUtf8().data());
			ISLOGGER_UNKNOWN("Command \"" + Argument + "\" executed with " + QString::number(CountingTime.GetElapsed()) + " msec");
		}
		catch (const ISQueryException &QueryException) {}
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
					QObject::connect(CommandBase, &CGConfiguratorBase::ProgressMessage, &ProgressMessage);
					try
					{
						ISCountingTime CountingTime;
						Result = QMetaObject::invokeMethod(CommandBase, SubArgument.toLocal8Bit().constData());
						ISLOGGER_INFO("Command \"" + Argument + " " + SubArgument + "\" executed with " + QString::number(CountingTime.GetElapsed()) + " msec");
					}
					catch (const ISQueryException &QueryException) {}
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
		if (Section->GetName().toLower() == Argument)
		{
			return Section->GetClassName();
		}
	}
	return QString();
}
//-----------------------------------------------------------------------------
void ProgressMessage(const QString &Message)
{
	ISLOGGER_UNKNOWN(Message);
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
