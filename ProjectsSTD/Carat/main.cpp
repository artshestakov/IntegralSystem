#include "ISCaratApplication.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    ISCaratApplication CaratApplication(argc, argv);
    bool Result = CaratApplication.Init();
    if (Result) //���������� ������� ������������������
    {
        Result = CaratApplication.Start() == EXIT_SUCCESS;
    }
    else //��� ������������� �������� �������� - ��������� ������, ����� ������ ���������� � ���
    {
        ISLogger::Instance().Shutdown();
    }
#ifdef DEBUG
    system("PAUSE");
#endif
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
