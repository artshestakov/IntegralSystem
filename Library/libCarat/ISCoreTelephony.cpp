#include "ISCoreTelephony.h"
#include "ISConstants.h"
#include "ISSettingsDatabase.h"
#include "ISQuery.h"
#include "ISPhoneNumberParser.h"
#include "ISDebug.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
static QString QS_CDR_ID = PREPARE_QUERY("SELECT id "
										 "FROM _cdr "
										 "WHERE NOT handling "
										 "AND NOT handling_error "
										 "AND NOT (dstchannel = '') "
										 "AND NOT dstchannel IS NULL "
										 "ORDER BY id");
//-----------------------------------------------------------------------------
static QString QS_CDR = PREPARE_QUERY("SELECT calldate, src, dst, dstchannel, duration, billsec, disposition, uniqueid "
									  "FROM _cdr "
									  "WHERE id = :ID");
//-----------------------------------------------------------------------------
static QString QU_CDR_HANDLING = PREPARE_QUERY("UPDATE _cdr SET handling = true WHERE id = :ID");
//-----------------------------------------------------------------------------
static QString QI_ASTERISK_CALLS = PREPARE_QUERY("INSERT INTO _asteriskcalls(ascl_uniqueid, ascl_dialbegin, ascl_direction, ascl_user, ascl_subscriber, ascl_number, ascl_dialend, ascl_hangup, ascl_duration, ascl_dialstatus) "
												 "VALUES(:UniqueID, :DialBegin, :Direction, :User, :Subscriber, :Number, :DialEnd, :Hangup, :Duration, :DialStatus)");
