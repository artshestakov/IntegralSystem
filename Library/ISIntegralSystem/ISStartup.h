#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISStartup : public QObject
{
	Q_OBJECT

public:
	ISStartup();
	virtual ~ISStartup();

	static int Startup(const QString &UserLogin, const QString &UserPassword);

protected:
	static int ExitNormal(); //��������� ���������
	static int ExitError(); //��������� ��������� � �������
	static void SetApplicationNameQuery(); //�������� ������������ ���������� ����� ���������� � ��
	static bool CheckAlreadyConnected(); //��������� �� ������ �� ��� ������� ������������
	static bool CheckAccessDatabase(const QString &Login); //�������� ������� � ����
	static bool CheckAccessAllowed(); //�������� ���������� ������� ������������
	static bool CheckExistUserGroup(); //�������� ������� �������� ������������ � ������
};
//-----------------------------------------------------------------------------
