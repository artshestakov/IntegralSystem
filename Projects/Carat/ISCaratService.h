#pragma once
#ifndef _ISCARATSERVICE_H_INCLUDED
#define _ISCARATSERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratService : public QObject
{
	Q_OBJECT

public:
	ISCaratService(QObject *parent = 0);
	virtual ~ISCaratService();

	void StartService(); //������ �������

private:
	void Finished(int ExitCode, QProcess::ExitStatus Status);
	void Error(QProcess::ProcessError ErrorType);
	void ReadyReadStandartOutput();
	void OutputString(const QString &CoreObjectName, const QString &String);

private:
	void NewConnection(); //������� ����������� ������ ������� � ������ �������
	void Disconnected(); //������� ���������� ������� � ������ �������

private:
	QLocalServer *LocalServer;
	QLocalSocket *LocalSocket;
	bool IsConnectedDebugger;
};
//-----------------------------------------------------------------------------
#endif
