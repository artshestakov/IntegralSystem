#pragma once
//-----------------------------------------------------------------------------
#include "libGui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT	ISStartup : public QObject
{
	Q_OBJECT

public:
	ISStartup();
	virtual ~ISStartup();

	static bool Startup(const QString &UserLogin, const QString &UserPassword);
	static void Shutdown();

private:
	static bool CheckAccessDatabase(const QString &Login); //�������� ������� � ����
	static bool CheckAccessAllowed(); //�������� ���������� ������� ������������
	static bool CheckExistUserGroup(); //�������� ������� �������� ������������ � ������
	static bool LoadLocalNames(); //�������� ��������������� ���� �����
};
//-----------------------------------------------------------------------------
