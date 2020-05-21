#include "ISTcpServerWorker.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISDatabase.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);

	QString ErrorString;
	bool Result = ISCore::Startup(false, "Server", ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		return EXIT_FAILURE;
	}

	Result = argc == 5;
	if (Result) //���� ������, ����� � ������ �������, ����� ���� ������
	{
		int Port = std::atoi(argv[1]);
		Result = Port > 0;
		if (Result) //���� ��������
		{
			Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT,
				CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER), CONFIG_INT(CONST_CONFIG_CONNECTION_PORT), CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE),
				argv[2], argv[3]);
			if (!Result) //�� ������� ������������ � ��
			{
				ISLOGGER_E(ISDatabase::Instance().GetErrorString());
				return EXIT_FAILURE;
			}

			ISTcpServerWorker TcpServerWorker;
			TcpServerWorker.SetToken(argv[4]);
			Result = TcpServerWorker.Run(static_cast<quint16>(Port));
			if (Result) //������ ������� - �������� �� ���� ������
			{
				ISLOGGER_I("Worker is started with port " + QString::number(Port));
				Result = Application.exec() == EXIT_SUCCESS ? true : false;
			}
			else //�� ������� ��������� ������
			{
				ISLOGGER_E(TcpServerWorker.GetErrorString());
			}
		}
		else //���� �� ��������
		{
			ISLOGGER_E(QString("Invalid port %1").arg(Port));
		}
	}
	else //���� �� ������
	{
		ISLOGGER_E("Invalid arguments");
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
