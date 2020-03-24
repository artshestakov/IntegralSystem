#include "ISCore.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISQueryPool.h"
#include "ISLogger.h"
#include "ISProtocol.h"
#include "ISSystem.h"
#include "ISSettings.h"
#include "ISMetaUser.h"
#include "ISSettingsDatabase.h"
#include "ISCrashDumper.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
static QString QI_HISTORY = PREPARE_QUERY("INSERT INTO _history(htry_user, htry_tablename, htry_tablelocalname, htry_objectname, htry_objectid) "
										  "VALUES(:CurrentUserID, :TableName, :TableLocalName, :ObjectName, :ObjectID)");
//-----------------------------------------------------------------------------
static QString QD_HISTORY = PREPARE_QUERY("DELETE FROM _history WHERE htry_user = currentuserid()");
//-----------------------------------------------------------------------------
static QString QI_CALENDAR = PREPARE_QUERY("INSERT INTO _calendar(cldr_date, cldr_timealert, cldr_name, cldr_text, cldr_tablename, cldr_objectid) "
                                           "VALUES(:Date, :TimeAlert, :Name, :Text, :TableName, :ObjectID) "
										   "RETURNING cldr_id");
//-----------------------------------------------------------------------------
static QString QU_CALENDAR_CLOSE = PREPARE_QUERY("UPDATE _calendar SET cldr_closed = true WHERE cldr_id = :CalendarID");
//-----------------------------------------------------------------------------
static QString QI_EMAIL = PREPARE_QUERY("INSERT INTO _emailqueue(mail_server, mail_port, mail_connectiontype, mail_senderlogin, mail_senderpassword, mail_sendername, mail_recipientlogin, mail_subject, mail_message, mail_status) "
										"VALUES(:Server, :Port, (SELECT ectp_id FROM _emailconnectiontype WHERE ectp_uid = :ConnectionUID), :SenderLogin, :SenderPassword, :SenderName, :RecipientLogin, :Subject, :Message, (SELECT eqst_id FROM _emailqueuestatus WHERE eqst_uid = :StatusUID))");
//-----------------------------------------------------------------------------
static QString QI_SMS = PREPARE_QUERY("INSERT INTO _smsqueue(smsq_login, smsq_password, smsq_phone, smsq_message, smsq_charset, smsq_status) "
									  "VALUES(:Login, :Password, :Phone, :Message, :Charset, (SELECT smss_id FROM _smsstatus WHERE smss_uid = :StatusUID))");
//-----------------------------------------------------------------------------
static QString QS_TASK_STATUS = PREPARE_QUERY("SELECT tsst_uid "
											  "FROM _task "
											  "LEFT JOIN _taskstatus ON tsst_id = task_status "
											  "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QU_TASK_STATUS = PREPARE_QUERY("UPDATE _task SET "
											  "task_status = (SELECT tsst_id FROM _taskstatus WHERE tsst_uid = :StatusUID), "
											  "task_resolution = :Resolution "
											  "WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QS_TASK_COUNT = PREPARE_QUERY("SELECT COUNT(*) FROM _task WHERE task_id = :TaskID");
//-----------------------------------------------------------------------------
static QString QS_TASK_DUPLICATE = PREPARE_QUERY("SELECT COUNT(*) "
												 "FROM _taskduplicate "
												 "WHERE tsdp_original = :OriginalID "
												 "AND tsdp_duplicate = :DuplicateID");
//-----------------------------------------------------------------------------
static QString QI_TASK_DUPLICATE = PREPARE_QUERY("INSERT INTO _taskduplicate(tsdp_original, tsdp_duplicate) "
												 "VALUES(:OriginalID, :DuplicateID)");
//-----------------------------------------------------------------------------
static QString QI_TASK_HISTORY = PREPARE_QUERY("INSERT INTO _taskhistory(thst_task, thst_user, thst_action, thst_information) "
											   "VALUES(:TaskID, :UserID, (SELECT thac_id FROM _TaskHistoryAction WHERE thac_uid = :ActionUID), :Information)");
//-----------------------------------------------------------------------------
static QString QS_TASK_ATTACH = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _taskobject "
											  "WHERE tobj_task = :TaskID "
											  "AND tobj_tablename = :TableName "
											  "AND tobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QI_TASK_ATTACH = PREPARE_QUERY("INSERT INTO _taskobject(tobj_task, tobj_tablename, tobj_objectid) "
											  "VALUES(:TaskID, :TableName, :ObjectID)");
