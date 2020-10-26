#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCaratController.h"
#include "ISTcpServer.h"
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
	Q_OBJECT

signals:
	void Quit();

public:
	ISCaratApplication(int argc, char **argv);
	virtual ~ISCaratApplication();

	QString GetErrorString() const; //�������� ��������� �������� ������
	bool Init(); //�������������
	bool Run(const QStringList &Arguments); //������ � �����������
	bool Run(); //����������� ������

private:
	void Shutdown();
	void Version();
	void Help();

private:
	QString ErrorString;
	ISCaratController *Controller;
	ISTcpServer *TcpServer;
};
//-----------------------------------------------------------------------------
#endif
