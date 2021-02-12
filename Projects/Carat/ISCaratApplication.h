#pragma once
#ifndef _ISCARATAPPLICATION_H_INCLUDED
#define _ISCARATAPPLICATION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpServer.h"
#include "ISAsterisk.h"
//-----------------------------------------------------------------------------
void QtLog(QtMsgType MessageType, const QMessageLogContext &MessageLogContext, const QString &Message);
//-----------------------------------------------------------------------------
class ISCaratApplication : public QCoreApplication
{
	Q_OBJECT

signals:
    void Stopped();

public:
    ISCaratApplication(int &argc, char **argv);
	virtual ~ISCaratApplication();

	QString GetErrorString() const; //�������� ��������� �������� ������
	bool Initialize(); //�������������
    bool Run(const QStringList &Arguments); //������ � �����������
	bool Run(); //����������� ������
	void Shutdown(); //��������� ����������

private:
    void StopController();
    bool Help();
    bool Version();
    bool SendShutdown();
    bool ConfigCreate();
    bool ConfigReset();

private:
	QString ErrorString;
	ISTcpServer *TcpServer;
	ISAsterisk *Asterisk;

    QString PathFileStop;
};
//-----------------------------------------------------------------------------
#endif
