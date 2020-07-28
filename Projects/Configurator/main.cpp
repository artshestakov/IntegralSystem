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
void RegisterMetatype(); //����������� ����-�����
bool InitConfiguratorScheme(QString &ErrorString); //������������� ����� �������������
bool CreateDatabase(); //�������� ������������� ��
void InterpreterMode(bool &IsRunning); //����� ��������������
bool Execute(const QString &Argument); //��������� ��������� �������
bool Execute(const QString &Argument, const QString &SubArgument); //��������� ������� �������
QString GetClassName(const QString &Argument); //�������� ��� ������
QStringList ParseInputCommand(const QString &Command); //������� ��������� �������
void FillConfig(); //���������� ����������������� �����
bool GetDatabaseList(QStringList &StringList); //�������� ������ ��� �� �������
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

	//�������� ���������� QT
	if (!ISLocalization::Instance().LoadTraslatorQT())
	{
		ISLOGGER_W(ISLocalization::Instance().GetErrorString());
	}

	//�������� ����������� ����
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
		while (!NodeSections.isNull()) //����� ��������
		{
			CGSection *Section = new CGSection
			{
				NodeSections.attributes().namedItem("Name").nodeValue(),
				NodeSections.attributes().namedItem("ClassName").nodeValue(),
				std::vector<CGSectionItem*>()
			};
			Arguments.emplace_back(Section);

			QDomNode NodeFunctions = NodeSections.firstChild();
			while (!NodeFunctions.isNull()) //����� �����������
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
	if (!Result) //�� ������� ������������ � ��������� ��
	{
		ISLOGGER_E(QString("Not connected to system database \"%1\": %2").arg(SYSTEM_DATABASE_NAME).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	Result = ISDatabase::Instance().CheckExistDatabase(CONNECTION_SYSTEM, DBName, Exist);
	if (!Result) //������� ��������� ������� ��
	{
		ISLOGGER_E(QString("Checking exist database \"%1\": %2").arg(DBName).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	if (Exist) //�� ���������� - ������������ � ������������
	{
		Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
	}
	else //�� �� ���������� - ������ �
	{
		//���������� � �����������, ��������� ��?
		Result = ISConsole::Question("Database \"" + DBName + "\" not exist! Create?");
		if (Result) //������� ��
		{
			QSqlError SqlError = ISDatabase::Instance().GetDB(CONNECTION_SYSTEM).exec(QC_DATABASE.arg(DBName).arg(DBLogin)).lastError(); //���������� ������� �� �������� ���� ������
			Result = SqlError.type() == QSqlError::NoError;
			Result ? ISLOGGER_L("The \"" + DBName + "\" database was created successfully. It is recommended that you run the \"update database\" command")
				: ISLOGGER_L("Error creating database \"" + DBName + "\": " + SqlError.databaseText());
			if (Result) //���� �� ���� ������� - ������������ � ���
			{
				Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
			}
		}
		else //������������ ���������
		{
			ISLOGGER_W("You have refused to create a database");
		}
	}

	//����������� �� ��������� ��
	ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	
	if (!Result) //�� �����-�� �� ������ �������� �� �������� ������ - �������
	{
		return Result;
	}

	//��������� ������� ������� ��� ��������� ������������ ����
	ISQuery qFunction(QS_FUNCTION);
	qFunction.SetShowLongQuery(false);
	Result = qFunction.ExecuteFirst();
	if (!Result)
	{
		ISLOGGER_E("Not checked configuration function: " + qFunction.GetErrorString());
		return Result;
	}

	if (!qFunction.ReadColumn("count").toBool()) //���� ������� �� ���������� - ����������� ��� ������������ � ������ �������
	{
		//����������� �������� ������������
		QString ConfigurationName = ISConsole::GetString("Input configuration name (from file Configuration.xml): ");
		Result = !ConfigurationName.isEmpty();
		if (!Result) //���� �������� �� ����� - ������� � �������
		{
			ISLOGGER_W("Configuration name is empty.");
			return Result;
		}

		//������ �������
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
			if (StringList.front().toLower() == "exit" || StringList.front() == "quit") //�����
			{
				IsRunning = false;
				return;
			}
			else if (StringList.front() == "restart") //����������
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
	//�������� ������
	DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	while (DBHost.isEmpty())
	{
		DBHost = ISConsole::GetString("Enter the host: ");
		if (DBHost.isEmpty())
		{
			ISLOGGER_L("Host is empty!");
		}
	}

	//�������� ����
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	while (!DBPort)
	{
		DBPort = ISConsole::GetInt("Enter the port: ");
		if (!DBPort)
		{
			ISLOGGER_L("Port is empty or null!");
		}
	}

	//�������� �����
	DBLogin = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	while (DBLogin.isEmpty())
	{
		DBLogin = ISConsole::GetString("Enter the login: ");
		if (DBLogin.isEmpty())
		{
			ISLOGGER_L("Login is empty!");
		}
	}

	//�������� ������
	DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	while (DBPassword.isEmpty())
	{
		DBPassword = ISConsole::GetString("Enter the password: ");
		if (DBPassword.isEmpty())
		{
			ISLOGGER_L("Password is empty!");
		}
	}

	//�������� ��� ��
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	if (DBName.isEmpty()) //���� ���� ������ �� �������
	{
		if (ISConsole::Question("Show database list?")) //���������� �������� ������ ���� ��� �� �������
		{
			QStringList StringList;
			if (GetDatabaseList(StringList)) //���� ������� ��������� ������ ��� - ������� ��� � �������
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
