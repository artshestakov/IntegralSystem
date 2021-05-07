#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISConsole.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    ISCaratApplication Carat(argc, argv);
    bool Result = Carat.Init();
    if (Result) //���������� ������� ������������������ - ��������
    {
        Result = Carat.Start() == EXIT_SUCCESS;
    }
    //��������� ������
    ISLogger::Instance().Shutdown();

    //��������, ����� �� ��� ������������ ����� �������
    if (ISAlgorithm::IsDebug() && ISAlgorithm::IsWin32() && Carat.GetServiceMode())
    {
        ISConsole::Pause();
    }
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
