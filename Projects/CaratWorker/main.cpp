#include "ISTcpServerWorker.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISDatabase.h"
#include "ISConfig.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
quint16 Port = 0; //�������������� ����
const char *Login = NULL; //�����
const char *Password = NULL; //������
//-----------------------------------------------------------------------------
bool ParseArgs(char **argv); //������� ���������� ���������� ������
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	QCoreApplication Application(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, "Server", ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		return EXIT_FAILURE;
	}

    Result = ParseArgs(argv);
	if (!Result)
	{
		return EXIT_FAILURE;
	}

	Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
		CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
		argv[2], argv[3]);
	if (!Result) //�� ������� ������������ � ��
	{
		ISLOGGER_E(ISDatabase::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	ISTcpServerWorker TcpServerWorker;
	Result = TcpServerWorker.Run(static_cast<quint16>(Port));
	if (Result) //������ ������� - �������� �� ���� ������
	{
		ISLOGGER_I(QString("Worker is started. Port: %1 Login: %2 Password: %3").arg(Port).arg(Login).arg(Password));
		Result = Application.exec() == EXIT_SUCCESS ? true : false;
	}
	else //�� ������� ��������� ������
	{
		ISLOGGER_E(TcpServerWorker.GetErrorString());
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
bool ParseArgs(char **argv)
{
	//�������� �����
	if (!argv[1])
	{
		ISLOGGER_E("Port not specified");
		return false;
	}

	Port = std::atoi(argv[1]);
	if (Port == 0)
	{
		ISLOGGER_E("Port invalid value");
		return false;
	}

	//�������� ������
	Login = argv[2];
	if (!Login)
	{
		ISLOGGER_E("Login not specified");
		return false;
	}

	//�������� ������
	Password = argv[3];
	if (!Password)
	{
		ISLOGGER_E("Password not specified");
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
