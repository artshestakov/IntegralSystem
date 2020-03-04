#include "ISCaratService.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISCountingTime.h"
#include "ISSystem.h"
#include "ISNetwork.h"
#include "ISDatabase.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_name, core_filename "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "AND core_active "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(QObject *parent) : QObject(parent)
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
		//Общее количество ядер и количество запущенных
		int CoreCountTotal = qSelectCore.GetCountResultRows(), CoreCountStarted = 0;

		while (qSelectCore.Next()) //Обход ядер
		{
			QString CoreName = qSelectCore.ReadColumn("core_name").toString();
			QString FileName = qSelectCore.ReadColumn("core_filename").toString() + (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows ? QString().append(".").append(EXTENSION_EXE) : QString());

			ISLOGGER_INFO("Core \"" + CoreName + "\": starting...");
			QString CoreFilePath = ISDefines::Core::PATH_APPLICATION_DIR + '/' + FileName;
			if (QFile::exists(CoreFilePath)) //Если ядро существует
			{
				QProcess *Process = new QProcess(this);
				Process->setObjectName(CoreName);
				connect(Process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ISCaratService::Finished);
				connect(Process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), this, &ISCaratService::Error);
				connect(Process, &QProcess::readyReadStandardOutput, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				connect(Process, &QProcess::readyReadStandardError, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				Process->start(CoreFilePath);

				//Если дождались первого сообщения от ядра и оно валидное - считаем, что ядро успешно запустилось - иначе ошибка в любом случае
				if (Process->waitForReadyRead(CARAT_CORE_START_TIMEOUT) && Process->readAll().contains(CARAT_CORE_START_FLAG))
				{
					ISLOGGER_INFO("Core \"" + CoreName + "\" started. PID: " + QString::number(Process->processId()));
					++CoreCountStarted;
				}
				else
				{
					ISLOGGER_ERROR("Core \"" + CoreName + "\" not started");
				}
			}
			else //Ядро не существует
			{
				ISLOGGER_ERROR("Core \"" + CoreName + "\" not found. Path: " + CoreFilePath);
			}
		}

		CoreCountStarted == CoreCountTotal ?
			ISLOGGER_INFO("Started all cores") : //Если ядра были успешно запущены
			ISLOGGER_WARNING("Started " + QString::number(CoreCountStarted) + " of " + QString::number(CoreCountTotal));
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
	ISLOGGER_UNKNOWN(QString("[%1] %2").arg(CoreObjectName).arg(String));
}
//-----------------------------------------------------------------------------
