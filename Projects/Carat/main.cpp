#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
#include "ISArguments.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //��������� ��������� ����������
    ISArguments CMD(argc, argv);
    CMD.AddFlag("-h", "Show this help and exit");
    CMD.AddFlag("-v", "Show version and exit");
    CMD.AddFlag("-s", "Shutdown service");
    CMD.AddFlag("-c", "Create config");

    ISCaratApplication Carat;

    bool Result = true;
    if (CMD.IsEmpty()) //���������� ��� - ����������� ��� ������
    {
        Result = Carat.Init();
        if (Result)
        {
            Result = Carat.Start() == EXIT_SUCCESS;
        }
    }
    else //��������� ���� - �����������
    {
        if (CMD.IsExist("-h")) //�������� ������
        {
            Carat.ShowHelp();
        }
        else if (CMD.IsExist("-v")) //�������� ������
        {
            Carat.ShowVersion();
        }
        else if (CMD.IsExist("-s")) //��������� ������
        {
            Carat.Shutdown();
        }
        else if (CMD.IsExist("-c")) //�������� ����������������� �����
        {
            Carat.ConfigCreate();
        }
    }

    //��������, ����� �� ��� ������������ ����� �������
    if (ISAlgorithm::IsDebug() && ISAlgorithm::IsWin32() && CMD.IsEmpty())
    {
        ISConsole::Pause();
    }
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
