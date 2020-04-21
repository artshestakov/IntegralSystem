#include "ISCaratService.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISSystem.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_name, core_filename "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "AND core_active "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(QObject *parent)
	: QObject(parent),
	LocalSocket(new QLocalSocket(this)),
	IsConnectedDebugger(false)
{
	
}
//-----------------------------------------------------------------------------
ISCaratService::~ISCaratService()
{
	
}
//-----------------------------------------------------------------------------
void ISCaratService::StartService()
{
	ISQuery qSelectCore(QS_CARAT_CORE);
	qSelectCore.SetShowLongQuery(false);
	if (qSelectCore.Execute())
	{
		//ќбщее количество €дер и количество запущенных
		int CoreCountTotal = qSelectCore.GetCountResultRows(), CoreCountStarted = 0;
		if (CoreCountTotal) //≈сли активные €дра существуют - запускаем
		{
			while (qSelectCore.Next()) //ќбход €дер
			{
				QString CoreName = qSelectCore.ReadColumn("core_name").toString();
				QString FileName = qSelectCore.ReadColumn("core_filename").toString() + EXTENSION_BINARY;

				ISLOGGER_I("Core \"" + CoreName + "\": starting...");
				QString CoreFilePath = ISDefines::Core::PATH_APPLICATION_DIR + '/' + FileName;
				if (!QFile::exists(CoreFilePath)) //≈сли €дро не существует - переходим к следующему
				{
					ISLOGGER_E("Core \"" + CoreName + "\" not found. Path: " + CoreFilePath);
					continue;
				}

				QProcess *Process = new QProcess(this);
				Process->setObjectName(CoreName);
				connect(Process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ISCaratService::Finished);
				connect(Process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), this, &ISCaratService::Error);
				connect(Process, &QProcess::readyReadStandardOutput, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				connect(Process, &QProcess::readyReadStandardError, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				Process->start(CoreFilePath);

				//≈сли дождались первого сообщени€ от €дра и оно валидное - считаем, что €дро успешно запустилось - иначе ошибка в любом случае
				if (Process->waitForReadyRead(CARAT_CORE_START_TIMEOUT) && Process->readAll().contains(CARAT_CORE_START_FLAG))
				{
					ISLOGGER_I("Core \"" + CoreName + "\" started. PID: " + QString::number(Process->processId()));
					++CoreCountStarted;
				}
				else
				{
					ISLOGGER_E("Core \"" + CoreName + "\" not started");
				}
			}
			CoreCountStarted == CoreCountTotal ?
				ISLOGGER_I("Started all cores") : //≈сли €дра были успешно запущены
				ISLOGGER_W("Started " + QString::number(CoreCountStarted) + " of " + QString::number(CoreCountTotal));
		}
		else
		{
			ISLOGGER_W("Active core not exist");
		}
	}

	LocalServer = new QLocalServer(this);
	LocalServer->setMaxPendingConnections(CARAT_DEBUGGER_MAX_CLIENTS);
	if (LocalServer->listen(CARAT_DEBUGGER_PORT))
	{
		connect(LocalServer, &QLocalServer::newConnection, this, &ISCaratService::NewConnection);
	}
	else
	{
		ISLOGGER_W("Not listen port for local server");
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::Finished(int ExitCode, QProcess::ExitStatus Status)
{
	QProcess *Process = dynamic_cast<QProcess*>(sender());
	OutputString(Process->objectName(), QString("Core finished with code %1 and %2 status").arg(ExitCode).arg(Status == QProcess::NormalExit ? "normal" : "crash"));
}
//-----------------------------------------------------------------------------
void ISCaratService::Error(QProcess::ProcessError ErrorType)
{
    Q_UNUSED(ErrorType);
	QProcess *Process = dynamic_cast<QProcess*>(sender());
	OutputString(Process->objectName(), Process->errorString());
}
//-----------------------------------------------------------------------------
void ISCaratService::ReadyReadStandartOutput()
{
	QProcess *Process = dynamic_cast<QProcess*>(sender());
	QByteArray ByteArray = Process->readAllStandardOutput();
	if (!ByteArray.isEmpty())
	{
		OutputString(Process->objectName(), ByteArray);
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::OutputString(const QString &CoreObjectName, const QString &String)
{
	QString CompleteString = "[" + CoreObjectName + "] " + String;
	ISLOGGER_L(CompleteString);
	if (IsConnectedDebugger)
	{
		if (LocalSocket->isOpen() && LocalSocket->isValid())
		{
			LocalSocket->write(CompleteString.toStdString().c_str(), CompleteString.size());
			LocalSocket->flush();
		}
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::NewConnection()
{
	LocalSocket = LocalServer->nextPendingConnection();
	connect(LocalSocket, &QLocalSocket::disconnected, this, &ISCaratService::Disconnected);
	IsConnectedDebugger = true;
}
//-----------------------------------------------------------------------------
void ISCaratService::Disconnected()
{
	LocalSocket->deleteLater();
	IsConnectedDebugger = false;
}
//-----------------------------------------------------------------------------
