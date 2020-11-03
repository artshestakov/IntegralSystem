#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISCaratController.h"
#include "ISTcpServer.h"
#include "ISAsterisk.h"
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
	Q_OBJECT

signals:
	void Quit();

public:
    ISCaratApplication(int &argc, char **argv);
	virtual ~ISCaratApplication();

	QString GetErrorString() const; //�������� ��������� �������� ������
	bool Init(); //�������������
	void Run(const QStringList &Arguments); //������ � �����������
	bool Run(); //����������� ������

private:
	void Shutdown();

	void Help();
	void Version();
	void SendShutdown();
	void ConfigCreate();
	void ConfigReset();

private:
	void SendCommand(const QByteArray &ByteArray);

private:
	QString ErrorString;
	ISCaratController *Controller;
	ISTcpServer *TcpServer;
	ISAsterisk *Asterisk;
};
//-----------------------------------------------------------------------------
#endif
