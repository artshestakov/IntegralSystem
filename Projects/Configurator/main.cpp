#include "ISNamespace.h"
#include "ISConfig.h"
#include "ISMetaData.h"
#include "CGConsole.h"
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
bool InitConfiguratorScheme(QString &ErrorString); //������������� ����� �������������
bool CreateDatabase(); //�������� ������������� ��
void InterpreterMode(bool &IsRunning); //����� ��������������
bool Execute(const QString &Argument); //��������� ��������� �������
bool Execute(const QString &Argument, const QString &SubArgument); //��������� ������� �������
QString GetClassName(const QString &Argument); //�������� ��� ������
QStringList ParseInputCommand(const QString &Command); //������� ��������� �������
void FillConfig(); //���������� ����������������� �����
bool GetDatabaseList(QStringList &StringList); //�������� ������ ��� �� �������
bool ExistConfiguration(const QString &ConfigurationName, bool &Exist, QString &ErrorString); //�������� ������� ������ ������������
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //����������� ����-�����
    qRegisterMetaType<CGConfiguratorCreate*>("CGConfiguratorCreate");
    qRegisterMetaType<CGConfiguratorUpdate*>("CGConfiguratorUpdate");
    qRegisterMetaType<CGConfiguratorDelete*>("CGConfiguratorDelete");
    qRegisterMetaType<CGConfiguratorService*>("CGConfiguratorService");
    qRegisterMetaType<CGConfiguratorShow*>("CGConfiguratorShow");

	QCoreApplication CoreArralication(argc, argv);

	//������ ����������������� �����
	if (!ISConfig::Instance().Initialize("Server"))
	{
		ISDEBUG_E("Error init config file: " + ISConfig::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	//�������� ���������� QT
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

	FillConfig();
	if (!CreateDatabase())
	{
		return EXIT_FAILURE;
	}

	QString ConfigurationName = ISMetaDataHelper::GetConfigurationName(ErrorString);
	if (ConfigurationName.isEmpty())
	{
		ISDEBUG_E("Error getting configuration name: " + ErrorString);
		return EXIT_FAILURE;
	}

	if (!ISMetaData::Instance().Initialize(ConfigurationName, true, true))
	{
		ISDEBUG_E("Initialize meta data: " + ISMetaData::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	bool Result = true;
	ISVectorString ArgumentsCMD = CoreArralication.arguments().toVector().toStdVector();
	ArgumentsCMD.erase(ArgumentsCMD.begin());
	if (ArgumentsCMD.size() == 0)
	{
		ISDEBUG_L(QString("Configurator [Version %1] %2 %3").arg(ISVersionInfo::Instance().ToString()).arg(ISVersionInfo::Instance().Info.Configuration).arg(ISVersionInfo::Instance().Info.Platform));
		ISDEBUG_L("Welcome to the Configurator.");
		ISDEBUG_L("DBHost: " + DBHost);
		ISDEBUG_L("DBName: " + DBName);
		ISDEBUG_L("DBPort: " + QString::number(DBPort));
		ISDEBUG_L("Configuration: " + ConfigurationName);
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

	//����������� �� �� � �������
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
		ISDEBUG_E(QString("Not connected to system database \"%1\": %2").arg(SYSTEM_DATABASE_NAME).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	Result = ISDatabase::Instance().CheckExistDatabase(CONNECTION_SYSTEM, DBName, Exist);
	if (!Result) //������� ��������� ������� ��
	{
		ISDEBUG_E(QString("Checking exist database \"%1\": %2").arg(DBName).arg(ISDatabase::Instance().GetErrorString()));
		return Result;
	}

	if (Exist) //�� ���������� - ������������ � ������������
	{
		Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
	}
	else //�� �� ���������� - ������ �
	{
		//���������� � �����������, ��������� ��?
		Result = CGConsole::Question("Database \"" + DBName + "\" not exist! Create?");
		if (Result) //������� ��
		{
			QSqlError SqlError = ISDatabase::Instance().GetDB(CONNECTION_SYSTEM).exec(QC_DATABASE.arg(DBName).arg(DBLogin)).lastError(); //���������� ������� �� �������� ���� ������
			Result = SqlError.type() == QSqlError::NoError;
			Result ?
				ISDEBUG_L("The \"" + DBName + "\" database was created successfully. It is recommended that you run the \"update database\" command") :
				ISDEBUG_L("Error creating database \"" + DBName + "\": " + SqlError.databaseText());
			if (Result) //���� �� ���� ������� - ������������ � ���
			{
				Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, DBHost, DBPort, DBName, DBLogin, DBPassword);
			}
		}
		else //������������ ���������
		{
			ISDEBUG_W("You have refused to create a database");
		}
	}

	//����������� �� ��������� ��
	ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	
	if (!Result) //�� �����-�� �� ������ �������� �� �������� ������ - �������
	{
		return Result;
	}

	//����������� �������� ���� ������
	ISQuery qSelectDescriptionDB(QS_DATABASE_DESCRIPTION);
	qSelectDescriptionDB.SetShowLongQuery(false);
	Result = qSelectDescriptionDB.Execute();
	if (!Result)
	{
		ISDEBUG_E("Not getting database description: " + qSelectDescriptionDB.GetErrorString());
		return Result;
	}

	QString ConfigurationName; //��� ������������
	if (!qSelectDescriptionDB.First()) //�� ������� ������� �� ������ ������ ������� - ����������� ��� ������������
	{
		while (true)
		{
			//����������� �������� ������������
			ConfigurationName = CGConsole::GetString("Input configuration name (from file Configuration.xml): ");
			Result = !ConfigurationName.isEmpty();
			if (!Result) //���� �������� �� ����� - ������� � �������
			{
				ISDEBUG_W("Configuration name is empty.");
				return Result;
			}

			//��������� ������� ����� ������������
			QString ErrorString;
			Result = ExistConfiguration(ConfigurationName, Exist, ErrorString);
			if (!Result) //�� ������� ��������� ������� ����� ������������
			{
				ISDEBUG_W("Not checking configuration name: " + ErrorString);
				return Result;
			}

			if (Exist) //���� ������������ ������� - ������� �� ����� � ��������� � �������� �������
			{
				break;
			}
			else //������������ �� ������� - ��������� � ��������� �������� �����
			{
				ISDEBUG_W("Not found configuration: " + ConfigurationName);
			}
		}

		//������������ ��
		ISQuery qCommentDB;
		Result = qCommentDB.Execute(QString("COMMENT ON DATABASE %1 IS '%2'").arg(DBName).arg(QString(ISSystem::VariantMapToJsonString(
		{
			{ "ConfigurationName", ConfigurationName }
		}))));
		if (!Result)
		{
			ISDEBUG_L("Not updating database description: " + qCommentDB.GetErrorString());
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
void InterpreterMode(bool &IsRunning)
{
	ISDEBUG();
	QString Command = CGConsole::GetString("Enter command (press Enter or Return to exit): ");
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
void FillConfig()
{
	//�������� ������
	DBHost = CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER);
	while (DBHost.isEmpty())
	{
		DBHost = CGConsole::GetString("Enter the host: ");
		if (DBHost.isEmpty())
		{
			ISDEBUG_L("Host is empty!");
		}
	}

	//�������� ����
	DBPort = CONFIG_INT(CONST_CONFIG_CONNECTION_PORT);
	while (!DBPort)
	{
		DBPort = CGConsole::GetInt("Enter the port: ");
		if (!DBPort)
		{
			ISDEBUG_L("Port is empty or null!");
		}
	}

	//�������� �����
	DBLogin = CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN);
	while (DBLogin.isEmpty())
	{
		DBLogin = CGConsole::GetString("Enter the login: ");
		if (DBLogin.isEmpty())
		{
			ISDEBUG_L("Login is empty!");
		}
	}

	//�������� ������
	DBPassword = CONFIG_STRING(CONST_CONFIG_CONNECTION_PASSWORD);
	while (DBPassword.isEmpty())
	{
		DBPassword = CGConsole::GetString("Enter the password: ");
		if (DBPassword.isEmpty())
		{
			ISDEBUG_L("Password is empty!");
		}
	}

	//�������� ��� ��
	DBName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	if (DBName.isEmpty()) //���� ���� ������ �� �������
	{
		if (CGConsole::Question("Show database list?")) //���������� �������� ������ ���� ��� �� �������
		{
			QStringList StringList;
			if (GetDatabaseList(StringList)) //���� ������� ��������� ������ ��� - ������� ��� � �������
			{
				for (int i = 0; i < StringList.size(); ++i)
				{
					ISDEBUG_L(QString("%1. %2").arg(i + 1).arg(StringList[i]));
				}
			}
		}

		while (DBName.isEmpty())
		{
			DBName = CGConsole::GetString("Enter the database name: ");
			if (DBName.isEmpty())
			{
				ISDEBUG_L("Database name is empty!");
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
				ISDEBUG_E("Error getting database list: " + qSelect.GetErrorString());
			}
		}
		ISDatabase::Instance().Disconnect(CONNECTION_SYSTEM);
	}
	else
	{
		ISDEBUG_E("Not connected to system database: " + ISDatabase::Instance().GetErrorString());
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ExistConfiguration(const QString &ConfigurationName, bool &Exist, QString &ErrorString)
{
	QFile File(PATH_CONFIGURATIONS_SCHEME);
	bool Result = File.open(QIODevice::ReadOnly); //������ ���� � ��������� ������������
	if (Result) //���� ������� ������
	{
		QString Content = File.readAll();
		File.close();

		QDomElement DomElement = ISSystem::GetDomElement(Content);
		QDomNode DomNode = DomElement.firstChild();
		while (!DomNode.isNull()) //������� ������ ������������ � ���� ������
		{
			QDomNamedNodeMap DomNamedNodeMap = DomNode.attributes();
			Exist = DomNamedNodeMap.namedItem("Name").nodeValue() == ConfigurationName;
			if (Exist) //����� - ������� �� ����� ������
			{
				break;
			}
			//�� ����� - ��������� � ���������
			DomNode = DomNode.nextSibling();
		}
	}
	else //�� ������� ������� ���� � ��������� ������������
	{
		ErrorString = QString("Error open file %1: %2").arg(File.errorString()).arg(PATH_CONFIGURATIONS_SCHEME);
	}
	return Result;
}
//-----------------------------------------------------------------------------
