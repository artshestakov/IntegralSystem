#include "ISCaratApplication.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISProperty.h"
//-----------------------------------------------------------------------------
void test()
{
	std::string conn_name = "LibPQ";
	if (ISDatabase::Instance().ConnectLibPQ(conn_name, "127.0.0.1", 5432, "oilsphere_db", "postgres", "adm777"))
	{
		ISQueryLibPQ qInsert(ISDatabase::Instance().GetDBLibPQ(conn_name), "SELECT prtc_datetime::DATE FROM _protocol ORDER BY prtc_id DESC LIMIT 1");
		if (qInsert.Execute())
		{
			while (qInsert.Next())
			{
				//ISVariant GroupID = qInsert.ReadColumn("usgp_id");
				//ISVariant GroupUID = qInsert.ReadColumn("usgp_uid");
				ISVariant GroupName = qInsert.ReadColumn(0);
				//std::cout << GroupID.ToString() << ' ' << GroupUID.ToString() << ' ' << GroupName.ToString() << std::endl;
			}
		}
		ISDatabase::Instance().DisconnectLibPQ(conn_name);
	}
}
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
