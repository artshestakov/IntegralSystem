#include "ISCaratApplication.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    ISCaratApplication CaratApplication(argc, argv);
    if (CaratApplication.Init()) //���������� ������� ������������������
    {
        return CaratApplication.Start();
    }
    else //��� ������������� �������� �������� - ��������� ������, ����� ������ ���������� � ���
    {
        ISLogger::Instance().Shutdown();
    }
#ifdef DEBUG
    system("PAUSE");
#endif
    return EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
