#include "StdAfx.h"
#include "ISProcess.h"
#include "ISAssert.h"
#include "EXDefines.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISProcess::ISProcess(QObject *parent) : QObject(parent)
{
	Process = new QProcess(this);
	connect(Process, static_cast<void(QProcess::*)(int)>(&QProcess::finished), this, &ISProcess::OnFinished);
	connect(Process, &QProcess::readyReadStandardError, this, &ISProcess::OnReadyReadStandartError);
	connect(Process, &QProcess::readyReadStandardOutput, this, &ISProcess::OnReadyReadStandartOutput);
	connect(Process, &QProcess::started, this, &ISProcess::OnStarted);
	connect(Process, &QProcess::stateChanged, this, &ISProcess::OnStateChanged);
	connect(Process, &QProcess::errorOccurred, this, &ISProcess::OnErrorOccured);

	TimeWorking.Restart();
}
//-----------------------------------------------------------------------------
ISProcess::~ISProcess()
{

}
//-----------------------------------------------------------------------------
void ISProcess::SetProgram(const QString &Path)
{
	Process->setProgram(Path);
}
//-----------------------------------------------------------------------------
void ISProcess::SetArguments(const QStringList &Arguments)
{
	Process->setArguments(Arguments);
}
//-----------------------------------------------------------------------------
void ISProcess::SetProcessEnvironment(const QProcessEnvironment &ProcessEnvironment)
{
	Process->setProcessEnvironment(ProcessEnvironment);
}
//-----------------------------------------------------------------------------
void ISProcess::Start()
{
	Start(Process->program(), Process->arguments());
}
//-----------------------------------------------------------------------------
void ISProcess::Start(const QString &ProgrammPath, const QStringList &Arguments)
{
	QString Path = ProgrammPath;

	if (Path.contains("\""))
	{
		Path.replace("\"", QString());
	}

	IS_ASSERT(Path.length(), "Programm path is null.");
    IS_ASSERT(QFile::exists(Path), QString("Programm %1 not exist.").arg(Path));

	SetProgram(Path);

	if (Arguments.count())
	{
		Process->setArguments(Arguments);
		emit Message(QString("Start command %1 with arguments %2").arg(Path).arg(ParceArguments(Arguments)));
	}
	else
	{
		emit Message(QString("StartCommand: %1").arg(Path));
	}

	Process->start();
}
//-----------------------------------------------------------------------------
void ISProcess::Kill()
{
	Process->kill();
}
//-----------------------------------------------------------------------------
void ISProcess::OnFinished(int ExitCode)
{
	QString FinishString;

	if (ExitCode)
	{
		FinishString = "Process finish failure";
	}
	else
	{
		FinishString = "Process finish normal";
		emit Finished();
	}

	emit Message(FinishString);
	emit Message(QString("Time work command: %1").arg(ISSystem::MillisecondsToString(TimeWorking.GetElapsed())));
}
//-----------------------------------------------------------------------------
void ISProcess::OnReadyReadStandartError()
{
	QByteArray ByteArray = Process->readAllStandardError();
	QString StandartError = QString::fromLocal8Bit(ByteArray);
	emit Message(StandartError);
}
//-----------------------------------------------------------------------------
void ISProcess::OnReadyReadStandartOutput()
{
	QByteArray ByteArray = Process->readAllStandardOutput();
	QString StandartOutput = QString::fromLocal8Bit(ByteArray);
	emit Message(StandartOutput);
}
//-----------------------------------------------------------------------------
void ISProcess::OnStarted()
{
	emit Message("Process started");
}
//-----------------------------------------------------------------------------
void ISProcess::OnStateChanged(QProcess::ProcessState State)
{
	QString StateText;

	switch (State)
	{
	case QProcess::ProcessState::Starting: StateText = "Process starting"; break;
	case QProcess::ProcessState::Running: StateText = "Process running"; break;
	case QProcess::ProcessState::NotRunning: StateText = "Process not running"; break;
	}

	if (State == QProcess::Starting)
	{
		TimeWorking.Restart();
	}

	emit Message(StateText);
}
//-----------------------------------------------------------------------------
void ISProcess::OnErrorOccured(QProcess::ProcessError Error)
{
	QString ErrorText;

	switch (Error)
	{
	case QProcess::Crashed: ErrorText = "Process crashed"; break;
	case QProcess::FailedToStart: ErrorText = "Process failed to start"; break;
	case QProcess::ReadError: ErrorText = "Process read rrror"; break;
	case QProcess::WriteError: ErrorText = "Process write error"; break;
	case QProcess::Timedout: ErrorText = "Process timedout"; break;
	case QProcess::UnknownError: ErrorText = "Process unknown error"; break;
	}

	emit Message(ErrorText + SYMBOL_SPACE + Process->errorString());
}
//-----------------------------------------------------------------------------
void ISProcess::OnDelete()
{
	emit Delete(this);
}
//-----------------------------------------------------------------------------
QString ISProcess::ParceArguments(const QStringList &Arguments) const
{
	QString Result;

	for (const QString &Argument : Arguments)
	{
		Result += Argument + SYMBOL_SPACE;
	}

	ISSystem::RemoveLastSymbolFromString(Result);
	return Result;
}
//-----------------------------------------------------------------------------
