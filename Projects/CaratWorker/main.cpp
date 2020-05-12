#include "ISTcpServerWorker.h"
#include "ISCore.h"
#include "ISLogger.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QCoreApplication Application(argc, argv);
	
	QString ErrorString;
	bool Result = ISCore::Startup(false, ErrorString);
	if (!Result)
	{
		ISLOGGER_E(ErrorString);
		return EXIT_FAILURE;
	}

	Result = ISDatabase::Instance().Connect(CONNECTION_DEFAULT, "127.0.0.1", 5432, "oilsphere_db", "postgres", "adm777");
	if (!Result)
	{
		ISLOGGER_E(ISDatabase::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	Result = argc == 2;
	if (Result) //���� ������
	{
		int Port = std::atoi(argv[1]);
		Result = Port > 0;
		if (Result) //���� ��������
		{
			ISTcpServerWorker TcpServerWorker;
			Result = TcpServerWorker.Run(static_cast<quint16>(Port));
			if (Result) //�� ������� ��������� ������
			{
				Result = Application.exec() == EXIT_SUCCESS ? true : false;
			}
			else
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
		ISLOGGER_E("Port not specified");
	}
	return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
