#include "ISCore.h"
#include "ISDefines.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISLocalization.h"
#include "ISBuffer.h"
#include "ISMetaData.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISQueryPool.h"
#include "ISProtocol.h"
#include "ISSystem.h"
#include "ISMemoryObjects.h"
#include "ISSettings.h"
#include "ISDebug.h"
#include "ISMetaUser.h"
#include "ISSettingsDatabase.h"
//-----------------------------------------------------------------------------
static QString Q_DELETE_OR_RECOVERY_OBJECT = "UPDATE %1 SET %2_isdeleted = :IsDeleted WHERE %2_id = :ObjectID";
//-----------------------------------------------------------------------------
static QString QD_OBJECT_CASCADE = "DELETE FROM %1 WHERE %2_id = :ObjectID";
//-----------------------------------------------------------------------------
static QString QU_OBJECT = "UPDATE %1 SET %2_deletiondate = now(), %2_deletionuser = CURRENT_USER WHERE %2_id = %3";
//-----------------------------------------------------------------------------
static QString QI_HISTORY = PREPARE_QUERY("INSERT INTO _history(htry_user, htry_tablename, htry_tablelocalname, htry_objectname, htry_objectid) "
										  "VALUES(:CurrentUserID, :TableName, :TableLocalName, :ObjectName, :ObjectID)");
