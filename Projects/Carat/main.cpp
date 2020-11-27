#include "ISCaratApplication.h"
#include "ISQueryPool.h"
#include "ISDatabase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	QObject::connect(&CaratApplication, &ISCaratApplication::Quit, &CaratApplication, &ISCaratApplication::quit, Qt::QueuedConnection);
	int ResultCode = CaratApplication.Initialize() ? EXIT_SUCCESS : EXIT_FAILURE;
	if (ResultCode == EXIT_SUCCESS) //������������� ������ �������
	{
		//�������� ��������� ������� � ������� ������ (���� � ������������ �����)
		QStringList Arguments = CaratApplication.arguments();
		Arguments.removeFirst();
		if (Arguments.isEmpty()) //��������� ������� �� ����, ����������� � ������ �������
		{
			if (CaratApplication.Run())
			{
				ISLOGGER_I("", "Started application");
				ResultCode = CaratApplication.exec();
			}
			else
			{
				CaratApplication.Shutdown();
			}
		}
		else //���� ��������� ������� ����
		{
			CaratApplication.Run(Arguments);
			return EXIT_SUCCESS;
		}
	}

	//������������� ��������� ������� � ��������� ���������
	ISQueryPool::Instance().Shutdown();
	ISDatabase::Instance().DisconnectAll();
	ISLOGGER_I("", "Stopped application");
	ISLogger::Instance().Shutdown();
	return ResultCode;
}
//-----------------------------------------------------------------------------
