#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISServiceController
{
public:
	ISServiceController(const QString &service_name);
	~ISServiceController();

	QString GetErrorString() const; //�������� �������� ��������� ������
	bool Start(); //������ ������
	bool Stop(); //��������� ������
	bool IsExist(); //������� ������
	bool IsRunning(bool &IsRunning); //�������� ���������� ������

private:
	bool OpenDB(unsigned long Access);
	void CloseDB();

private:
	QString ErrorString;
	char *ServiceName;

	SC_HANDLE SCManager;
	SC_HANDLE SCService;
};
//-----------------------------------------------------------------------------
