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
    CMD.AddFlag("-h", "--help", "Show this help and exit");
    CMD.AddFlag("-v", "--version", "Show version and exit");
    CMD.AddFlag("-s", "--shutdown", "Shutdown service");
    CMD.AddFlag("-cc", "--config-create", "Create config");
    CMD.GetHelp();

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
        if (CMD.IsExist("-h") || CMD.IsExist("--help")) //�������� ������
        {
            //std::cout << CMD.GetHelp() << std::endl;

#ifdef WIN32
            std::cout << "Usage: Carat.exe [argument]" << std::endl;
#else
            std::cout << "Usage: ./Carat [argument]" << std::endl;
#endif
            std::cout << std::endl;
            std::cout << CMD.GetHelp() << std::endl;
            std::cout << std::endl;
#ifdef WIN32
            std::cout << "Example: Carat.exe (service mode)" << std::endl;
#else
            std::cout << "Example: ./Carat (service mode)" << std::endl;
#endif
            std::cout << "* No arguments needed to start in service mode" << std::endl;
        }
        else if (CMD.IsExist("-v") || CMD.IsExist("--version")) //�������� ������
        {
            Carat.ShowVersion();
        }
        else if (CMD.IsExist("-s") || CMD.IsExist("--shutdown")) //��������� ������
        {
            Carat.Shutdown();
        }
        else if (CMD.IsExist("-cc") || CMD.IsExist("--config-create")) //�������� ����������������� �����
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
