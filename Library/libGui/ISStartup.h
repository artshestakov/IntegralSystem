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

	static bool Startup();
	static void Shutdown();

private:
	static bool CheckAccessDatabase(); //�������� ������� � ����
	static bool CheckAccessAllowed(); //�������� ���������� ������� ������������
	static bool CheckExistUserGroup(); //�������� ������� �������� ������������ � ������
	static bool LoadLocalNames(); //�������� ��������������� ���� �����
};
//-----------------------------------------------------------------------------
