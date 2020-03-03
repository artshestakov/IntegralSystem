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
static QString QS_CARAT_CORE = PREPARE_QUERY("SELECT core_name, core_filename, core_localname "
											 "FROM _caratcore "
											 "WHERE NOT core_isdeleted "
											 "AND core_active "
											 "ORDER BY core_priority");
//-----------------------------------------------------------------------------
ISCaratService::ISCaratService(QObject *parent)
	: QObject(parent),
	EventLoop(new QEventLoop(this))
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
		while (qSelectCore.Next()) //Обход ядер
		{
			QString Name = qSelectCore.ReadColumn("core_name").toString();
			QString LocalName = qSelectCore.ReadColumn("core_localname").toString();
			QString FileName = qSelectCore.ReadColumn("core_filename").toString() + (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows ? QString().append(".").append(EXTENSION_EXE) : QString());

			ISLOGGER_INFO("Core \"" + Name + "\": starting...");
			QString CoreFilePath = ISDefines::Core::PATH_APPLICATION_DIR + '/' + FileName;
			if (QFile::exists(CoreFilePath)) //Если ядро существует
			{
				QProcess *Process = new QProcess(this);
				Process->setObjectName(Name);
				Process->setProgram(CoreFilePath);
				Process->setReadChannel(QProcess::StandardOutput);
				connect(Process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ISCaratService::Finished);
				connect(Process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error), this, &ISCaratService::Error);
				connect(Process, &QProcess::readyReadStandardOutput, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				connect(Process, &QProcess::readyReadStandardError, this, &ISCaratService::ReadyReadStandartOutput, Qt::QueuedConnection);
				Process->start();
				EventLoop->exec();
				QString s = "";
			}
			else //Ядро не существует
			{
				ISLOGGER_ERROR("Core \"" + Name + "\" not found. Path: " + CoreFilePath);
				ISSystem::SleepSeconds(3);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISCaratService::Started()
{
	EventLoop->quit();
}
//-----------------------------------------------------------------------------
void ISCaratService::Finished(int ExitCode, QProcess::ExitStatus Status)
{
	EventLoop->quit();
}
//-----------------------------------------------------------------------------
void ISCaratService::Error(QProcess::ProcessError ErrorType)
{
	EventLoop->quit();
}
//-----------------------------------------------------------------------------
void ISCaratService::ReadyReadStandartOutput()
{
	QProcess *Process = dynamic_cast<QProcess*>(sender());
	QByteArray ByteArray = Process->readAllStandardOutput();

	if (EventLoop->isRunning()) //Если ядро в процессе запуска - проверяем содержимое вывода
	{
		if (ByteArray.contains(CARAT_CORE_START_FLAG)) //Если ядро запустилось - останавливаем паузу потока событий
		{
			EventLoop->quit();
		}
	}
	else //Ядро не в процессе запуска - выводим в консоль вывод
	{
		char *Char = ByteArray.data();
		size_t Size = strlen(Char);
		printf("[%s] %s", Process->objectName().toStdString().c_str(), Char);
	}
}
//-----------------------------------------------------------------------------