//-----------------------------------------------------------------------------
static QString QD_TASK_DETACH = PREPARE_QUERY("DELETE FROM _taskobject "
											  "WHERE tobj_task = :TaskID "
											  "AND tobj_tablename = :TableName "
											  "AND tobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QS_COUNT_OVERDUE = PREPARE_QUERY("SELECT COUNT(*) "
												"FROM _task "
												"WHERE NOT task_isdeleted "
												"AND task_executor = currentuserid() "
												"AND task_deadline < CURRENT_DATE");
//-----------------------------------------------------------------------------
bool ISCore::Startup(bool IsGui, QString &ErrorString)
{
	ISDefines::Core::Init(IsGui);

	bool Result = ISSystem::CreateDir(ISDefines::Core::PATH_TEMP_DIR, ErrorString); //Создание папки для временных файлов
	if (!Result)
	{
		return Result;
	}

	Result = ISSystem::CreateDir(ISDefines::Core::PATH_CRASH_DIR, ErrorString); //Создание папки для crash-файлов
	if (!Result)
	{
		return Result;
	}

	ISCrashDumper::Init();

	Result = ISLogger::Instance().Initialize(true, true, ISDefines::Core::APPLICATION_NAME.toStdString());
	if (!Result)
	{
		ISLOGGER_ERROR(ISLogger::Instance().GetErrorString());
		return EXIT_FAILURE;
	}

	Result = ISConfig::Instance().Initialize();
	if (!Result)
	{
		ErrorString = ISConfig::Instance().GetErrorString();
		return Result;
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCore::ExitApplication()
{
	ISLogger::Instance().Shutdown();
	ISQueryPool::Instance().Shutdown();
	QCoreApplication::quit();
}
//-----------------------------------------------------------------------------
QString ISCore::GetObjectName(PMetaTable *MetaTable, int ObjectID)
{
	QString ObjectName;
	if (MetaTable->TitleName.isEmpty())
	{
		ObjectName = QString::number(ObjectID);
	}
	else
	{
		QString TitleName = MetaTable->TitleName;
		QStringList StringList = TitleName.split(';');
		QString QueryText = "SELECT ";

		if (StringList.count() > 1) //Если имя объекта строится из нескольких полей
		{
			QueryText += "concat(";
			for (int i = 0; i < StringList.count(); ++i)
			{
				QueryText += MetaTable->Alias + '_' + StringList[i] + ", ' ', ";
			}

			ISSystem::RemoveLastSymbolFromString(QueryText, 7);
			QueryText += ") \n";
		}
		else //Если указано только одно поле
		{
			QueryText += MetaTable->Alias + '_' + TitleName + " \n";
		}

		QueryText += "FROM " + MetaTable->Name + " \n";
		QueryText += "WHERE " + MetaTable->Alias + "_id = :ObjectID";

		ISQuery qSelectName(QueryText);
		qSelectName.BindValue(":ObjectID", ObjectID);
		if (qSelectName.ExecuteFirst())
		{
			ObjectName = qSelectName.ReadColumn(0).toString();
		}
	}
	return ObjectName;
}
//-----------------------------------------------------------------------------
void ISCore::AddHistory(const QString &TableName, const QString &LocalListName, const QString &ObjectName, int ObjectID)
{
	ISQueryPool::Instance().AddQuery(QI_HISTORY,
	{
		{ ":CurrentUserID", ISMetaUser::Instance().UserData->ID },
		{ ":TableName", TableName },
		{ ":TableLocalName", LocalListName },
		{ ":ObjectName", ObjectName },
		{ ":ObjectID", ObjectID }
		}
	);
}
//-----------------------------------------------------------------------------
void ISCore::ClearHistory()
{
	ISQueryPool::Instance().AddQuery(QD_HISTORY);
}
//-----------------------------------------------------------------------------
int ISCore::CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName, int ObjectID)
{
	int CalendarID = 0;

	ISQuery qInsertCalendar(QI_CALENDAR);
	qInsertCalendar.BindValue(":Date", DateTime.date());
	qInsertCalendar.BindValue(":TimeAlert", DateTime.time());
	qInsertCalendar.BindValue(":Name", Name);
	qInsertCalendar.BindValue(":Text", Text);

	if (TableName.isEmpty())
	{
		qInsertCalendar.BindValue(":TableName", QVariant());
		qInsertCalendar.BindValue(":ObjectID", QVariant());
	}
	else
	{
		IS_ASSERT(ObjectID, "Invalid object id");
		qInsertCalendar.BindValue(":TableName", TableName);
		qInsertCalendar.BindValue(":ObjectID", ObjectID);
	}

	if (qInsertCalendar.ExecuteFirst())
	{
		CalendarID = qInsertCalendar.ReadColumn("cldr_id").toInt();
	}

	return CalendarID;
}
//-----------------------------------------------------------------------------
bool ISCore::CalendarCloseEvent(int CalendarID)
{
	ISQuery qCloseEvent(QU_CALENDAR_CLOSE);
	qCloseEvent.BindValue(":CalendarID", CalendarID);
	return qCloseEvent.Execute();
}
//-----------------------------------------------------------------------------
void ISCore::EMailSend(const QString &Server, int Port, const ISUuid &ConnectionUID, const QString &SenderLogin, const QString &SenderPassword, const QString &SenderName, const QString &RecipientLogin, const QString &Subject, const QString &Message)
{
	ISQuery qInsert(QI_EMAIL);
	qInsert.BindValue(":Server", Server);
	qInsert.BindValue(":Port", Port);
	qInsert.BindValue(":ConnectionUID", ConnectionUID);
	qInsert.BindValue(":SenderLogin", SenderLogin);
	qInsert.BindValue(":SenderPassword", SenderPassword);
	qInsert.BindValue(":SenderName", SenderName);
	qInsert.BindValue(":RecipientLogin", RecipientLogin);
	qInsert.BindValue(":Subject", Subject);
	qInsert.BindValue(":Message", Message);
	qInsert.BindValue(":StatusUID", CONST_UID_EMAIL_STATUS_SENDING);
	qInsert.Execute();
}
//-----------------------------------------------------------------------------
void ISCore::EMailSend(const QString &RecipientLogin, const QString &Subject, const QString &Message)
{
	QString Server = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_EMAIL_SERVER);
	int Port = SETTING_DATABASE_VALUE_INT(CONST_UID_DATABASE_SETTING_EMAIL_PORT);
	QString SenderLogin = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_EMAIL_LOGIN);
	QString SenderPassword = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_EMAIL_PASSWORD);
	QString SenderName = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_EMAIL_NAME);
	EMailSend(Server, Port, CONST_UID_EMAIL_CONNECTION_SSL, SenderLogin, SenderPassword, SenderName, RecipientLogin, Subject, Message);
}
//-----------------------------------------------------------------------------
void ISCore::SMSSend(const QString &Login, const QString &Password, const QString &Phone, const QString &Message, const QString &Charset)
{
	ISQuery qInsertSMS(QI_SMS);
	qInsertSMS.BindValue(":Login", Login);
	qInsertSMS.BindValue(":Password", Password);
	qInsertSMS.BindValue(":Phone", Phone);
	qInsertSMS.BindValue(":Message", Message);
	qInsertSMS.BindValue(":Charset", Charset);
	qInsertSMS.BindValue(":StatusUID", CONST_UID_SMS_STATUS_PENDING);
	qInsertSMS.Execute();
}
//-----------------------------------------------------------------------------
void ISCore::SMSSend(const QString &Phone, const QString &Message)
{
	SMSSend
	(
		SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_SMSSERVICE_LOGIN),
		SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_SMSSERVICE_PASSWORD),
		Phone,
		Message,
		SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_SMSSERVICE_ENCODING)
	);
}
//-----------------------------------------------------------------------------
ISUuid ISCore::TaskGetStatusUID(int TaskID)
{
	ISQuery qSelectStatus(QS_TASK_STATUS);
	qSelectStatus.BindValue(":TaskID", TaskID);
	if (qSelectStatus.ExecuteFirst())
	{
		return qSelectStatus.ReadColumn("tsst_uid");
	}
	return ISUuid();
}
//-----------------------------------------------------------------------------
void ISCore::TaskSetStatus(int TaskID, const ISUuid &StatusUID, const QVariant &Resolution)
{
	ISQuery qUpdateStatus(QU_TASK_STATUS);
	qUpdateStatus.BindValue(":StatusUID", StatusUID);
	qUpdateStatus.BindValue(":Resolution", Resolution);
	qUpdateStatus.BindValue(":TaskID", TaskID);
	qUpdateStatus.Execute();
}
//-----------------------------------------------------------------------------
bool ISCore::TaskCheckExist(int TaskID)
{
	ISQuery qSelect(QS_TASK_COUNT);
	qSelect.BindValue(":TaskID", TaskID);
	bool Result = qSelect.ExecuteFirst();
	if (Result)
	{
		Result = qSelect.ReadColumn("count").toInt() > 0;
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISCore::TaskIsDuplicate(int TaskOriginalID, int TaskDuplicateID)
{
	ISQuery qSelectDuplicate(QS_TASK_DUPLICATE);
	qSelectDuplicate.BindValue(":OriginalID", TaskOriginalID);
	qSelectDuplicate.BindValue(":DuplicateID", TaskDuplicateID);
	bool Result = qSelectDuplicate.ExecuteFirst();
	if (Result)
	{
		Result = qSelectDuplicate.ReadColumn("count").toInt();
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCore::TaskInsertDuplicate(int TaskOriginalID, int TaskDuplicateID)
{
	ISQuery qInsertDuplicate(QI_TASK_DUPLICATE);
	qInsertDuplicate.BindValue(":OriginalID", TaskOriginalID);
	qInsertDuplicate.BindValue(":DuplicateID", TaskDuplicateID);
	qInsertDuplicate.Execute();
}
//-----------------------------------------------------------------------------
void ISCore::TaskInsertHistory(int TaskID, int UserID, const ISUuid &HistoryUID, const QString &Information)
{
	ISQueryPool::Instance().AddQuery(QI_TASK_HISTORY,
	{
		{ ":TaskID", TaskID },
		{ ":UserID", UserID },
		{ ":ActionUID", HistoryUID },
		{ ":Information", Information.isEmpty() ? QVariant() : Information }
	});
}
//-----------------------------------------------------------------------------
void ISCore::TaskInsertHistory(int TaskID, const ISUuid &HistoryUID, const QString &Information)
{
	TaskInsertHistory(TaskID, ISMetaUser::Instance().UserData->ID, HistoryUID, Information);
}
//-----------------------------------------------------------------------------
bool ISCore::TaskIsAttachedObject(int TaskID, const QString &TableName, int ObjectID)
{
	ISQuery qSelect(QS_TASK_ATTACH);
	qSelect.BindValue(":TaskID", TaskID);
	qSelect.BindValue(":TableName", TableName);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (qSelect.ExecuteFirst())
	{
		return qSelect.ReadColumn("count").toInt() > 0;
	}
	return false;
}
//-----------------------------------------------------------------------------
bool ISCore::TaskAttachObject(int TaskID, const QString &TableName, int ObjectID)
{
	ISQuery qInsert(QI_TASK_ATTACH);
	qInsert.BindValue(":TaskID", TaskID);
	qInsert.BindValue(":TableName", TableName);
	qInsert.BindValue(":ObjectID", ObjectID);
	return qInsert.Execute();
}
//-----------------------------------------------------------------------------
bool ISCore::TaskDetachObject(int TaskID, const QString &TableName, int ObjectID)
{
	ISQuery qDelete(QD_TASK_DETACH);
	qDelete.BindValue(":TaskID", TaskID);
	qDelete.BindValue(":TableName", TableName);
	qDelete.BindValue(":ObjectID", ObjectID);
	return qDelete.Execute();
}
//-----------------------------------------------------------------------------
int ISCore::TaskCountOverdue()
{
	int Result = 0;
	ISQuery qSelect(QS_COUNT_OVERDUE);
	if (qSelect.ExecuteFirst())
	{
		Result = qSelect.ReadColumn("count").toInt();
	}
	return Result;
}
//-----------------------------------------------------------------------------
