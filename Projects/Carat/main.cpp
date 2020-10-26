#include "ISCaratApplication.h"
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

	//��������� ������� �� ����, ����������� � ������ �������
	if (!CaratApplication.Run())
	{
		return EXIT_FAILURE;
	}
	return CaratApplication.exec();
}
//-----------------------------------------------------------------------------