//-----------------------------------------------------------------------------
static QString QU_CDR_ERROR = PREPARE_QUERY("UPDATE _cdr SET handling_error = true WHERE id = :ID");
//-----------------------------------------------------------------------------
static QString QS_USER = PREPARE_QUERY("SELECT aspt_user FROM _asteriskpattern WHERE aspt_pattern = :Pattern");
//-----------------------------------------------------------------------------
static QString QS_DIRECTION = PREPARE_QUERY("SELECT asdr_id FROM _asteriskdirection WHERE asdr_uid = :DirectionUID");
//-----------------------------------------------------------------------------
static QString QS_DIAS_STATUS = PREPARE_QUERY("SELECT asds_id FROM _asteriskcallstatus WHERE asds_asteriskname = :AsteriskName")
//-----------------------------------------------------------------------------
static QString QS_CALL_EXIST = PREPARE_QUERY("SELECT COUNT(*) FROM _asteriskcalls WHERE ascl_uniqueid = :UniqueID");
//-----------------------------------------------------------------------------
ISCoreTelephony::ISCoreTelephony(int &argc, char **argv) : ISCaratCoreApplication(argc, argv)
{
	
}
//-----------------------------------------------------------------------------
ISCoreTelephony::~ISCoreTelephony()
{

}
//-----------------------------------------------------------------------------
bool ISCoreTelephony::Invoke()
{
	bool Result = ISCaratCoreApplication::Invoke();
	if (Result)
	{
		AsteriskDigitNumbers = SETTING_DATABASE_VALUE_DB(CONST_UID_DATABASE_SETTING_ASTERISK_DIGITNUMBERS).toInt(); //Скольки значные внутренние номера

		Timer = new QTimer(this);
		Timer->setInterval(1000);
		connect(Timer, &QTimer::timeout, this, &ISCoreTelephony::Timeout);
		Timer->start();

		Started();
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCoreTelephony::Timeout()
{
	Timer->stop();

	try
	{
		ISQuery qSelectCDR(QS_CDR_ID);
		qSelectCDR.SetShowLongQuery(false);
		if (qSelectCDR.Execute())
		{
			while (qSelectCDR.Next())
			{
				HandlingCDR(qSelectCDR.ReadColumn("id").toInt());
			}
		}
	}
	catch (...) { }

	Timer->start();
}
//-----------------------------------------------------------------------------
void ISCoreTelephony::HandlingCDR(int ID)
{
	ISQuery qSelectCDR(QS_CDR);
	qSelectCDR.SetShowLongQuery(false);
	qSelectCDR.BindValue(":ID", ID);
	qSelectCDR.ExecuteFirst();

	QDateTime CallDate = qSelectCDR.ReadColumn("calldate").toDateTime(); //Дата и время вызова
	QString SRC = qSelectCDR.ReadColumn("src").toString(); //Источник вызова
	QString DST = qSelectCDR.ReadColumn("dst").toString(); //Получатель вызова
	QString DSTChannel = qSelectCDR.ReadColumn("dstchannel").toString(); //Канал получателя вызова
	int Duration = qSelectCDR.ReadColumn("duration").toInt(); //Количество секунд от начала вызова до его завершения
	int BillSec = qSelectCDR.ReadColumn("billsec").toInt(); //Фактическое время разговора: от перехода вызывающего канала (channel) в состояние ANSWERED
	QString Disposition = qSelectCDR.ReadColumn("disposition").toString();
	QString UniqueID = qSelectCDR.ReadColumn("uniqueid").toString();

	QVariant DirectionID; //Направление вызова
	QString Subscriber; //Абонент
	QString Number; //Номер
	QVariant UserID; //Идентификатор пользователя
	QVariant DialEnd = GetDialEnd(Duration, BillSec); //Длительность соединения

	if (SRC.length() == AsteriskDigitNumbers && DST.length() == AsteriskDigitNumbers) //Внутренний вызов
	{
		DirectionID = GetDirection(CONST_UID_ASTERISK_DIRECTION_INTERNAL);
		Subscriber = SRC;
		Number = DST;
		ISDebug::ShowInfoString(LANG("Telephony.HanglingInternalCall").arg(Subscriber).arg(Number).arg(ID));
		UserID = GetUser(ID, Subscriber);
	}
	else if (SRC.length() == AsteriskDigitNumbers) //Исходящий вызов
	{
		DirectionID = GetDirection(CONST_UID_ASTERISK_DIRECTION_OUTGOING);
		Subscriber = SRC;
		Number = ISPhoneNumberParser::PasteEvent(DST);
		ISDebug::ShowInfoString(LANG("Telephony.HanglingOutgoingCall").arg(Subscriber).arg(Number).arg(ID));
		UserID = GetUser(ID, Subscriber);
	}
	else //Входящий вызов
	{
		DirectionID = GetDirection(CONST_UID_ASTERISK_DIRECTION_INCOMING);
		Subscriber = ISPhoneNumberParser::PasteEvent(SRC);
		Number = GetPattern(ID, DSTChannel);
		ISDebug::ShowInfoString(LANG("Telephony.HanglingIncomingCall").arg(Subscriber).arg(Number).arg(ID));
		UserID = GetUser(ID, Number);
	}

	try
	{
		if (CheckExistCall(UniqueID)) //Если вызов переведенный - присвоить ему соответствующее направление
		{
			DirectionID = GetDirection(CONST_UID_ASTERISK_DIRECTION_TRANSFER);
		}

		ISQuery qInsert(QI_ASTERISK_CALLS);
		qInsert.SetShowLongQuery(false);
		qInsert.BindValue(":UniqueID", UniqueID);
		qInsert.BindValue(":DialBegin", CallDate);
		qInsert.BindValue(":Direction", DirectionID);
		qInsert.BindValue(":User", UserID);
		qInsert.BindValue(":Subscriber", Subscriber);
		qInsert.BindValue(":Number", Number);
		qInsert.BindValue(":DialEnd", DialEnd);
		qInsert.BindValue(":Hangup", BillSec);
		qInsert.BindValue(":Duration", Duration);
		qInsert.BindValue(":DialStatus", GetDialStatus(ID, Disposition));
		if (qInsert.Execute()) //Если добавление прошло успешно
		{
			ISQuery qUpdateCDR(QU_CDR_HANDLING);
			qUpdateCDR.SetShowLongQuery(false);
			qUpdateCDR.BindValue(":ID", ID);
			qUpdateCDR.Execute();
		}
	}
	catch (ISQueryException &QueryException) //Если запрос на вставку данных не прошел - обновить запись как ошибочкую для дальнейшего анализа
	{
		ISQuery qUpdateError(QU_CDR_ERROR);
		qUpdateError.SetShowLongQuery(false);
		qUpdateError.BindValue(":ID", ID);
		qUpdateError.Execute();
	}
	catch (...) { }
}
//-----------------------------------------------------------------------------
QVariant ISCoreTelephony::GetDialEnd(int Duration, int BillSec) const
{
	QVariant DialEnd = Duration - BillSec;
	if (DialEnd.toInt() < 0)
	{
		DialEnd.clear();
	}

	return DialEnd;
}
//-----------------------------------------------------------------------------
QVariant ISCoreTelephony::GetDirection(const ISUuid &DirectionUID) const
{
	QVariant DirectionID;

	try
	{
		ISQuery qSelect(QS_DIRECTION);
		qSelect.SetShowLongQuery(false);
		qSelect.BindValue(":DirectionUID", DirectionUID);
		if (qSelect.ExecuteFirst())
		{
			DirectionID = qSelect.ReadColumn("asdr_id");
		}
	}
	catch (...) { }

	return DirectionID;
}
//-----------------------------------------------------------------------------
QVariant ISCoreTelephony::GetDialStatus(int ID, const QString &DialStatus) const
{
	QVariant DiasStatusID;

	try
	{
		ISQuery qSelect(QS_DIAS_STATUS);
		qSelect.SetShowLongQuery(false);
		qSelect.BindValue(":AsteriskName", DialStatus);
		if (qSelect.ExecuteFirst()) //Если идентификатор статуса найден
		{
			DiasStatusID = qSelect.ReadColumn("asds_id");
		}
	}
	catch (...) { }

	if (!DiasStatusID.isValid())
	{
		ISDebug::ShowWarningString(LANG("Telephony.NotFoundDiasStatus").arg(DialStatus).arg(ID));
	}

	return DiasStatusID;
}
//-----------------------------------------------------------------------------
QString ISCoreTelephony::GetPattern(int ID, const QString &String) const
{
	QString Pattern;
	QStringList StringList = String.split('/');
	if (StringList.count() == 2)
	{
		StringList = StringList.at(1).split("-");
		if (StringList.count() == 2)
		{
			Pattern = StringList.at(0);
		}
	}

	if (Pattern.length() != AsteriskDigitNumbers)
	{
		ISDebug::ShowWarningString(LANG("Telephony.ParsePatternInvalid").arg(String).arg(ID));
	}

	return Pattern;
}
//-----------------------------------------------------------------------------
QVariant ISCoreTelephony::GetUser(int ID, const QVariant &Pattern) const
{
	QVariant UserID;

	try
	{
		ISQuery qSelect(QS_USER);
		qSelect.SetShowLongQuery(false);
		qSelect.BindValue(":Pattern", Pattern);
		if (qSelect.ExecuteFirst())
		{
			UserID = qSelect.ReadColumn("aspt_user");
		}
	}
	catch (...) { }

	if (!UserID.isValid())
	{
		ISDebug::ShowWarningString(LANG("Telephony.NotFoundUser").arg(Pattern.toString()).arg(ID));
	}

	return UserID;
}
//-----------------------------------------------------------------------------
bool ISCoreTelephony::CheckExistCall(const QString &UniqueID) const
{
	bool Result = false;

	try
	{
		ISQuery qSelect(QS_CALL_EXIST);
		qSelect.SetShowLongQuery(false);
		qSelect.BindValue(":UniqueID", UniqueID);
		if (qSelect.ExecuteFirst())
		{
			if (qSelect.ReadColumn("count").toInt() > 0)
			{
				Result = true;
			}
		}
	}
	catch (...) { }

	return Result;
}
//-----------------------------------------------------------------------------
