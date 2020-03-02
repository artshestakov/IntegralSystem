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
#include "ISQueryException.h"
#include "CGSection.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
#include "ISLogger.h"
#include "ISMetaDataHelper.h"
#include "ISConsole.h"
#include "ISVersion.h"
#include "ISQuery.h"
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
QVector<CGSection*> Arguments;
QString DBServer;
int DBPort = 0;
QString DBName;
QString DBLogin;
QString DBPassword;
//-----------------------------------------------------------------------------
void RegisterMetatype(); //����������� ����-�����
bool InitConfiguratorScheme(QString &ErrorString); //������������� ����� �������������
ISNamespace::ConsoleArgumentType CheckArguments(); //�������� ����������
bool CreateDatabase(const QString &DBName); //�������� ������������� ��
void InterpreterMode(bool &IsRunning); //����� ��������������
bool Execute(const QString &Argument); //��������� ��������� �������
bool Execute(const QString &Argument, const QString &SubArgument); //��������� ������� �������
QString GetClassName(const QString &Argument); //�������� ��� ������
void ProgressMessage(const QString &Message);
QStringList ParseInputCommand(const QString &Command); //������� ��������� �������
void FillConfig(); //���������� ����������������� �����
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

	ISNamespace::ConsoleArgumentType ArgumentType = CheckArguments();
	if (ArgumentType == ISNamespace::CAT_Unknown)
	{
		ISLOGGER_WARNING("Invalid arguments");
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

    Result = CreateDatabase(DBName);
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

	if (ArgumentType == ISNamespace::CAT_Interpreter)
	{
		ISLOGGER_UNKNOWN(QString("Configurator [Version %1]").arg(ISVersion::Instance().ToString()));
		ISLOGGER_UNKNOWN("Welcome to the Configurator. DBName: " + DBName + " DBHost: " + DBServer);
		ISLOGGER_UNKNOWN("Enter the \"help\" command to get help");
		
		while (Result)
		{
			InterpreterMode(Result);
		}
		ISLogger::Instance().Shutdown();
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
	
	ISConsole::Pause();
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
		while (!NodeSections.isNull()) //����� ��������
		{
			CGSection *Section = new CGSection();
			Section->SetName(NodeSections.attributes().namedItem("Name").nodeValue());
			Section->SetClassName(NodeSections.attributes().namedItem("ClassName").nodeValue());
			Arguments.append(Section);

			QDomNode NodeFunctions = NodeSections.firstChild();
			while (!NodeFunctions.isNull()) //����� �����������
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
	case 1: return ISNamespace::CAT_Interpreter; break; //�������������
	case 2: return ISNamespace::CAT_OneArgument; break; //���� ��������
	case 3: return ISNamespace::CAT_Standart; break; //��� ���������
	}
	return ISNamespace::CAT_Unknown;
}
//-----------------------------------------------------------------------------
bool CreateDatabase(const QString &DBName)
{
	QString ErrorString;
	bool Result = ISDatabase::GetInstance().ConnectToSystemDB(ErrorString);
	if (Result) //���� ������������ � ��������� �� - ��������� ������� ���, ��� ������� � ���������������� �����
	{
		if (ISDatabase::GetInstance().CheckExistDatabase(DBName)) //�� ���������� - ������������ � ���
		{
			Result = ISDatabase::GetInstance().ConnectToDefaultDB(DBLogin, DBPassword, ErrorString);
		}
		else //�� �� ���������� - ������ �
		{
			//���������� � �����������, ��������� ��?
			Result = ISConsole::Question("Database \"" + DBName + "\" not exist! Create?");
			if (Result) //������� ��
			{
				//����������� �������� ������������
				QString ConfigurationName = ISConsole::GetString("Input configuration name (from file Configuration.xml): ");
				Result = !ConfigurationName.isEmpty();
				if (!Result) //���� �������� �� ����� - ������� � �������
				{
					ISLOGGER_UNKNOWN("Configuration name is empty.");
				}

				if (Result)
				{
					QSqlQuery SqlQuery = ISDatabase::GetInstance().GetSystemDB().exec(QC_DATABASE.arg(DBName).arg(DBLogin)); //���������� ������� �� �������� ���� ������
					QSqlError SqlError = SqlQuery.lastError();
					Result = SqlError.type() == QSqlError::NoError;
					Result ? ISLOGGER_UNKNOWN("The \"" + DBName + "\" database was created successfully. It is recommended that you run the \"update database\" command")
						: ISLOGGER_UNKNOWN("Error creating database \"" + DBName + "\": " + SqlError.text());
				}
				if (Result) //���� �� ���� ������� - ������������ � ���
				{
					Result = ISDatabase::GetInstance().ConnectToDefaultDB(DBLogin, DBPassword, ErrorString);
				}
				if (Result) //���� ����������� ������ ������� - ������ �������
				{
					ISQuery qCreateFunction;
					Result = qCreateFunction.Execute("CREATE OR REPLACE FUNCTION get_configuration_name() RETURNS VARCHAR AS $$ BEGIN RETURN '" + ConfigurationName + "'; END; $$ LANGUAGE plpgsql IMMUTABLE");
					if (!Result)
					{
						ErrorString = qCreateFunction.GetErrorText();
					}
				}
			}
		}
		ISDatabase::GetInstance().DisconnectFromSystemDB();
	}
	else //������ ����������� � ��������� ��
	{
		ISLOGGER_ERROR(ErrorString);
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
			if (StringList.at(0).toLower() == "exit" || StringList.at(0) == "quit") //�����
			{
				IsRunning = false;
				return;
			}
			else if (StringList.at(0) == "restart") //����������
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
void FillConfig()
{
	//�������� ������
	DBServer = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	while (DBServer.isEmpty())
	{
		DBServer = ISConsole::GetString("Enter the host: ");
		if (DBServer.isEmpty())
		{
			ISLOGGER_UNKNOWN("Host is empty!");
		}
	}

	//�������� ����
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	while (!DBPort)
	{
		DBPort = ISConsole::GetInt("Enter the port: ");
		if (!DBPort)
		{
			ISLOGGER_UNKNOWN("Port is empty or null!");
		}
	}

	//�������� ��� ��
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	while (DBName.isEmpty())
	{
		DBName = ISConsole::GetString("Enter the database name: ");
		if (DBName.isEmpty())
		{
			ISLOGGER_UNKNOWN("Database name is empty!");
		}
	}

	//�������� �����
	DBLogin = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	while (DBLogin.isEmpty())
	{
		DBLogin = ISConsole::GetString("Enter the login: ");
		if (DBLogin.isEmpty())
		{
			ISLOGGER_UNKNOWN("Login is empty!");
		}
	}

	//�������� ������
	DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	while (DBPassword.isEmpty())
	{
		DBPassword = ISConsole::GetString("Enter the password: ");
		if (DBPassword.isEmpty())
		{
			ISLOGGER_UNKNOWN("Password is empty!");
		}
	}

	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_SERVER, DBServer);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PORT, DBPort);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_DATABASE, DBName);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_LOGIN, DBLogin);
	ISConfig::Instance().SetValue(CONST_CONFIG_CONNECTION_PASSWORD, DBPassword);
	ISConfig::Instance().SaveForce();
}
//-----------------------------------------------------------------------------
