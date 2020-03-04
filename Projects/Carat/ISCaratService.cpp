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
			QString CoreFilePath = ISDefines::Core::PATH_APPLICATION_DIR + '/' + FileName + "1";
			if (QFile::exists(CoreFilePath)) //Если ядро существует
			{
				QProcess *Process = new QProcess(this);
				Process->setObjectName(CoreName);
				Process->setProgram(CoreFilePath);
				connect(Process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ISCaratService::Finished);
				connect(Process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), this, &ISCaratService::Error);
				connect(Process, &QProcess::readyReadStandardOutput, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				connect(Process, &QProcess::readyReadStandardError, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				Process->start();

				//Если дождались первого сообщения от ядра и оно валидное - считаем, что ядро успешно запустилось - иначе ошибка в любом случае
				if (Process->waitForReadyRead(CARAT_CORE_START_TIMEOUT) && Process->readAll().contains(CARAT_CORE_START_FLAG))
				{
					ISLOGGER_INFO("Core \"" + CoreName + "\" started");
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

		if (CoreCountTotal)
		{
			ISLOGGER_WARNING("Not starting " + QString::number(CoreCountTotal) + " of " + QString::number(qSelectCore.GetCountResultRows()));
		}
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::Finished(int ExitCode, QProcess::ExitStatus Status)
{
	
}
//-----------------------------------------------------------------------------
void ISCaratService::Error(QProcess::ProcessError ErrorType)
{
	
}
//-----------------------------------------------------------------------------
void ISCaratService::ReadyReadStandartOutput()
{
	QProcess *Process = dynamic_cast<QProcess*>(sender());
	QByteArray ByteArray = Process->readAllStandardOutput();
	if (!ByteArray.isEmpty())
	{
		ISLOGGER_UNKNOWN("[" + Process->objectName() + "] " + ByteArray);
	}
}
//-----------------------------------------------------------------------------
