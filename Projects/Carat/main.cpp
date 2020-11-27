#include "ISCaratApplication.h"
#include "ISQueryPool.h"
#include "ISDatabase.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
	QObject::connect(&CaratApplication, &ISCaratApplication::Quit, &CaratApplication, &ISCaratApplication::quit, Qt::QueuedConnection);
	if (!CaratApplication.Initialize()) //��� ������������� ������� ��������� ������ - �������
	{
		return EXIT_FAILURE;
	}

	//�������� ��������� ������� � ������� ������ (���� � ������������ �����)
	QStringList Arguments = CaratApplication.arguments();
    Arguments.removeFirst();

    int ResultCode = -1;
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

	//������������� ��������� ������� � ��������� ���������
	ISQueryPool::Instance().Shutdown();
	ISDatabase::Instance().DisconnectAll();
	ISLOGGER_I("", "Stopped application");
	ISLogger::Instance().Shutdown();
	return ResultCode;
}
//-----------------------------------------------------------------------------
