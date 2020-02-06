#include "ISLogger.h"
#include "ISDefinesCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
	: QObject(),
	FutureWatcher(new QFutureWatcher<void>(this))
{
	UpdateLogFileName();

	QDir DirLogs(DEFINES_CORE.PATH_LOGS_DIR);
	if (!DirLogs.exists())
	{
		DirLogs.mkdir(DEFINES_CORE.PATH_LOGS_DIR);
	}

	QFile LogFile(LogPath);
	IS_ASSERT(LogFile.open(QIODevice::Append), QString("Not opened log file \"%1\". Error: %2").arg(LogPath).arg(LogFile.errorString()));
	LogFile.close();

	QTimer *Timer = new QTimer(this);
	Timer->setInterval(500);
	connect(Timer, &QTimer::timeout, this, &ISLogger::Timeout);
	Timer->start();
}
//-----------------------------------------------------------------------------
ISLogger::~ISLogger()
{

}
//-----------------------------------------------------------------------------
ISLogger& ISLogger::GetInstance()
{
	static ISLogger Logger;
	return Logger;
}
//-----------------------------------------------------------------------------
QString ISLogger::GetLogFileName() const
{
	return LogPath;
}
//-----------------------------------------------------------------------------
void ISLogger::AddMessage(const QString &Message)
{
	Queue.enqueue(Message);
}
//-----------------------------------------------------------------------------
void ISLogger::AddMessageFunc(const QString &Function, const QString &Message)
{
	QString String = Function + ": " + Message;
	AddMessage(String);
}
//-----------------------------------------------------------------------------
void ISLogger::Timeout()
{
	if (Queue.count())
	{
		UpdateLogFileName();

		if (FutureWatcher->isRunning())
		{
			return;
		}

		QFuture<void> Future = QtConcurrent::run(this, &ISLogger::AddData);
		FutureWatcher->setFuture(Future);
	}
}
//-----------------------------------------------------------------------------
void ISLogger::AddData()
{
	while (Queue.count())
	{
		QString Message = Queue.dequeue();
		Message.append("\r\n");

		QFile File(LogPath);
		if (File.open(QIODevice::Append))
		{
			QTextStream TextStream(&File);
			TextStream << Message;
			File.close();
		}
	}
}
//-----------------------------------------------------------------------------
void ISLogger::UpdateLogFileName()
{
	LogPath = DEFINES_CORE.PATH_LOGS_DIR + '/' + DEFINES_CORE.APPLICATION_NAME + '_' + QDateTime::currentDateTime().toString(DATE_FORMAT_V2) + SYMBOL_POINT + EXTENSION_LOG;
}
//-----------------------------------------------------------------------------
