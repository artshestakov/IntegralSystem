#include "ISCoreAsteriskQueue.h"
#include "ISQuery.h"
#include "ISLogger.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "ISSettingsDatabase.h"
#include "ISConstants.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QS_ASTERISK_QUEUE = PREPARE_QUERY("SELECT astq_id, astq_creationdate, asqt_uid, asqt_name, astq_initiated, userfullname(astq_initiated), astq_parameters "
												 "FROM _asteriskqueue "
												 "LEFT JOIN _asteriskqueuetype ON asqt_id = astq_type "
												 "ORDER BY astq_id");
//-----------------------------------------------------------------------------
static QString QD_ASTERISK_QUEUE = PREPARE_QUERY("DELETE FROM _asteriskqueue WHERE astq_id = :QueueID");
//-----------------------------------------------------------------------------
static QString QD_ASTERISK_QUEUES = PREPARE_QUERY("DELETE FROM _asteriskqueue");
//-----------------------------------------------------------------------------
static QString QS_USER_PATTERN = PREPARE_QUERY("SELECT userfullname(:UserID), aspt_pattern, aspt_context "
											   "FROM _asteriskpattern "
											   "WHERE aspt_user = :UserID");
//-----------------------------------------------------------------------------
ISCoreAsteriskQueue::ISCoreAsteriskQueue(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreAsteriskQueue::~ISCoreAsteriskQueue()
{

}
//-----------------------------------------------------------------------------
bool ISCoreAsteriskQueue::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		ISMetaData::GetInstanse().Initialize(/*CONFIG_STRING(CONST_CONFIG_OTHER_CONFIGURATION)*/"", false, false); //???
		ISSettingsDatabase::GetInstance().Initialize();

		Timer = new QTimer(this);
		Timer->setInterval(200);
		connect(Timer, &QTimer::timeout, this, &ISCoreAsteriskQueue::Timeout);

		AsteriskSocket = new ISAsteriskSocket(this);
		connect(AsteriskSocket, &ISAsteriskSocket::SuccessfulAuth, this, &ISCoreAsteriskQueue::SuccessfulAuth);
		AsteriskSocket->Connect();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskQueue::SuccessfulAuth()
{
	AsteriskSocket->AddFilter(AMI_ORIGINATE_RESPONSE);
	ClearQueue();
	Timer->start();
	Started();
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskQueue::Timeout()
{
	Timer->stop();

	ISQuery qSelectQueue(QS_ASTERISK_QUEUE);
	qSelectQueue.SetShowLongQuery(false);
	if (qSelectQueue.Execute())
	{
		while (qSelectQueue.Next())
		{
			int QueueID = qSelectQueue.ReadColumn("astq_id").toInt();
			QDateTime CreationDateTime = qSelectQueue.ReadColumn("astq_creationdate").toDateTime();
			ISUuid TypeUID = qSelectQueue.ReadColumn("asqt_uid");
			QString TypeName = qSelectQueue.ReadColumn("asqt_name").toString();
			int UserID = qSelectQueue.ReadColumn("astq_initiated").toInt();
			QString UserFullName = qSelectQueue.ReadColumn("userfullname").toString();
			QString Parameters = qSelectQueue.ReadColumn("astq_parameters").toString();

			ISLOGGER_UNKNOWN("Event in queue from \"" + UserFullName + "\": " + TypeName);
			QueueEvent(TypeUID, UserID, Parameters);

			ISQuery qDelete(QD_ASTERISK_QUEUE);
			qDelete.SetShowLongQuery(false);
			qDelete.BindValue(":QueueID", QueueID);
			qDelete.Execute();
		}
	}

	Timer->start();
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskQueue::QueueEvent(const ISUuid &EventTypeUID, int UserID, const QString &Parameters)
{
	QVariantMap VariantMap = ISSystem::JsonStringToVariantMap(Parameters);

	if (EventTypeUID == CONST_UID_ASTERISK_QUEUE_TYPE_OUT_CALLED)
	{
		Originate(UserID, VariantMap);
	}
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskQueue::Originate(int UserID, const QVariantMap &Parameters)
{
	ISQuery qSelectPattern(QS_USER_PATTERN);
	qSelectPattern.BindValue(":UserID", UserID);
	if (qSelectPattern.ExecuteFirst())
	{
		QString UserFullname = qSelectPattern.ReadColumn("userfullname").toString();
		int Pattern = qSelectPattern.ReadColumn("aspt_pattern").toInt();
		QString Context = qSelectPattern.ReadColumn("aspt_context").toString();

		AsteriskSocket->write(QString("Action: Originate\r\n").toUtf8().data());
		AsteriskSocket->write(QString("Channel: SIP/%1\r\n").arg(Pattern).toUtf8().data());
		AsteriskSocket->write(QString("Context: %1\r\n").arg(Context).toUtf8().data());
		AsteriskSocket->write(QString("Exten: %1\r\n").arg(Parameters.value("Phone").toString()).toUtf8().data());
		AsteriskSocket->write(QString("Priority: 1\r\n").toUtf8().data());
		AsteriskSocket->write(QString("Callerid: %1\r\n").arg(Pattern).toUtf8().data());
		AsteriskSocket->write("\r\n");
	}
}
//-----------------------------------------------------------------------------
void ISCoreAsteriskQueue::ClearQueue()
{
	ISLOGGER_UNKNOWN("Clear asterisk task queue...");
	ISQuery qDeleteQueue(QD_ASTERISK_QUEUES);
	qDeleteQueue.SetShowLongQuery(false);
	if (qDeleteQueue.Execute())
	{
		int CountAffected = qDeleteQueue.GetCountAffected();
		CountAffected ? ISLOGGER_UNKNOWN("Clear finished. Deleted event count:" + QString::number(CountAffected)) : ISLOGGER_UNKNOWN("Clear not required");
	}
}
//-----------------------------------------------------------------------------
