#include "StdAfx.h"
#include "ISCoreSMS.h"
#include "EXConstants.h"
#include "ISQuery.h"
#include "ISDebug.h"
#include "ISSMS.h"
//-----------------------------------------------------------------------------
static QString QS_SMS_QUEUE = PREPARE_QUERY("SELECT smsq_id, smsq_login, smsq_password, smsq_phone, smsq_message, smsq_charset "
											"FROM _smsqueue "
											"WHERE smsq_id = (SELECT MIN(smsq_id) "
															 "FROM _smsqueue "
															 "WHERE NOT smsq_isdeleted "
															 "AND NOT smsq_sended "
															 "AND smsq_status = (SELECT smss_id FROM _smsstatus WHERE smss_uid = :StatusUID))");
//-----------------------------------------------------------------------------
static QString QS_CHARSET = PREPARE_QUERY("SELECT smse_name FROM _smsencodingtype WHERE smse_uid = :CharsetUID");
//-----------------------------------------------------------------------------
static QString QU_SMS_STATUS = PREPARE_QUERY("UPDATE _smsqueue SET "
											 "smsq_status = (SELECT smss_id FROM _smsstatus WHERE smss_uid = :StatusUID) "
											 "WHERE smsq_id = :ID");
//-----------------------------------------------------------------------------
static QString QU_SMS_DONE = PREPARE_QUERY("UPDATE _smsqueue SET "
										   "smsq_status = (SELECT smss_id FROM _smsstatus WHERE smss_uid = :StatusUID), "
										   "smsq_sended = true, "
										   "smsq_messageid = :MessageID "
										   "WHERE smsq_id = :ID");
//-----------------------------------------------------------------------------
static QString QU_SMS_FAIL = PREPARE_QUERY("UPDATE _smsqueue SET "
										   "smsq_status = (SELECT smss_id FROM _smsstatus WHERE smss_uid = :StatusUID), "
										   "smsq_errorstring = :ErrorString, "
										   "smsq_sended = false, "
										   "smsq_messageid = :MessageID "
										   "WHERE smsq_id = :ID");
//-----------------------------------------------------------------------------
ISCoreSMS::ISCoreSMS(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{

}
//-----------------------------------------------------------------------------
ISCoreSMS::~ISCoreSMS()
{

}
//-----------------------------------------------------------------------------
bool ISCoreSMS::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		Timer = new QTimer(this);
		Timer->setInterval(1000);
		connect(Timer, &QTimer::timeout, this, &ISCoreSMS::Timeout);
		Timer->start();

		Started();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreSMS::Timeout()
{
	Timer->stop();

	ISQuery qSelectSMS(QS_SMS_QUEUE);
	qSelectSMS.BindValue(":StatusUID", CONST_UID_SMS_STATUS_PENDING);
	if (qSelectSMS.ExecuteFirst())
	{
		int ID = qSelectSMS.ReadColumn("smsq_id").toInt();
		QString Login = qSelectSMS.ReadColumn("smsq_login").toString();
		QString Password = qSelectSMS.ReadColumn("smsq_password").toString();
		QString Phone = qSelectSMS.ReadColumn("smsq_phone").toString();
		QString Message = qSelectSMS.ReadColumn("smsq_message").toString();
		ISUuid CharsetUID = qSelectSMS.ReadColumn("smsq_charset");
		QString Charset = GetCharset(CharsetUID);

		UpdateStatus(ID, CONST_UID_SMS_STATUS_SENDING);

		ISDebug::ShowInfoString("Sending message - start");
		ISSMS SMS(Login, Password, Phone, Message, Charset, this);
		if (SMS.SendMessage())
		{
			SendDone(ID, SMS.GetMessageID());
			ISDebug::ShowInfoString("Sending message - done");
		}
		else
		{
			SendFailed(ID, SMS.GetMessageID(), SMS.GetErrorString());
			ISDebug::ShowWarningString(QString("Sending message - failed: %1").arg(SMS.GetErrorString()));
		}
	}

	Timer->start();
}
//-----------------------------------------------------------------------------
void ISCoreSMS::UpdateStatus(int ID, const ISUuid &StatusUID)
{
	ISQuery qUpdate(QU_SMS_STATUS);
	qUpdate.BindValue(":StatusUID", StatusUID);
	qUpdate.BindValue(":ID", ID);
	qUpdate.Execute();
}
//-----------------------------------------------------------------------------
void ISCoreSMS::SendDone(int ID, int MessageID)
{
	ISQuery qUpdate(QU_SMS_DONE);
	qUpdate.BindValue(":StatusUID", CONST_UID_SMS_STATUS_SENDED);
	qUpdate.BindValue(":MessageID", MessageID);
	qUpdate.BindValue(":ID", ID);
	qUpdate.Execute();
}
//-----------------------------------------------------------------------------
void ISCoreSMS::SendFailed(int ID, int MessageID, const QString &ErrorString)
{
	ISQuery qUpdate(QU_SMS_FAIL);
	qUpdate.BindValue(":StatusUID", CONST_UID_SMS_STATUS_ERROR);
	qUpdate.BindValue(":ErrorString", ErrorString);
	qUpdate.BindValue(":MessageID", MessageID);
	qUpdate.BindValue(":ID", ID);
	qUpdate.Execute();
}
//-----------------------------------------------------------------------------
QString ISCoreSMS::GetCharset(const ISUuid &CharsetUID) const
{
	QString Charset = "utf-8";
	ISQuery qSelectCharset(QS_CHARSET);
	qSelectCharset.BindValue(":CharsetUID", CharsetUID);
	if (qSelectCharset.ExecuteFirst())
	{
		Charset = qSelectCharset.ReadColumn("smse_name").toString();
	}

	return Charset;
}
//-----------------------------------------------------------------------------
