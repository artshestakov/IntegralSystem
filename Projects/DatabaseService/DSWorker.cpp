#include "StdAfx.h"
#include "DSWorker.h"
#include "EXDefines.h"
#include "EXConstants.h"
#include "ISConfig.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISDebug.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
DSWorker::DSWorker(QObject *parent) : QObject(parent)
{
	DatabaseName = CONFIG_STRING(CONST_CONFIG_CONNECTION_DATABASE);
	FilePath = DatabaseName + "_" + DATE_TODAY.toString(DATE_FORMAT_V2) + "_" + QTime::currentTime().toString(TIME_FORMAT_V2);

	EventLoop = new QEventLoop(this);

	QProcessEnvironment ProcessEnvironment;
	ProcessEnvironment.insert("PGPASSWORD", CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_PASSWORD));

	Process = new ISProcess(this);
	Process->SetProcessEnvironment(ProcessEnvironment);
	connect(Process, &ISProcess::Finished, EventLoop, &QEventLoop::quit);
	connect(Process, &ISProcess::Message, this, &DSWorker::ProcessMessage);
}
//-----------------------------------------------------------------------------
DSWorker::~DSWorker()
{

}
//-----------------------------------------------------------------------------
void DSWorker::ClearBackupFolder()
{
	int KeepLast = CONFIG_INT(CONST_CONFIG_DATABASESERVICE_KEEP_OVER_DAYS);
	if (KeepLast > 0)
	{
		ISDebug::ShowInfoString(LOCALIZATION("CheckBackupFolder").arg(CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERBACKUP)));
		QStringList FilterList;
		FilterList.append("*.sql");
		FilterList.append("*.dmp");
		QFileInfoList FileInfoList = QDir(CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERBACKUP)).entryInfoList(FilterList, QDir::Files, QDir::Name);
		for (const QFileInfo &FileInfo : FileInfoList)
		{
			QString FileName = FileInfo.fileName();
			QString DateString;
			QRegExp RegExp(REG_EXP_BACKUP_FILE_DATE);
			int Pos = 0;
			while ((Pos = RegExp.indexIn(FileName, Pos)) != -1)
			{
				DateString = RegExp.cap(0);
				Pos += RegExp.matchedLength();
			}

			QDate DateFile = QDate::fromString(DateString, DATE_FORMAT_V2);
			QDate DateKeep = QDate::currentDate().addDays(-KeepLast);
			if (DateFile <= DateKeep)
			{
				ISDebug::ShowInfoString(LOCALIZATION("RemoveBackupFile.Process").arg(FileName));
				QFile File(FileInfo.absoluteFilePath());
				if (File.remove())
				{
					ISDebug::ShowInfoString(LOCALIZATION("RemoveBackupFile.Done"));
				}
				else
				{
					ISDebug::ShowInfoString(LOCALIZATION("RemoveBackupFile.Failed").arg(FileName).arg(File.errorString()));
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void DSWorker::Start()
{
	QString PgDump = CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERPOSTGRESBIN) + "/";
    QString PgDumpAll = CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERPOSTGRESBIN) + "/";

    if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
    {
		PgDump += PG_DUMP_WINDOWS;
        PgDumpAll += PG_DUMPALL_WINDOWS;
    }
    else if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Linux)
    {
		PgDump += PG_DUMP_LINUX;
        PgDumpAll += PG_DUMPALL_LINUX;
    }

	Process->Start("\"" + PgDump + "\"", GetDumpArguments(FilePath));
	EventLoop->exec();

    Process->Start("\"" + PgDumpAll + "\"", GetDumpAllArguments(FilePath));
	EventLoop->exec();
}
//-----------------------------------------------------------------------------
void DSWorker::ProcessMessage(const QString &Message)
{
	QTextStream TextStream(stdout);
	TextStream.setCodec(TEXT_CODEC_IBM866);
	TextStream << Message << endl;

	IS_LOGGER(Message);
}
//-----------------------------------------------------------------------------
QStringList DSWorker::GetDumpArguments(const QString &FilePath) const
{
	QStringList Arguments;
	Arguments += QString("--host=%1").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER));
	Arguments += QString("--port=%1").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT));
	Arguments += QString("--username=%1").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN));
	Arguments += QString("--format=c");
	Arguments += QString("--encoding=UTF8");
	Arguments += QString("--compress=9");
	Arguments += QString("--inserts");
	Arguments += QString("--column-inserts");
	Arguments += QString("--verbose");

	Arguments += QString("--exclude-table-data=_distfiles"); //Исключение обновлений
	Arguments += QString("--exclude-table-data=_distfilesdata"); //Исключение данных обновлений
	Arguments += QString("--exclude-table-data=_fias_*"); //Исключение данных ФИАС
	
	Arguments += QString("--file=%1_Data.dmp").arg(CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERBACKUP) + "/" + FilePath);
	Arguments += QString(DatabaseName);
	return Arguments;
}
//-----------------------------------------------------------------------------
QStringList DSWorker::GetDumpAllArguments(const QString &FilePath) const
{
	QStringList Arguments;
	Arguments += QString("--host=%1").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_SERVER));
	Arguments += QString("--port=%1").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_PORT));
	Arguments += QString("--username=%1").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN));
	Arguments += QString("--role=%1").arg(CONFIG_STRING(CONST_CONFIG_CONNECTION_LOGIN));
	Arguments += QString("-c");
	Arguments += QString("-r");
	Arguments += QString("-v");
	Arguments += QString("--file=%1_Roles.sql").arg(CONFIG_STRING(CONST_CONFIG_DATABASESERVICE_FOLDERBACKUP) + "/" + FilePath);
	return Arguments;
}
//-----------------------------------------------------------------------------
