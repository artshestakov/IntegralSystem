#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
class ISProcess : public QObject
{
	Q_OBJECT

signals:
	void Message(const QString &Message);
	void Finished();
	void Delete(QObject *Object);

public:
	ISProcess(QObject *parent = 0);
	virtual ~ISProcess();

	void SetProgram(const QString &Path);
	void SetArguments(const QStringList &Arguments);
	void SetProcessEnvironment(const QProcessEnvironment &ProcessEnvironment);

	void Start();
	void Start(const QString &ProgrammPath, const QStringList &Arguments = QStringList());
	
	void Kill();

protected slots:
	void OnFinished(int ExitCode);

protected:
	void OnReadyReadStandartError();
	void OnReadyReadStandartOutput();
	void OnStarted();
	void OnStateChanged(QProcess::ProcessState State);
	void OnErrorOccured(QProcess::ProcessError Error);
	void OnDelete();

	QString ParceArguments(const QStringList &Arguments) const;

private:
	QProcess *Process;
	ISCountingTime TimeWorking;
};
//-----------------------------------------------------------------------------
