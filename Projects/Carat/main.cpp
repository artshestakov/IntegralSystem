#include "ISCaratApplication.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISProperty.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	ISCaratApplication CaratApplication(argc, argv);
    int ResultCode = 0;

    //�������� ��������� ������� � ������� ������ (���� � ������������ �����)
	std::vector<std::string> Arguments = ISAlgorithm::ParseCommandArgs(argc, argv);
	ISAlgorithm::VectorRemoveFirst(Arguments);
    if (Arguments.empty()) //��������� ������� �� ����, ����������� � ������ �������
    {
        if (CaratApplication.Run())
        {
            ISLOGGER_I("", "Started application");
			PROPERTY_SET("Uptime", QDateTime::currentDateTime());
            ResultCode = CaratApplication.exec();

			//������������� ��������� ������� � ��������� ���������
			ISDatabase::Instance().DisconnectAll();
			ISLOGGER_I("", "Stopped application");
			ISLogger::Instance().Shutdown();
        }
        else
        {
            CaratApplication.Shutdown();
#ifdef DEBUG //����������� ����� ������� � �������
			system("PAUSE");
#endif
        }
    }
    else //���� ��������� ������� ����
    {
        ResultCode = CaratApplication.Run(Arguments) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    return ResultCode;
}
//-----------------------------------------------------------------------------
