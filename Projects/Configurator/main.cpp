#include "StdAfx.h"
#include "CGConfigurator.h"
#include "EXDefines.h"
#include "ISConstants.h"
#include "ISNamespace.h"
#include "ISLocalization.h"
#include "ISDebug.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "ISCommandLine.h"
#include "ISDatabase.h"
#include "ISLicense.h"
#include "ISCore.h"
#include "ISSystem.h"
#include "ISCountingTime.h"
#include "ISQueryException.h"
#include "CGSection.h"
//-----------------------------------------------------------------------------
#include "CGConfiguratorCreate.h"
#include "CGConfiguratorUpdate.h"
#include "CGConfiguratorDelete.h"
#include "CGConfiguratorService.h"
#include "CGConfiguratorLicense.h"
#include "CGConfiguratorShow.h"
#include "CGConfiguratorFIAS.h"
//-----------------------------------------------------------------------------
QVector<CGSection*> Arguments;
//-----------------------------------------------------------------------------
void RegisterMetatype(); //Регистрация мета-типов
bool InitConfiguratorScheme(QString &ErrorString); //Инициализация схемы конфигуратора
ISNamespace::ConsoleArgumentType CheckArguments(); //Проверка аргументов
bool CheckExistDatabase(const QString &DBName, bool &Connected); //Проверка существования БД
void InterpreterMode(bool &IsRunning);
bool Execute(const QString &Argument);
bool Execute(const QString &Argument, const QString &SubArgument);
QString GetClassName(const QString &Argument);
void ProgressMessage(const QString &Message);
QStringList ParseInputCommand(const QString &Command);
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	RegisterMetatype();
	QCoreApplication CoreArralication(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, ErrorString);
	if (!Result)
	{
		printf("%s\n", ErrorString.toStdString().c_str());
		return EXIT_FAILURE;
	}

	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CONFIGURATOR);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_INTEGRAL_SYSTEM);
	ISLocalization::GetInstance().LoadResourceFile(LOCALIZATION_FILE_CORE);

	Result = InitConfiguratorScheme(ErrorString);
	if (!Result)
	{
		printf("%s\n", ErrorString.toStdString().c_str());
		return EXIT_FAILURE;
	}

	ISNamespace::ConsoleArgumentType ArgumentType = CheckArguments();
	if (ArgumentType == ISNamespace::CAT_Unknown)
	{
		ISDebug::ShowString("Invalid arguments");
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
		ISDebug::ShowString("Not specified Server or Password in config file");
		ISCommandLine::Pause();
		return EXIT_FAILURE;
	}

    bool Connected = true;
    bool ExistDB = CheckExistDatabase(DBName, Connected);
	if (ExistDB)
	{
        QString ErrorString;
        if (ISDatabase::GetInstance().ConnectToDefaultDB(DBLogin, DBPassword, ErrorString))
		{
			if (!ISLicense::GetInstance().Initialize())
			{
				ISDebug::ShowWarningString(ISLicense::GetInstance().GetErrorString());
			}
		}
		else
		{
			Connected = false;
		}
	}

    if (!Connected)
    {
        ISCommandLine::Pause();
        return EXIT_FAILURE;
    }

	ISMetaData::GetInstanse().Initialize(ISLicense::GetInstance().GetName(), true, true);

	bool Executed = false;
	if (ArgumentType == ISNamespace::CAT_Interpreter)
	{
		ISDebug::ShowEmptyString();
		ISDebug::ShowString("Welcome to the IntegralSystem database. DBName: " + DBName + " DBHost: " + DBServer);
		ISDebug::ShowString("Enter the 'help' command to get help");
		ISDebug::ShowString("Press Enter or Return to exit");
		
		if (!ExistDB)
		{
			ISDebug::ShowString("ATTENTION! Database \"" + DBName + "\" not exist. Run the 'create database' command");
		}

		bool IsRunning = true;
		while (IsRunning)
		{
			InterpreterMode(IsRunning);
		}
		return EXIT_SUCCESS;
	}
	else if (ArgumentType == ISNamespace::CAT_OneArgument)
	{
		Executed = Execute(CoreArralication.arguments().at(1).toLower());
	}
	else if (ArgumentType == ISNamespace::CAT_Standart)
	{
		Executed = Execute(CoreArralication.arguments().at(1).toLower(), CoreArralication.arguments().at(2).toLower());
	}
	
	if (Executed)
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
	qRegisterMetaType<CGConfiguratorLicense*>("CGConfiguratorLicense");
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
bool CheckExistDatabase(const QString &DBName, bool &Connected)
{
	QString ErrorString;
	bool Exist = false;
	Connected = ISDatabase::GetInstance().ConnectToSystemDB(ErrorString);
	if (Connected)
	{
		Exist = ISDatabase::GetInstance().CheckExistDatabase(DBName);
		ISDatabase::GetInstance().DisconnectFromSystemDB();
	}
	else
	{
		ISDebug::ShowString(ErrorString);
	}
	return Exist;
}
//-----------------------------------------------------------------------------
void InterpreterMode(bool &IsRunning)
{
	ISDebug::ShowEmptyString();
	ISDebug::ShowString("Enter command: ");

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
				QProcess::startDetached(PATH_APPLICATION_FILE);
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
			ISDebug::ShowString("Command not found");
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
			ISDebug::ShowString("Command \"" + Argument + "\" executed with " + QString::number(CountingTime.GetElapsed()) + " msec");
		}
		catch (const ISQueryException &QueryException) {}
	}
	else
	{
		ISDebug::ShowString("Command \"" + Argument + "\" not found");
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
						ISDebug::ShowString("Command \"" + Argument + " " + SubArgument + "\" executed with " + QString::number(CountingTime.GetElapsed()) + " msec");
					}
					catch (const ISQueryException &QueryException) {}
				}
				else
				{
					ISDebug::ShowString("Command \"" + SubArgument + "\" not found");
				}
				delete CommandBase;
			}
			else
			{
				ISDebug::ShowString("Class \"" + Argument + "\" not found");
			}
		}
		else
		{
			ISDebug::ShowString("Class \"" + Argument + "\" not found");
		}
	}
	else
	{
		ISDebug::ShowString("Class \"" + Argument + "\" not found");
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
	ISDebug::ShowString(Message);
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
