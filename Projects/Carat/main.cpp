#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISDatabase.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	QObject::connect(&CaratApplication, &ISCaratApplication::Quit, &CaratApplication, &ISCaratApplication::quit, Qt::QueuedConnection);
	if (!CaratApplication.Init()) //��� ������������� ������� ��������� ������ - �������
	{
		return EXIT_FAILURE;
	}

	//�������� ��������� ������� � ������� ������ (���� � ������������ �����)
	QStringList Arguments = CaratApplication.arguments();
	Arguments.removeFirst();

	if (!Arguments.isEmpty()) //���� ��������� ������� ����
	{
		return CaratApplication.Run(Arguments);
	}

	int ResultCode = -1;

	//��������� ������� �� ����, ����������� � ������ �������
	if (CaratApplication.Run())
	{
		ISLOGGER_I("", "Started application");
		ResultCode = CaratApplication.exec();
	}

	//������������� ��������� ������� � ��������� ���������
	ISDatabase::Instance().DisconnectAll();
	ISLOGGER_I("", "Stopped application");
	ISLogger::Instance().Shutdown();
	return ResultCode;
}
//-----------------------------------------------------------------------------