//-----------------------------------------------------------------------------
static QString QD_HISTORY = PREPARE_QUERY("DELETE FROM _history WHERE htry_user = :CurrentUserID");
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
ISCore::ISCore()
{

}
//-----------------------------------------------------------------------------
ISCore::~ISCore()
{

}
//-----------------------------------------------------------------------------
bool ISCore::DeleteOrRecoveryObject(ISNamespace::DeleteRecoveryObject DeleteOrRecovery, const QString &TableName, const QString &TableAlias, int ID, const QString &LocalListName)
{
	bool Result = false;

	QString QueryText = Q_DELETE_OR_RECOVERY_OBJECT.arg(TableName).arg(TableAlias);

	ISQuery qDeleteOrRecovery(QueryText);
	qDeleteOrRecovery.BindValue(":ObjectID", ID);

	if (DeleteOrRecovery == ISNamespace::DeleteRecoveryObject::DRO_Delete)
	{
		qDeleteOrRecovery.BindValue(":IsDeleted", true);
	}
	else if (ISNamespace::DeleteRecoveryObject::DRO_Recovery)
	{
		qDeleteOrRecovery.BindValue(":IsDeleted", false);
	}

	Result = qDeleteOrRecovery.Execute();

	if (Result && DeleteOrRecovery == ISNamespace::DRO_Delete)
	{
		QString UpdateQuery = QU_OBJECT;
		UpdateQuery = UpdateQuery.arg(TableName);
		UpdateQuery = UpdateQuery.arg(TableAlias);
		UpdateQuery = UpdateQuery.arg(ID);

		ISQueryPool::GetInstance().AddQuery(UpdateQuery);
	}

	if (DeleteOrRecovery == ISNamespace::DRO_Delete)
	{
		ISProtocol::DeleteObject(TableName, LocalListName, ID);
	}
	else if (DeleteOrRecovery == ISNamespace::DRO_Recovery)
	{
		ISProtocol::RecoveryObject(TableName, LocalListName, ID);
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISCore::DeleteCascadeObject(const QString &TableName, const QString &TableAlias, int ObjectID)
{
	bool Result = false;

	QString QueryText = QD_OBJECT_CASCADE.arg(TableName).arg(TableAlias);
	ISQuery qDeleteCascade(QueryText);
	qDeleteCascade.BindValue(":ObjectID", ObjectID);
	if (qDeleteCascade.Execute())
	{
		Result = true;
	}

	return Result;
}
//-----------------------------------------------------------------------------
void ISCore::ChangeUser()
{
	RestartApplication();
}
//-----------------------------------------------------------------------------
void ISCore::RestartApplication()
{
	ExitApplication();
	ISSystem::SleepSeconds(1);
	QProcess::startDetached(APPLICATION_FILE_PATH);
}
//-----------------------------------------------------------------------------
void ISCore::ExitApplication()
{
	if (ISSystem::GetApplicationType() == ISNamespace::AT_GUI)
	{
		if (ISMemoryObjects::GetInstance().GetMainWindow())
		{
			ISMemoryObjects::GetInstance().GetMainWindow()->setProperty("CloseEvent", false);
		}

		qApp->closeAllWindows();
	}

	qApp->quit();
}
//-----------------------------------------------------------------------------
void ISCore::TerminateCurrentProcess()
{
	QStringList StringList;
	QString Program;
	
	if (ISSystem::GetCurrentOSType() == ISNamespace::OST_Windows)
	{
		Program = "TASKKILL";
		
		StringList.append("/PID");
		StringList.append(QString::number(CURRENT_PID));
		StringList.append("/F");
	}

	QProcess::execute(Program, StringList);
}
//-----------------------------------------------------------------------------
QString ISCore::GetObjectName(PMetaClassTable *MetaTable, int ObjectID)
{
	QString ObjectName;

	if (MetaTable->GetTitleName().length())
	{
		QString TitleName = MetaTable->GetTitleName();
		QStringList StringList = TitleName.split(";");
		QString QueryText = "SELECT ";

		if (StringList.count() > 1) //Если имя объекта строится из нескольких полей
		{
			QueryText += "concat(";
			for (int i = 0; i < StringList.count(); i++)
			{
				QueryText += MetaTable->GetAlias() + "_" + StringList.at(i) + ", ' ', ";
			}

			ISSystem::RemoveLastSymbolFromString(QueryText, 7);
			QueryText += ") \n";
		}
		else //Если указано только одно поле
		{
			QueryText += MetaTable->GetAlias() + "_" + TitleName + " \n";
		}

		QueryText += "FROM " + MetaTable->GetName() + " \n";
		QueryText += "WHERE " + MetaTable->GetAlias() + "_id = :ObjectID";

		ISQuery qSelectName(QueryText);
		qSelectName.BindValue(":ObjectID", ObjectID);
		if (qSelectName.ExecuteFirst())
		{
			ObjectName = qSelectName.ReadColumn(0).toString();
		}
	}
	else
	{
		ObjectName = QString::number(ObjectID);
	}

	return ObjectName;
}
//-----------------------------------------------------------------------------
QString ISCore::GetVersionInFileName(const QString &FileName)
{
	QStringList StringListFile = FileName.split("_");
	QString Version = StringListFile[StringListFile.length() - 1];
	QStringList StringListVersion = Version.split(".");

	QString VersionComplete;
	for (int i = 0; i < StringListVersion.count(); i++)
	{
		QString String = StringListVersion.at(i);
		VersionComplete += String + ".";
	}

	ISSystem::RemoveLastSymbolFromString(VersionComplete);
	return VersionComplete;
}
//-----------------------------------------------------------------------------
QString ISCore::ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat)
{
	return ConvertDateToString(DateTime.date(), DateFormat) + " " + LOCALIZATION("At") + " " + DateTime.time().toString(TimeFormat);
}
//-----------------------------------------------------------------------------
QString ISCore::ConvertDateToString(const QDate &Date, const QString &DateFormat)
{
	QString Result;
	if (Date == DATE_YESTERDAY) //Вчера
	{
		Result = LOCALIZATION("Yesterday");
	}
	else if (Date == DATE_TODAY) //Сегодня
	{
		Result = LOCALIZATION("Today");
	}
	else if (Date == DATE_TOMORROW) //Завтра
	{
		Result = LOCALIZATION("Tomorrow");
	}
	else
	{
		Result = Date.toString(DateFormat);
	}
	return Result;
}
//-----------------------------------------------------------------------------
void ISCore::ExecuteStartCommand()
{
	if (SETTING_BOOL(CONST_UID_SETTING_EVENTS_EVENT_AT_STARTUP))
	{
		QString CommandText = SETTING_STRING(CONST_UID_SETTING_EVENTS_STARTUP_COMMAND);
		if (CommandText.length())
		{
			ISDebug::ShowDebugString(QString("Executing command: %1").arg(CommandText));
			int ExitCode = QProcess::execute("cmd.exe", QStringList() << "/C" << CommandText);
			if (ExitCode)
			{
				ISDebug::ShowDebugString("Executed command error.");
			}
			else
			{
				ISDebug::ShowDebugString("Executed command done.");
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISCore::ExecuteExitComamnd()
{
	if (SETTING_BOOL(CONST_UID_SETTING_EVENTS_EVENT_ON_EXIT))
	{
		QString CommandText = SETTING_STRING(CONST_UID_SETTING_EVENTS_EXIT_COMMAND);
		if (CommandText.length())
		{
			ISDebug::ShowDebugString(QString("Executing command: %1").arg(CommandText));
			int ExitCode = QProcess::execute("cmd.exe", QStringList() << "/C" << CommandText);
			if (ExitCode)
			{
				ISDebug::ShowDebugString("Executed command error.");
			}
			else
			{
				ISDebug::ShowDebugString("Executed command done.");
			}
		}
	}
}
//-----------------------------------------------------------------------------
void ISCore::AddHistory(const QString &TableName, const QString &LocalListName, const QString &ObjectName, int ObjectID)
{
	QVariantMap Parameters;
	Parameters.insert(":CurrentUserID", CURRENT_USER_ID);
	Parameters.insert(":TableName", TableName);
	Parameters.insert(":TableLocalName", LocalListName);
	Parameters.insert(":ObjectName", ObjectName);
	Parameters.insert(":ObjectID", ObjectID);

	ISQueryPool::GetInstance().AddQuery(QI_HISTORY, Parameters);
}
//-----------------------------------------------------------------------------
void ISCore::ClearHistory()
{
	QVariantMap Parameters;
	Parameters.insert(":CurrentUserID", CURRENT_USER_ID);

	ISQueryPool::GetInstance().AddQuery(QD_HISTORY, Parameters);
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

	if (TableName.length())
	{
		IS_ASSERT(ObjectID, "Invalid object id");

		qInsertCalendar.BindValue(":TableName", TableName);
		qInsertCalendar.BindValue(":ObjectID", ObjectID);
	}
	else
	{
		qInsertCalendar.BindValue(":TableName", QVariant());
		qInsertCalendar.BindValue(":ObjectID", QVariant());
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
	QString Login = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_SMSSERVICE_LOGIN);
	QString Password = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_SMSSERVICE_PASSWORD);
	QString Charset = SETTING_DATABASE_VALUE_STRING(CONST_UID_DATABASE_SETTING_SMSSERVICE_ENCODING);
	SMSSend(Login, Password, Phone, Message, Charset);
}
//-----------------------------------------------------------------------------
ISUuid ISCore::TaskGetStatusUID(int TaskID)
{
	ISUuid StatusUID;

	ISQuery qSelectStatus(QS_TASK_STATUS);
	qSelectStatus.BindValue(":TaskID", TaskID);
	if (qSelectStatus.ExecuteFirst())
	{
		StatusUID = qSelectStatus.ReadColumn("tsst_uid");
	}

	return StatusUID;
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
	bool Result = false;

	ISQuery qSelect(QS_TASK_COUNT);
	qSelect.BindValue(":TaskID", TaskID);
	if (qSelect.ExecuteFirst())
	{
		if (qSelect.ReadColumn("count").toInt())
		{
			return true;
		}
	}

	return Result;
}
//-----------------------------------------------------------------------------
bool ISCore::TaskIsDuplicate(int TaskOriginalID, int TaskDuplicateID)
{
	ISQuery qSelectDuplicate(QS_TASK_DUPLICATE);
	qSelectDuplicate.BindValue(":OriginalID", TaskOriginalID);
	qSelectDuplicate.BindValue(":DuplicateID", TaskDuplicateID);
	if (qSelectDuplicate.ExecuteFirst())
	{
		if (qSelectDuplicate.ReadColumn("count").toInt())
		{
			return true;
		}
	}

	return false;
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
	QVariantMap VariantMap;
	VariantMap.insert(":TaskID", TaskID);
	VariantMap.insert(":UserID", UserID);
	VariantMap.insert(":ActionUID", HistoryUID);
	if (Information.length())
	{
		VariantMap.insert(":Information", Information);
	}
	else
	{
		VariantMap.insert(":Information", QVariant());
	}

	ISQueryPool::GetInstance().AddQuery(QI_TASK_HISTORY, VariantMap);
}
//-----------------------------------------------------------------------------
void ISCore::TaskInsertHistory(int TaskID, const ISUuid &HistoryUID, const QString &Information)
{
	TaskInsertHistory(TaskID, CURRENT_USER_ID, HistoryUID, Information);
}
//-----------------------------------------------------------------------------
bool ISCore::TaskIsAttachedObject(int TaskID, const QString &TableName, int ObjectID)
{
	bool Result = false;
	ISQuery qSelect(QS_TASK_ATTACH);
	qSelect.BindValue(":TaskID", TaskID);
	qSelect.BindValue(":TableName", TableName);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (qSelect.ExecuteFirst())
	{
		int Count = qSelect.ReadColumn("count").toInt();
		if (Count)
		{
			Result = true;
		}
	}

	return Result;
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
