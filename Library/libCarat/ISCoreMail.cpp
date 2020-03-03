#include "ISCoreMail.h"
#include "ISConstants.h"
#include "ISQuery.h"
#include "ISEMail.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
static QString QS_MAIL_QUEUE = PREPARE_QUERY("SELECT mail_id, mail_server, mail_port, ectp_uid, mail_senderlogin, mail_senderpassword, mail_sendername, mail_recipientlogin, mail_subject, mail_message "
											 "FROM _emailqueue "
											 "LEFT JOIN _emailconnectiontype ON ectp_id = mail_connectiontype "
											 "WHERE mail_id = (SELECT MIN(mail_id) "
															  "FROM _emailqueue "
															  "WHERE NOT mail_isdeleted "
															  "AND NOT mail_sended "
															  "AND mail_status = (SELECT eqst_id FROM _emailqueuestatus WHERE eqst_uid = :StatusUID))");
//-----------------------------------------------------------------------------
static QString QU_MAIL_DONE = PREPARE_QUERY("UPDATE _emailqueue SET "
											"mail_status = (SELECT eqst_id FROM _emailqueuestatus WHERE eqst_uid = :StatusUID), "
											"mail_sended = true "
											"WHERE mail_id = :MailID");
//-----------------------------------------------------------------------------
static QString QU_MAIL_FAILED = PREPARE_QUERY("UPDATE _emailqueue SET "
											  "mail_status = (SELECT eqst_id FROM _emailqueuestatus WHERE eqst_uid = :StatusUID), "
											  "mail_sended = false, "
											  "mail_errorstring = :ErrorString "
											  "WHERE mail_id = :MailID");
//-----------------------------------------------------------------------------
ISCoreMail::ISCoreMail(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreMail::~ISCoreMail()
{

}
//-----------------------------------------------------------------------------
bool ISCoreMail::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		Timer = new QTimer(this);
		Timer->setInterval(1000);
		connect(Timer, &QTimer::timeout, this, &ISCoreMail::Timeout);
		Timer->start();

		Started();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreMail::Timeout()
{
	Timer->stop();

	ISQuery qSelectMailQueue(QS_MAIL_QUEUE);
	qSelectMailQueue.SetShowLongQuery(false);
	qSelectMailQueue.BindValue(":StatusUID", CONST_UID_EMAIL_STATUS_SENDING);
	if (qSelectMailQueue.ExecuteFirst())
	{
		int ID = qSelectMailQueue.ReadColumn("mail_id").toInt();
		QString Server = qSelectMailQueue.ReadColumn("mail_server").toString();
		int Port = qSelectMailQueue.ReadColumn("mail_port").toInt();
		ISUuid ConnectionType = qSelectMailQueue.ReadColumn("ectp_uid");
		QString SenderLogin = qSelectMailQueue.ReadColumn("mail_senderlogin").toString();
		QString SenderPassword = qSelectMailQueue.ReadColumn("mail_senderpassword").toString();
		QString SenderName = qSelectMailQueue.ReadColumn("mail_sendername").toString();
		QString RecipientLogin = qSelectMailQueue.ReadColumn("mail_recipientlogin").toString();
		QString Subject = qSelectMailQueue.ReadColumn("mail_subject").toString();
		QString Message = qSelectMailQueue.ReadColumn("mail_message").toString();

		ISLOGGER_INFO("Sending message - start");
		ISEMail EMail(Server, Port, ConnectionType, SenderLogin, SenderPassword, SenderName, RecipientLogin, Subject, Message, this);
		if (EMail.SendMessage())
		{
			SendDone(ID);
			ISLOGGER_INFO("Sending message - done");
		}
		else
		{
			SendFailed(ID, EMail.GetErrorString());
			ISLOGGER_WARNING(QString("Sending message - failed: %1").arg(EMail.GetErrorString()));
		}
	}

	Timer->start();
}
//-----------------------------------------------------------------------------
void ISCoreMail::SendDone(int MailID)
{
	ISQuery qUpdateDone(QU_MAIL_DONE);
	qUpdateDone.SetShowLongQuery(false);
	qUpdateDone.BindValue(":StatusUID", CONST_UID_EMAIL_STATUS_SENDED);
	qUpdateDone.BindValue(":MailID", MailID);
	qUpdateDone.Execute();
}
//-----------------------------------------------------------------------------
void ISCoreMail::SendFailed(int MailID, const QString &ErrorString)
{
	ISQuery qUpdateFailed(QU_MAIL_FAILED);
	qUpdateFailed.SetShowLongQuery(false);
	qUpdateFailed.BindValue(":StatusUID", CONST_UID_EMAIL_STATUS_ERROR);
	qUpdateFailed.BindValue(":ErrorString", ErrorString);
	qUpdateFailed.BindValue(":MailID", MailID);
	qUpdateFailed.Execute();
}
//-----------------------------------------------------------------------------
