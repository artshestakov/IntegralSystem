#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISTrace.h"
#include "ISTcpQueue.h"
#include "ISVersionInfo.h"
#include "ISConfig.h"
#include "ISTcpClients.h"
#include "ISMetaData.h"
#include "ISMetaDataHelper.h"
#include "ISFail2Ban.h"
#include "ISQueryModel.h"
//-----------------------------------------------------------------------------
static QString QS_USERS_HASH = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
											 "FROM _users "
											 "WHERE usrs_hash IS NOT NULL "
											 "AND usrs_salt IS NOT NULL");
//-----------------------------------------------------------------------------
static QString QS_USER_AUTH = PREPARE_QUERY("SELECT usrs_id, usrs_issystem, usrs_group, usrs_fio, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, usgp_fullaccess, "
											"(SELECT sgdb_useraccessdatabase FROM _settingsdatabase WHERE sgdb_active) "
											"FROM _users "
											"LEFT JOIN _usergroup ON usgp_id = usrs_group "
											"WHERE usrs_hash = :Hash");
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("INSERT INTO _protocol(prtc_datetime, prtc_user, prtc_tablename, prtc_tablelocalname, prtc_type, prtc_objectid, prtc_information) "
										   "VALUES(:DateTime, :UserID, :TableName, :TableLocalName, (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = :TypeUID), :ObjectID, :Information)");
//-----------------------------------------------------------------------------
static QString QS_SETTINGS_DATABASE = PREPARE_QUERY("SELECT sgdb_settingname, sgdb_useraccessdatabase, sgdb_numbersimbolsaftercomma, sgdb_storagefilemaxsize "
													"FROM _settingsdatabase "
													"WHERE sgdb_active");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE = PREPARE_QUERY("SELECT gatb_table, gatt_uid "
													 "FROM _groupaccesstable "
													 "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_AccessType "
													 "WHERE gatb_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("SELECT gast_uid "
													   "FROM _groupaccessspecial "
													   "LEFT JOIN _groupaccessspecialtype ON gast_id = gasp_specialaccess "
													   "WHERE gasp_group = :GroupID");
//-----------------------------------------------------------------------------
static QString QS_SYSTEM = PREPARE_QUERY("SELECT stms_issystem, stms_uid, stms_localname, stms_icon, stms_hint "
										 "FROM _systems "
										 "ORDER BY stms_orderid");
//-----------------------------------------------------------------------------
static QString QS_SUBSYSTEM = PREPARE_QUERY("SELECT sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
											"FROM _subsystems "
											"WHERE sbsm_system = :SystemUID "
											"AND check_access_user_subsystem(:UserID, :UserIsSystem, sbsm_uid) "
											"ORDER BY sbsm_orderid");
//-----------------------------------------------------------------------------
static QString QS_REPORT = PREPARE_QUERY("SELECT rprt_uid, rprt_type, rprt_tablename, rprt_localname, rprt_filetemplate "
										 "FROM _report "
										 "WHERE rprt_parent IS NULL "
										 "ORDER BY rprt_id");
//-----------------------------------------------------------------------------
static QString QS_REPORT_FIELD = PREPARE_QUERY("SELECT rprt_replacevalue, rprt_sqlquery "
											   "FROM _report "
											   "WHERE rprt_parent = :ParentUID");
//-----------------------------------------------------------------------------
static QString QS_FAVORITE = PREPARE_QUERY("SELECT fvts_tablename, fvts_objectsid "
										   "FROM _favorites "
										   "WHERE fvts_user = :UserID");
//-----------------------------------------------------------------------------
static QString QS_HISTORY = PREPARE_QUERY("SELECT htry_datetime, htry_tablename, htry_objectid "
										  "FROM _history "
										  "WHERE htry_user = :UserID "
										  "ORDER BY htry_id");
//-----------------------------------------------------------------------------
static QString QS_SORTINGS = PREPARE_QUERY("SELECT sgts_tablename, sgts_fieldname, sgts_sorting "
										   "FROM _sortingtables "
										   "WHERE sgts_user = :UserID");
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_tablename, clsz_fieldname, clsz_size "
											  "FROM _columnsize "
											  "WHERE clsz_user = :UserID");
//-----------------------------------------------------------------------------
static QString QS_SETTING_GROUP = PREPARE_QUERY("SELECT stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint "
												"FROM _settingsgroup "
												"ORDER BY stgp_order");
//-----------------------------------------------------------------------------
static QString QS_SETTING = PREPARE_QUERY("SELECT stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
										  "usst_value, "
										  "(SELECT COUNT(*) FROM _usersettings WHERE usst_user = :UserID AND usst_setting = stgs_id) "
										  "FROM _settings "
										  "LEFT JOIN _usersettings ON usst_setting = stgs_id AND usst_user = :UserID "
										  "WHERE stgs_group = :GroupUID "
										  "ORDER BY stgs_order");
//-----------------------------------------------------------------------------
static QString QI_USER_SETTING = PREPARE_QUERY("INSERT INTO _usersettings(usst_user, usst_setting, usst_value) "
											   "VALUES(:UserID, (SELECT stgs_id FROM _settings WHERE stgs_uid = :SettingUID), :Value)");
//-----------------------------------------------------------------------------
static QString QS_PARAGRAPH = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname "
											"FROM _paragraphs "
											"ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
static QString QU_USER_HASH = PREPARE_QUERY("UPDATE _users SET "
											"usrs_hash = :Hash, "
											"usrs_salt = :Salt "
											"WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_USER_PASSWORD = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
												"FROM _users "
												"WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QU_USER_HASH_RESET = PREPARE_QUERY("UPDATE _users SET "
												  "usrs_hash = NULL, "
												  "usrs_salt = NULL "
												  "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QU_USER_SETTINGS_RESET = PREPARE_QUERY("UPDATE _usersettings SET "
													  "usst_value = (SELECT stgs_defaultvalue FROM _settings WHERE stgs_id = usst_setting) "
													  "WHERE usst_user = :UserID "
													  "RETURNING (SELECT stgs_uid FROM _settings WHERE stgs_id = usst_setting), usst_value");
//-----------------------------------------------------------------------------
static QString QS_ASTERISK_RECORD = PREPARE_QUERY("SELECT ascl_uniqueid "
												  "FROM _asteriskcalls "
												  "WHERE ascl_id = :RecordID");
//-----------------------------------------------------------------------------
static QString QS_USER_PASSWORD_IS_NULL = PREPARE_QUERY("SELECT "
														"( "
														"SELECT (COUNT(*) > 0)::BOOLEAN is_exist "
														"FROM _users "
														"WHERE usrs_id = :UserID "
														"AND usrs_hash IS NOT NULL "
														"AND usrs_salt IS NOT NULL "
														") "
														"FROM _users "
														"WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_USER_IS_SYSTEM = PREPARE_QUERY("SELECT usrs_issystem "
												 "FROM _users "
												 "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QS_CLIENT = PREPARE_QUERY("SELECT usrs_fio, usrs_photo "
										 "FROM _users "
										 "WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QI_DISCUSSION = PREPARE_QUERY("INSERT INTO _discussion(dson_user, dson_tablename, dson_objectid, dson_message) "
											 "VALUES(:UserID, :TableName, :ObjectID, :Message) "
											 "RETURNING dson_id");
//-----------------------------------------------------------------------------
static QString QU_DISCUSSION = PREPARE_QUERY("UPDATE _discussion SET "
											 "dson_message = :Message "
											 "WHERE dson_id = :DiscussionID");
//-----------------------------------------------------------------------------
static QString QI_DISCUSSION_COPY = PREPARE_QUERY("INSERT INTO _discussion(dson_user, dson_tablename, dson_objectid, dson_message) "
												  "SELECT dson_user, dson_tablename, dson_objectid, dson_message "
												  "FROM _discussion "
												  "WHERE dson_id = :DiscussionID "
												  "RETURNING dson_id");
//-----------------------------------------------------------------------------
static QString QS_SORTING = PREPARE_QUERY("SELECT sgts_fieldname, sgts_sorting "
										  "FROM _sortingtables "
										  "WHERE sgts_user = :UserID "
										  "AND sgts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QU_SORTING = PREPARE_QUERY("UPDATE _sortingtables SET "
										  "sgts_fieldname = :FieldName, "
										  "sgts_sorting = :Sorting "
										  "WHERE sgts_user = :UserID "
										  "AND sgts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QI_SORTING = PREPARE_QUERY("INSERT INTO _sortingtables(sgts_user, sgts_tablename, sgts_fieldname, sgts_sorting)"
										  "VALUES(:UserID, :TableName, :FieldName, :Sorting)");
//-----------------------------------------------------------------------------
static QString QS_COLUMN_SIZE_TABLE = PREPARE_QUERY("SELECT clsz_fieldname, clsz_size "
													"FROM _columnsize "
													"WHERE clsz_user = :UserID");
//-----------------------------------------------------------------------------
static QString QS_NOTE_RECORD = PREPARE_QUERY("SELECT nobj_note "
											  "FROM _noteobject "
											  "WHERE nobj_tablename = :TableName "
											  "AND nobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QD_NOTE_RECORD = PREPARE_QUERY("DELETE FROM _noteobject "
											  "WHERE nobj_tablename = :TableName "
											  "AND nobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QU_NOTE_RECORD = PREPARE_QUERY("UPDATE _noteobject SET "
											  "nobj_note = :Note "
											  "WHERE nobj_tablename = :TableName "
											  "AND nobj_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QI_NOTE_RECORD = PREPARE_QUERY("INSERT INTO _noteobject(nobj_tablename, nobj_objectid, nobj_note) "
											  "VALUES(:TableName, :ObjectID, :Note)");
//-----------------------------------------------------------------------------
static QString QI_FILE_STORAGE = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_owner, sgfs_name, sgfs_expansion, sgfs_size, sgfs_data) "
											   "VALUES (:Owner, :Name, :Expansion, :Size, :Data) "
											   "RETURNING sgfs_id");
//-----------------------------------------------------------------------------
static QString QI_FILE_STORAGE_COPY = PREPARE_QUERY("INSERT INTO _storagefiles(sgfs_owner, sgfs_name, sgfs_expansion, sgfs_size, sgfs_note, sgfs_data) "
													"SELECT sgfs_owner, :Name, sgfs_expansion, sgfs_size, sgfs_note, sgfs_data "
													"FROM _storagefiles "
													"WHERE sgfs_id = :ObjectID "
													"RETURNING sgfs_id");
//-----------------------------------------------------------------------------
static QString QS_FILE_STORAGE = PREPARE_QUERY("SELECT sgfs_id, sgfs_data "
											   "FROM _storagefiles "
											   "WHERE sgfs_id = :ObjectID");
//-----------------------------------------------------------------------------
static QString QS_TASK_SEARCH_TEXT = PREPARE_QUERY("WITH w AS "
												   "( "
												   "SELECT task_id, task_parent, lower(task_name) AS search_field "
												   "FROM _task "
												   "UNION "
												   "SELECT task_id, task_parent, lower(task_description) AS search_field "
												   "FROM _task "
												   "WHERE task_description IS NOT NULL "
												   "UNION "
												   "SELECT tcom_task AS task_id, task_parent, lower(tcom_comment) AS search_field "
												   "FROM _taskcomment "
												   "LEFT JOIN _task ON tcom_task = task_id "
												   "UNION "
												   "SELECT tfls_task AS task_id, task_parent, lower(tfls_name) AS search_field "
												   "FROM _taskfile "
												   "LEFT JOIN _task ON tfls_task = task_id "
												   ") "
												   "SELECT DISTINCT task_id, task_parent, (SELECT task_name FROM _task WHERE task_id = w.task_id) "
												   "FROM w "
												   "WHERE search_field LIKE '%' || lower(:Value) || '%' "
												   "ORDER BY task_id");
//-----------------------------------------------------------------------------
static QString QS_TASK_SEARCH_ID = PREPARE_QUERY("SELECT (COUNT(*) > 0)::BOOLEAN AS result "
												 "FROM _task "
												 "WHERE task_id = :ID");
//-----------------------------------------------------------------------------
static QString QS_CALENDAR_EVENTS = PREPARE_QUERY("SELECT cldr_id, EXTRACT(DAY FROM cldr_date) AS day, cldr_timealert, cldr_name, cldr_text, cldr_closed "
												  "FROM _calendar "
												  "WHERE cldr_user = :UserID "
												  "AND cldr_date "
												  "BETWEEN to_timestamp('01' || to_char(:Month::INTEGER, '09') || :Year, 'DDMMYYYY')::DATE "
												  "AND to_timestamp('01' || to_char(:Month::INTEGER, '09') || :Year, 'DDMMYYYY')::DATE + INTERVAL '1 MONTH - 1 day' "
												  "ORDER BY cldr_date, cldr_timealert");
//-----------------------------------------------------------------------------
static QString QD_CALENDAR = PREPARE_QUERY("DELETE FROM _calendar "
										   "WHERE cldr_id = :ObjectID "
										   "RETURNING cldr_name");
//-----------------------------------------------------------------------------
static QString QS_INTERNAL_LISTS = PREPARE_QUERY("SELECT intd_tablename "
												 "FROM _internaldirectories "
												 "ORDER BY intd_order");
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password)
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	DBHost(db_host),
	DBPort(db_port),
	DBName(db_name),
	DBUser(db_user),
	DBPassword(db_password),
	IsStarted(false),
	IsRunning(false),
	CurrentMessage(nullptr),
	IsStopped(false)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpWorker::~ISTcpWorker()
{
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetStarted() const
{
	return IsStarted;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRunning()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool is_running = IsRunning;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return is_running;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetMessage(ISTcpMessage *TcpMessage)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	CurrentMessage = TcpMessage;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Run()
{
	//Поток, связанный с этим воркером будет удалятсья из памяти после завершения
	connect(thread(), &QThread::finished, thread(), &QThread::deleteLater);

	//Формируем имя подключения к БД
	DBConnectionName = QString::number(CURRENT_THREAD_ID());
	
	//Пытаемся подключиться к БД
	IsStarted = ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword);
	if (!IsStarted)
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
	}

	qProtocol = new ISQuery(ISDatabase::Instance().GetDB(DBConnectionName), QI_PROTOCOL);

	//Сигналим об успехе или ошибке
	emit IsStarted ? StartedDone() : StartedFailed();
	if (IsStarted)
	{
		ISLOGGER_I(__CLASS__, "Started");
		Process();
	}
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Stop()
{
	bool is_running = true;
	while (is_running)
	{
		ISSleep(1);
		CRITICAL_SECTION_LOCK(&CriticalSection);
		is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
	}
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsStopped = true;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Process()
{
	while (true)
	{
		ISSleep(1); //Засыпаем на милисекунду и даём поработать другим потокам

		//Проверяем, не остановлен ли воркер
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_stopped = IsStopped;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (is_stopped) //Если флаг остановки установлен - выходим из цикла
		{
			ISLOGGER_I(__CLASS__, QString("Stopping %1...").arg(DBConnectionName));
			delete qProtocol;
			ISDatabase::Instance().Disconnect(DBConnectionName);
			break;
		}

		//Получаем текущее сообщение
		CRITICAL_SECTION_LOCK(&CriticalSection);
		ISTcpMessage *tcp_message = CurrentMessage;
		if (tcp_message) //Если сообщение есть - устанавливаем флаг работы воркера
		{
			IsRunning = true;
			CurrentMessage = nullptr;
		}
		CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//Если появилось сообщение на обработку - выполняем его
		if (tcp_message)
		{
			bool Result = false;
			long long PerfomanceMsec = 0;
			ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpSocket); //Заранее формируем ответ

			if (tcp_message->IsValid()) //Если сообщение валидное - переходим к выполнению
			{
				//Если запрос не авторизационный и клиент ещё не авторизовался - ошибка
				if (tcp_message->Type != ISNamespace::AMT_Auth && !tcp_message->TcpSocket->GetAuthorized())
				{
					ErrorString = LANG("Carat.Error.NotAuthorized");
				}
				else //Клиент авторизовался - продолжаем
				{
					ISTimePoint TimePoint = ISAlgorithm::GetTick(); //Запоминаем текущее время
					switch (tcp_message->Type)
					{
					case ISNamespace::AMT_Unknown: break;
					case ISNamespace::AMT_Auth: Result = Auth(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_Sleep: Result = Sleep(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetMetaData: Result = GetMetaData(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetLastClient: Result = GetLastClient(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordExist: Result = UserPasswordExist(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordCreate: Result = UserPasswordCreate(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordEdit: Result = UserPasswordEdit(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserPasswordReset: Result = UserPasswordReset(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_UserSettingsReset: Result = UserSettingsReset(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetRecordCall: Result = GetRecordCall(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetClients: Result = GetClients(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_RecordDelete: Result = RecordDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionAdd: Result = DiscussionAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionEdit: Result = DiscussionEdit(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionCopy: Result = DiscussionCopy(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetTableData: Result = GetTableData(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_NoteRecordGet: Result = NoteRecordGet(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_NoteRecordSet: Result = NoteRecordSet(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_FileStorageAdd: Result = FileStorageAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_FileStorageCopy: Result = FileStorageCopy(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_FileStorageGet: Result = FileStorageGet(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_SearchTaskText: Result = SearchTaskText(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_SearchTaskID: Result = SearchTaskID(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_SearchFullText: Result = SearchFullText(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetCalendarEvents: Result = GetCalendarEvents(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_CalendarDelete: Result = CalendarDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetInternalLists: Result = GetInternalLists(tcp_message, TcpAnswer); break;
					default:
						ErrorString = LANG("Carat.Error.NotExistFunction").arg(tcp_message->TypeName);
					}
					PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
				}
			}
			else //Сообщение не валидное
			{
				ErrorString = tcp_message->GetErrorString();
			}

			//Формируем лог-сообщение
			QString LogText = QString("%1 message \"%2\". ID: %3 Size: %4 Chunk: %5 Parse msec: %6 MSec: %7").
				arg(Result ? "Done" : "Failed").
				arg(tcp_message->TypeName).
				arg(tcp_message->MessageID).
				arg(tcp_message->Size).
				arg(tcp_message->ChunkCount).
				arg(tcp_message->ParseMSec).
				arg(PerfomanceMsec);
			if (!Result) //Запрос выполнен с ошибкой - устанавливаем текст ошибки в ответе и лог-сообщении, а так же очищаем потенциально не пустые параметры ответа
			{
				TcpAnswer->SetError(ErrorString);
				LogText.append("\nError string: " + ErrorString);
				ErrorString.clear();
				TcpAnswer->Parameters.clear();
			}

			//Удаляем сообщение, логируемся, добавляем ответ в очередь ответов и завершаем работу
			delete tcp_message;
			Result ? ISLOGGER_I(__CLASS__, LogText) : ISLOGGER_E(__CLASS__, LogText);
			emit Answer(TcpAnswer);
			Finish();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Finish()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
QVariant ISTcpWorker::CheckNullField(const QString &FieldName, ISTcpMessage *TcpMessage)
{
	if (TcpMessage->Parameters.contains(FieldName))
	{
		QVariant Value = TcpMessage->Parameters[FieldName];
		if (!Value.toString().isEmpty())
		{
			return Value;
		}
		else
		{
			ErrorString = LANG("Carat.Error.FieldIsEmpty").arg(FieldName);
		}
	}
	else
	{
		ErrorString = LANG("Carat.Error.FieldNotExist").arg(FieldName);
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Protocol(int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName, const QVariant &TableLocalName, const QVariant &ObjectID, const QVariant &Information)
{
	qProtocol->BindValue(":DateTime", QDateTime::currentDateTime());
	qProtocol->BindValue(":UserID", UserID);
	qProtocol->BindValue(":TableName", TableName);
	qProtocol->BindValue(":TypeUID", ActionTypeUID);
	qProtocol->BindValue(":ObjectID", ObjectID);
	qProtocol->BindValue(":TableLocalName", TableLocalName);
	qProtocol->BindValue(":Information", Information);
	if (!qProtocol->Execute()) //Не удалось добавить запись в протокол
	{
		ISLOGGER_E(__CLASS__, "Not insert protocol: " + qProtocol->GetErrorString());
	}
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(const QVariant &UserID, bool &Exist)
{
	ISQuery qSelectHashIsNull(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_PASSWORD_IS_NULL);
	qSelectHashIsNull.BindValue(":UserID", UserID);
	if (!qSelectHashIsNull.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.CheckExistUserPassword").arg(qSelectHashIsNull.GetErrorString());
		return false;
	}

	if (!qSelectHashIsNull.First()) //Не удалось перейти на первую строку, т.к. пользователя с таким UserID не существует
	{
		ErrorString = LANG("Carat.Error.UserNotExist").arg(UserID.toInt());
		return false;
	}
	Exist = qSelectHashIsNull.ReadColumn("is_exist").toBool();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserIsSystem(const QVariant &UserID, bool &IsSystem)
{
	ISQuery qSelectIsSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_IS_SYSTEM);
	qSelectIsSystem.BindValue(":UserID", UserID);
	if (!qSelectIsSystem.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.CheckUserIsSystem").arg(qSelectIsSystem.GetErrorString());
		return false;
	}

	if (!qSelectIsSystem.First())
	{
		ErrorString = LANG("Carat.Error.UserNotExist").arg(UserID.toInt());
		return false;
	}
	IsSystem = qSelectIsSystem.ReadColumn("usrs_issystem").toBool();
	return true;
}
//-----------------------------------------------------------------------------
QString ISTcpWorker::ConvertDateTimeToString(const QDateTime &DateTime, const QString &DateFormat, const QString &TimeFormat)
{
	return DateTime.isValid() ?
		ConvertDateToString(DateTime.date(), DateFormat) + SYMBOL_SPACE + LANG("Carat.In") + SYMBOL_SPACE + DateTime.time().toString(TimeFormat) :
		QString();
}
//-----------------------------------------------------------------------------
QString ISTcpWorker::ConvertDateToString(const QDate &Date, const QString &DateFormat)
{
	QString Result;
	if (Date == QDate::currentDate().addDays(-1)) //Вчера
	{
		Result = LANG("Carat.Yesterday");
	}
	else if (Date == QDate::currentDate()) //Сегодня
	{
		Result = LANG("Carat.Today");
	}
	else if (Date == QDate::currentDate().addDays(1)) //Завтра
	{
		Result = LANG("Carat.Tomorrow");
	}
	else
	{
		Result = Date.toString(DateFormat);
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetObjectName(PMetaTable *MetaTable, unsigned int ObjectID, QString &ObjectName)
{
	//Если TitleName у мета-таблицы не заполнен - возвращаем идентификатор объекта
	if (MetaTable->TitleName.isEmpty())
	{
		ObjectName = QString::number(ObjectID);
		return true;
	}

	//Формируем запрос
	QString QueryText = "SELECT ";
	QStringList StringList = MetaTable->TitleName.split(';');
	if (StringList.count() > 1) //Если имя объекта строится из нескольких полей
	{
		QueryText += "concat(";
		for (const QString &FieldName : StringList) //Обход полей
		{
			PMetaForeign *MetaForeign = MetaTable->GetField(FieldName)->Foreign;
			QueryText += MetaForeign ?
				("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "), ' ', ") :
				(MetaTable->Alias + '_' + FieldName + ", ' ', ");
		}
		QueryText.chop(7);
		QueryText += ") \n";
	}
	else //Если указано только одно поле
	{
		PMetaForeign *MetaForeign = MetaTable->GetField(MetaTable->TitleName)->Foreign;
		QueryText += MetaForeign ?
			("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, MetaTable->TitleName) + ") \n") :
			(MetaTable->Alias + '_' + MetaTable->TitleName + " \n");
	}
	QueryText += "FROM " + MetaTable->Name + " \n";
	QueryText += "WHERE " + MetaTable->Alias + "_id = :ObjectID";

	//Запрашиваем имя
	ISQuery qSelectName(ISDatabase::Instance().GetDB(DBConnectionName), QueryText);
	qSelectName.SetShowLongQuery(false);
	qSelectName.BindValue(":ObjectID", ObjectID);
	if (!qSelectName.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.GetObjectName").arg(qSelectName.GetErrorString());
		return false;
	}

	if (!qSelectName.First()) //Запись не найдена
	{
		return true;
	}
	
	//Анализируем
	QVariant Value = qSelectName.ReadColumn(0);
	switch (Value.type())
	{
	case QVariant::Date: ObjectName = Value.toDate().toString(FORMAT_DATE_V2); break;
	case QVariant::Time: ObjectName = Value.toTime().toString(FORMAT_TIME_V1); break;
	case QVariant::DateTime: ObjectName = Value.toDateTime().toString(FORMAT_DATE_TIME_V2); break;
	default:
		ObjectName = qSelectName.ReadColumn(0).toString();
	}

	//Удаляем возможные пробелы в конце имени объекта
	ISAlgorithm::RemoveLastSymbolLoop(ObjectName, SYMBOL_SPACE);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//Проверяем, не авторизаван ли уже клиент. Если авторизован - выходим с ошибкой
	if (TcpMessage->TcpSocket->GetAuthorized())
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AlreadyAuthorized");
		return false;
	}

	//Если дата запрета меньше чем текущая - не даём зайти в программу
	if (ISVersionInfo::Instance().ConfigurationInfo.DateExpired.isValid() &&
		QDate::currentDate() > ISVersionInfo::Instance().ConfigurationInfo.DateExpired)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AuthDenied");
		return false;
	}

	QVariant Hash = CheckNullField("Hash", TcpMessage);
	if (!Hash.isValid())
	{
		return false;
	}
	QString HashString = Hash.toString();

	//Проверяем размер хэша
	if ((size_t)HashString.size() != CARAT_HASH_SIZE)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.InvalidHashSize");
		return false;
	}

	//Проверяем валидность хэша
	for (const QChar &Char : HashString)
	{
		int ASCII = (int)Char.toLatin1(); //Преобразовываем текущий символ в ASCII-код
		if ((ASCII >= 48 && ASCII <= 57) || (ASCII >= 65 && ASCII <= 70)) //Если текущий символ входит в диапазон [0-9] или [a-z] - все окей
		{
			continue;
		}
		else //Иначе - хэш невалидный
		{
			ErrorString = LANG("Carat.Error.Query.Auth.InvalidHash");
			return false;
		}
	}

	{
		//Запрашиваем все хэши из БД
		ISQuery qSelectHash(ISDatabase::Instance().GetDB(DBConnectionName), QS_USERS_HASH);
		if (!qSelectHash.Execute()) //Ошибка запроса
		{
			ErrorString = LANG("Carat.Error.Query.Auth.SelectHash").arg(qSelectHash.GetErrorString());
			return false;
		}

		//Если запрос ничего не вернул, значит в БД нет ни одного пользователя
		if (qSelectHash.GetCountResultRows() == 0)
		{
			ErrorString = LANG("Carat.Error.Query.Auth.NoUsers");
			return false;
		}

		//Ищем пользователя
		bool IsFound = false;
		while (qSelectHash.Next())
		{
			//Получаем хэш и соль
			QString CurrentHash = qSelectHash.ReadColumn("usrs_hash").toString(),
				CurrentSalt = qSelectHash.ReadColumn("usrs_salt").toString();

			//Солим присланный хэш текущей солью
			QString HashResult = ISAlgorithm::SaltPassword(HashString, CurrentSalt);
			IsFound = HashResult == CurrentHash;
			if (IsFound) //Нашли
			{
				Hash = HashResult;
				break;
			}
		}

		QString IPAddress = TcpMessage->TcpSocket->GetAddress();
		if (IsFound) //Нашли пользователя - удаляем адрес из Fail2Ban
		{
			ISFail2Ban::Instance().Remove(IPAddress);
		}
		else //Не нашли пользователя - добавляем адрес в Fail2Ban
		{
			ISFail2Ban::Instance().Add(IPAddress);

			//Если адрес заблокирован - сообщаем об этом, иначе - предупреждаем о неправильном вводе логина или пароля
			ErrorString = ISFail2Ban::Instance().IsLock(IPAddress)
				? LANG("Carat.Error.Query.Auth.Fail2Ban").arg(ISFail2Ban::Instance().GetUnlockDateTime(IPAddress).toString(FORMAT_DATE_TIME_V2))
				: LANG("Carat.Error.Query.Auth.InvalidLoginOrPassword");
			return false;
		}
	}

	//Проверка пользователя
	ISQuery qSelectAuth(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_AUTH);
	qSelectAuth.BindValue(":Hash", Hash);
	if (!qSelectAuth.ExecuteFirst()) //Запрос выполнен с ошибкой
	{
		ErrorString = LANG("Carat.Error.Query.Auth.SelectLogin").arg(qSelectAuth.GetErrorString());
		return false;
	}

	int UserID = qSelectAuth.ReadColumn("usrs_id").toInt();
	bool IsSystem = qSelectAuth.ReadColumn("usrs_issystem").toBool();
	QString UserFIO = qSelectAuth.ReadColumn("usrs_fio").toString();
	int GroupID = qSelectAuth.ReadColumn("usrs_group").toInt();
	bool GroupFullAccess = qSelectAuth.ReadColumn("usgp_fullaccess").toBool();

	//Доступ к БД запрещен
	if (!qSelectAuth.ReadColumn("sgdb_useraccessdatabase").toBool() && !IsSystem)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.ConnectionBan");
		return false;
	}

	//Если у пользователя нет права доступа
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.Auth.LoginNoAccess");
		return false;
	}

	//Проверка наличия привязки не системного пользователя к группе
	if (!IsSystem && GroupID == 0)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.LoginLinkGroup");
		return false;
	}

	//Если для пользователя настроено ограничение срока действия учётной записи
	if (qSelectAuth.ReadColumn("usrs_accountlifetime").toBool())
	{
		QDate DateStart = qSelectAuth.ReadColumn("usrs_accountlifetimestart").toDate();
		QDate DateEnd = qSelectAuth.ReadColumn("usrs_accountlifetimeend").toDate();
		if (DateStart.isValid() && DateEnd.isValid()) //Если дата начала и окончания срока действия установлена
		{
			QDate CurrentDate = QDate::currentDate();
			if (CurrentDate < DateStart)
			{
				ErrorString = LANG("Carat.Error.Query.Auth.LoginLifetimeNotStarted").arg(DateStart.toString(FORMAT_DATE_V1));
				return false;
			}
			else if (CurrentDate > DateEnd)
			{
				ErrorString = LANG("Carat.Error.Query.Auth.LoginLifetimeEnded");
				return false;
			}
		}
	}

	//Устанавливаем флаги сокету
	TcpMessage->TcpSocket->SetAuthorized(true);
	TcpMessage->TcpSocket->SetUserID(UserID);
	TcpMessage->TcpSocket->SetUserGroupID(GroupID);
	TcpMessage->TcpSocket->SetUserIsSystem(IsSystem);

	//Проверяем версию клиента
	bool IsNeedUpdate = false; //По умолчанию флаг обновления должен быть false (вдруг клиент отправил невалидную версию)
	QVariant Version = TcpMessage->SystemParameters["Version"];
	if (Version.isValid()) //Если версия указана
	{
		//Проверяем его версию на валидность
		QString VersionClientString = Version.toString();
		ISVersion VersionClient(VersionClientString);
		if (VersionClient.IsValid()) //Версия валидна
		{
			IsNeedUpdate = VersionClient < ISVersionInfo::Instance().Info.Version;
		}
		else //Версия невалидна
		{
			ISLOGGER_W(__CLASS__, "Client version invalid: " + VersionClientString);
		}
	}

	//Отдаём информацию о пользователе и конфигурации
	TcpAnswer->Parameters["UserID"] = UserID;
	TcpAnswer->Parameters["UserIsSystem"] = IsSystem;
	TcpAnswer->Parameters["UserFIO"] = UserFIO;
	TcpAnswer->Parameters["UserGroupID"] = GroupID;
	TcpAnswer->Parameters["UserGroupFullAccess"] = GroupFullAccess;
	TcpAnswer->Parameters["IsNeedUpdate"] = IsNeedUpdate;
	TcpAnswer->Parameters["Configuration"] = QVariantMap
	{
		{ "UID", ISVersionInfo::Instance().ConfigurationInfo.UID },
		{ "Name", ISVersionInfo::Instance().ConfigurationInfo.Name },
		{ "Local", ISVersionInfo::Instance().ConfigurationInfo.LocalName },
		{ "Desktop", ISVersionInfo::Instance().ConfigurationInfo.DesktopForm },
		{ "DateExpired", ISVersionInfo::Instance().ConfigurationInfo.DateExpired },
		{ "Logo", ISVersionInfo::Instance().ConfigurationInfo.LogoName }
	};

    //Регистрируем пользователя в "онлайн", протоколируем и выходим
	ISTcpClients::Instance().Add(
		TcpMessage->TcpSocket->socketDescriptor(),
		UserID,
		TcpMessage->TcpSocket->peerAddress().toString(),
        TcpMessage->TcpSocket->peerPort());
	Protocol(UserID, CONST_UID_PROTOCOL_ENTER_APPLICATION, QVariant(), QVariant(), QVariant(), QVariant());
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);
	QVariant Timeout = CheckNullField("Timeout", TcpMessage);
	if (!Timeout.isValid())
	{
		return false;
	}
	
	bool Ok = true;
	int TimeoutInt = Timeout.toInt(&Ok);
	if (!Ok) //Не удалось привести значение к числу
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueInvalid");
		return false;
	}

	if (!TimeoutInt) //Если значение равно нулю - ошибка
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueIsNull");
		return false;
	}

	if (TimeoutInt < 0) //Если значение меньше нуля - ошибка
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueIsNegative");
		return false;
	}

	ISSleep(TimeoutInt);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Получаем настройки БД
	QVariantMap SettingsDBMap;
	ISQuery qSelectSettingsDB(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTINGS_DATABASE);
	if (qSelectSettingsDB.Execute())
	{
		if (qSelectSettingsDB.First())
		{
			SettingsDBMap["Name"] = qSelectSettingsDB.ReadColumn("sgdb_settingname");
			SettingsDBMap["UserAccessDatabase"] = qSelectSettingsDB.ReadColumn("sgdb_useraccessdatabase");
			SettingsDBMap["NumberSymbolsAfterComma"] = qSelectSettingsDB.ReadColumn("sgdb_numbersimbolsaftercomma");
			SettingsDBMap["StirageFileMaxSize"] = qSelectSettingsDB.ReadColumn("sgdb_storagefilemaxsize");
			TcpAnswer->Parameters["SettingsDB"] = SettingsDBMap;
		}
		else
		{
			ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsDBNotActive");
			return false;
		}
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsDB").arg(qSelectSettingsDB.GetErrorString());
		return false;
	}

	//Получаем права на таблицы
	QVariantMap AccessTablesMap;
	ISQuery qSelectAccessTables(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_TABLE);
	qSelectAccessTables.BindValue(":GroupID", TcpMessage->TcpSocket->GetUserGroupID());
	if (qSelectAccessTables.Execute())
	{
		while (qSelectAccessTables.Next())
		{
			QString TableName = qSelectAccessTables.ReadColumn("gatb_table").toString();
			ISUuid AccessUID = qSelectAccessTables.ReadColumn("gatt_uid");
			if (AccessTablesMap.contains(TableName))
			{
				QVariantList AccessList = AccessTablesMap[TableName].toList();
				AccessList.append(AccessUID);
				AccessTablesMap[TableName] = AccessList;
			}
			else
			{
				AccessTablesMap[TableName] = QVariantList{ AccessUID };
			}
		}
		TcpAnswer->Parameters["AccessTables"] = AccessTablesMap;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserGroupAccessTable").arg(qSelectAccessTables.GetErrorString());
		return false;
	}

	//Получаем специальные права
	QVariantList AccessSpecialList;
	ISQuery qSelectAccessSpecial(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_SPECIAL);
	qSelectAccessSpecial.BindValue(":GroupID", TcpMessage->TcpSocket->GetUserGroupID());
	if (qSelectAccessSpecial.Execute())
	{
		while (qSelectAccessSpecial.Next())
		{
			AccessSpecialList.append(qSelectAccessSpecial.ReadColumn("gast_uid"));
		}
		TcpAnswer->Parameters["AccessSpecial"] = AccessSpecialList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial").arg(qSelectAccessSpecial.GetErrorString());
		return false;
	}

	//Получаем системы и подсистемы
	QVariantList SystemSubSystemList;
	ISQuery qSelectSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_SYSTEM),
		qSelectSubSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_SUBSYSTEM);
	if (qSelectSystem.Execute()) //Запрашиваем системы
	{
		while (qSelectSystem.Next())
		{
			QVariantList SubSystemsList;
			qSelectSubSystem.BindValue(":SystemUID", qSelectSystem.ReadColumn("stms_uid"));
			qSelectSubSystem.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qSelectSubSystem.BindValue(":UserIsSystem", TcpMessage->TcpSocket->GetUserIsSystem());
			if (qSelectSubSystem.Execute()) //Запрашиваем подсистемы текущей системы
			{
				while (qSelectSubSystem.Next())
				{
					SubSystemsList.append(QVariantMap
					{
						{ "UID", qSelectSubSystem.ReadColumn("sbsm_uid") },
						{ "Local", qSelectSubSystem.ReadColumn("sbsm_localname") },
						{ "Icon", qSelectSubSystem.ReadColumn("sbsm_icon") },
						{ "Class", qSelectSubSystem.ReadColumn("sbsm_classname") },
						{ "Table", qSelectSubSystem.ReadColumn("sbsm_tablename") },
						{ "Hint", qSelectSubSystem.ReadColumn("sbsm_hint") }
					});
				}
			}
			else
			{
				ErrorString = LANG("Carat.Error.Query.GetMetaData.SubSystems").arg(qSelectSubSystem.GetErrorString());
				return false;
			}
			
			//Добавляем систему только если по ней разрешены какие-нибудь подсистемы
			if (qSelectSubSystem.GetCountResultRows() > 0)
			{
				SystemSubSystemList.append(QVariantMap
				{
					{ "UID", qSelectSystem.ReadColumn("stms_uid") },
					{ "IsSystem", qSelectSystem.ReadColumn("stms_issystem") },
					{ "Local", qSelectSystem.ReadColumn("stms_localname") },
					{ "Icon", qSelectSystem.ReadColumn("stms_icon") },
					{ "Hint", qSelectSystem.ReadColumn("stms_hint") },
					{ "SubSystems", SubSystemsList }
				});
			}
		}
		TcpAnswer->Parameters["SystemSubSystem"] = SystemSubSystemList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Systems").arg(qSelectSystem.GetErrorString());
		return false;
	}

	//Получаем печать
	QVariantList PrintingList;
	ISQuery qSelectReport(ISDatabase::Instance().GetDB(DBConnectionName), QS_REPORT),
		qSelectReportField(ISDatabase::Instance().GetDB(DBConnectionName), QS_REPORT_FIELD);
	if (qSelectReport.Execute())
	{
		while (qSelectReport.Next())
		{
			QVariantMap ReportFieldMap;
			qSelectReportField.BindValue(":ParentUID", qSelectReport.ReadColumn("rprt_uid"));
			if (qSelectReportField.Execute())
			{
				while (qSelectReportField.Next())
				{
					ReportFieldMap.insert(qSelectReportField.ReadColumn("rprt_replacevalue").toString(), qSelectReportField.ReadColumn("rprt_sqlquery"));
				}
			}
			else
			{
				ErrorString = LANG("Carat.Error.Query.GetMetaData.ReportField").arg(qSelectReportField.GetErrorString());
				return false;
			}
			PrintingList.append(QVariantMap
			{
				{ "Type", qSelectReport.ReadColumn("rprt_type") },
				{ "Table", qSelectReport.ReadColumn("rprt_tablename") },
				{ "Local", qSelectReport.ReadColumn("rprt_localname") },
				{ "File", qSelectReport.ReadColumn("rprt_filetemplate") },
				{ "Fields", ReportFieldMap }
			});
		}
		TcpAnswer->Parameters["Printing"] = PrintingList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Report").arg(qSelectReport.GetErrorString());
		return false;
	}

	//Получаем избранные объекты
	QVariantMap FavoriteMap;
	ISQuery qSelectFavorite(ISDatabase::Instance().GetDB(DBConnectionName), QS_FAVORITE);
	qSelectFavorite.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectFavorite.Execute())
	{
		while (qSelectFavorite.Next())
		{
			QString TableName = qSelectFavorite.ReadColumn("fvts_tablename").toString();
			QString ObjectsID = qSelectFavorite.ReadColumn("fvts_objectsid").toString();
			FavoriteMap[TableName] = ObjectsID.mid(1, ObjectsID.size() - 2).split(SYMBOL_COMMA);
		}
		TcpAnswer->Parameters["Favorite"] = FavoriteMap;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Favorite").arg(qSelectFavorite.GetErrorString());
		return false;
	}

	//Получаем историю
	QVariantList HistoryList;
	ISQuery qSelectHistory(ISDatabase::Instance().GetDB(DBConnectionName), QS_HISTORY);
	qSelectHistory.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectHistory.Execute())
	{
		while (qSelectHistory.Next())
		{
			HistoryList.append(QVariantMap
			{
				{ "Date", qSelectHistory.ReadColumn("htry_datetime") },
				{ "Table", qSelectHistory.ReadColumn("htry_tablename") },
				{ "ID", qSelectHistory.ReadColumn("htry_objectid") }
			});
		}
		TcpAnswer->Parameters["History"] = HistoryList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.History").arg(qSelectHistory.GetErrorString());
		return false;
	}

	//Получаем сортировки
	QVariantList SortingList;
	ISQuery qSelectSorting(ISDatabase::Instance().GetDB(DBConnectionName), QS_SORTINGS);
	qSelectSorting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectSorting.Execute())
	{
		while (qSelectSorting.Next())
		{
			SortingList.append(QVariantMap
			{
				{ "Table", qSelectSorting.ReadColumn("sgts_tablename") },
				{ "Field", qSelectSorting.ReadColumn("sgts_fieldname") },
				{ "Sort", qSelectSorting.ReadColumn("sgts_sorting") }
			});
		}
		TcpAnswer->Parameters["Sorting"] = SortingList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Sorting").arg(qSelectSorting.GetErrorString());
		return false;
	}

	//Получаем размеры колонок
	QVariantList ColumnSizeList;
	ISQuery qSelectColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QS_COLUMN_SIZE);
	qSelectColumnSize.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectColumnSize.Execute())
	{
		while (qSelectColumnSize.Next())
		{
			ColumnSizeList.append(QVariantMap
			{
				{ "Table", qSelectColumnSize.ReadColumn("clsz_tablename") },
				{ "Field", qSelectColumnSize.ReadColumn("clsz_fieldname") },
				{ "Size", qSelectColumnSize.ReadColumn("clsz_size") }
			});
		}
		TcpAnswer->Parameters["ColumnSize"] = ColumnSizeList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.ColumnSize").arg(qSelectColumnSize.GetErrorString());
		return false;
	}

	//Получаем пользовательские настройки
	QVariantList Settings;
	ISQuery qSelectSettingGroup(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTING_GROUP),
		qSelectSettingUser(ISDatabase::Instance().GetDB(DBConnectionName), QS_SETTING);
	if (qSelectSettingGroup.Execute())
	{
		while (qSelectSettingGroup.Next())
		{
			ISUuid GroupUID = qSelectSettingGroup.ReadColumn("stgp_uid");
			QVariantList SettingsList;
			qSelectSettingUser.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qSelectSettingUser.BindValue(":GroupUID", GroupUID);
			if (qSelectSettingUser.Execute())
			{
				while (qSelectSettingUser.Next())
				{
					ISUuid SettingUID = qSelectSettingUser.ReadColumn("stgs_uid");
					QVariant SettingDefault = qSelectSettingUser.ReadColumn("stgs_defaultvalue");
					QVariantMap SettingMap =
					{
						{ "UID", SettingUID },
						{ "Name", qSelectSettingUser.ReadColumn("stgs_name") },
						{ "Type", qSelectSettingUser.ReadColumn("stgs_type") },
						{ "WidgetEditName", qSelectSettingUser.ReadColumn("stgs_widgeteditname") },
						{ "Local", qSelectSettingUser.ReadColumn("stgs_localname") },
						{ "Hint", qSelectSettingUser.ReadColumn("stgs_hint") },
						{ "Default", SettingDefault }
					};

					if (qSelectSettingUser.ReadColumn("count").toInt())  //Если такая настройка у пользователя уже есть - получаем её значение
					{
						SettingMap["Value"] = qSelectSettingUser.ReadColumn("usst_value");
					}
					else //Такой настройки у пользователя нет - добавляем со значением по умолчанию
					{
						ISQuery qInsertSetting(ISDatabase::Instance().GetDB(DBConnectionName), QI_USER_SETTING);
						qInsertSetting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
						qInsertSetting.BindValue(":SettingUID", SettingUID);
						qInsertSetting.BindValue(":Value", SettingDefault);
						if (!qInsertSetting.Execute())
						{
							ErrorString = "Error inserting new user setting: " + qInsertSetting.GetErrorString();
							return false;
						}
						SettingMap["Value"] = SettingDefault;
					}
					SettingsList.append(SettingMap);
				}
			}
			else
			{
				ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsUser").arg(qSelectSettingUser.GetErrorString());
				return false;
			}
			Settings.append(QVariantMap
			{
				{ "UID", GroupUID },
				{ "Name", qSelectSettingGroup.ReadColumn("stgp_name").toString() },
				{ "Local", qSelectSettingGroup.ReadColumn("stgp_localname").toString() },
				{ "Icon", qSelectSettingGroup.ReadColumn("stgp_iconname").toString() },
				{ "Hint", qSelectSettingGroup.ReadColumn("stgp_hint").toString() },
				{ "Settings", SettingsList }
			});
		}
		TcpAnswer->Parameters["Settings"] = Settings;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsGroup").arg(qSelectSettingGroup.GetErrorString());
		return false;
	}

	//Получаем параграфы
	QVariantList ParagraphList;
	ISQuery qSelectParagraph(ISDatabase::Instance().GetDB(DBConnectionName), QS_PARAGRAPH);
	if (qSelectParagraph.Execute())
	{
		while (qSelectParagraph.Next())
		{
			ParagraphList.append(QVariantMap
			{
				{ "UID", qSelectParagraph.ReadColumn("prhs_uid") },
				{ "Name", qSelectParagraph.ReadColumn("prhs_name") },
				{ "Local", qSelectParagraph.ReadColumn("prhs_localname") },
				{ "ToolTip", qSelectParagraph.ReadColumn("prhs_tooltip") },
				{ "Icon", qSelectParagraph.ReadColumn("prhs_icon") },
				{ "Class", qSelectParagraph.ReadColumn("prhs_classname") }
			});
		}
		TcpAnswer->Parameters["Paragraphs"] = ParagraphList;
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Paragraph").arg(qSelectParagraph.GetErrorString());
		return false;
	}

	//Читаем мета-данные
	QVariantList MetaDataList;
	QStringList Filter("*.xsn"); //Фильтр
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //Загрузка мета-данных движка
	FileInfoList.append(QDir(":_" + ISVersionInfo::Instance().ConfigurationInfo.Name).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка мета-данных конфигурации
	for (const QFileInfo &FileInfo : FileInfoList) //Обход всех XSN файлов
	{
		QFile FileXSN(FileInfo.filePath());
		if (!FileXSN.open(QIODevice::ReadOnly)) //Не удалось открыть файл на чтение - выходим с ошибкой
		{
			ErrorString = LANG("Carat.Error.Query.GetMetaData.FileXSN").arg(FileInfo.fileName()).arg(FileXSN.errorString());
			return false;
		}
		MetaDataList.append(FileXSN.readAll());
		FileXSN.close();
	}
	TcpAnswer->Parameters["MetaData"] = MetaDataList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Получаем директорию с дистрибутивами
	QString UpdateClientDir = CONFIG_STRING(CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR);
	if (UpdateClientDir.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.GetLastClient.PathIsEmpty");
		return false;
	}

	QDir Dir(UpdateClientDir);
	if (!Dir.exists())
	{
		ISLOGGER_W(__CLASS__, "Folder \"" + UpdateClientDir + "\" not exist. Check config parameter \"" + CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR + "\"");
		ErrorString = LANG("Carat.Error.Query.GetLastClient.Other");
		return false;
	}

	//Получаем отсортированный (по дате) список файлов и проверяем его на пустоту
	QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
	if (StringList.isEmpty()) //Если обновлений нет - выходим
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//Обходим список файлов и ищем самый последний
	QString FilePath;
	for (const QString &FileName : StringList)
	{
		QStringList FileNameList = FileName.split(SYMBOL_POINT);
		if (FileNameList.size() != 4) //Если имя файла невалидное - пропускаем его
		{
			continue;
		}
		FilePath = Dir.absolutePath() + '/' + FileName;
		break;
	}

	//Файл не найден - выходим
	if (FilePath.isEmpty())
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//Открываем файл
	QFile File(FilePath);
	if (!File.open(QIODevice::ReadOnly)) //Не удалось открыть файл
	{
		ISLOGGER_E(__CLASS__, "Not read file \"" + FilePath + "\": " + File.errorString());
		ErrorString = LANG("Carat.Error.Query.GetLastClient.Other");
		return false;
	}

	//Читаем и сразу закрываем файл
	QByteArray ByteArray = File.readAll();
	File.close();

	//Конвертируем в base64 и отдаём
	TcpAnswer->Parameters["FileName"] = QFileInfo(FilePath).fileName();
	TcpAnswer->Parameters["Data"] = ByteArray.toBase64();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant UserID = CheckNullField("UserID", TcpMessage);
	if (!UserID.isValid())
	{
		return false;
	}

	//Проверяем наличие пароля
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist)) //Не удалось проверить наличие пароля
	{
		return false;
	}
	TcpAnswer->Parameters["IsExist"] = Exist;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage),
		Hash = CheckNullField("Hash", TcpMessage);
	if (!Hash.isValid() || !UserID.isValid())
	{
		return false;
	}

	//Проверяем пользователя на системность
	bool IsSystem = true;
	if (!UserIsSystem(UserID, IsSystem))
	{
		return false;
	}

	if (IsSystem) //Пользователь системный - отказываем в сбросе пароля
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.UserIsSystem");
		return false;
	}

	//Проверяем наличие пароля
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist)) //Не удалось проверить наличие пароля
	{
		return false;
	}

	if (Exist) //Пароль уже существует - считаем за ошибку
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.AlreadyExist");
		return false;
	}

	//Генерируем соль и солим пароль
	QString Salt;
	if (!ISAlgorithm::GenerateSalt(Salt, ErrorString)) //Ошибка генерации
	{
		return false;
	}
	QString HashResult = ISAlgorithm::SaltPassword(Hash.toString(), Salt);

	//Устанавливаем пароль
	ISQuery qUpdateHash(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH);
	qUpdateHash.BindValue(":Hash", HashResult);
	qUpdateHash.BindValue(":Salt", Salt);
	qUpdateHash.BindValue(":UserID", UserID);
	if (!qUpdateHash.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.UpdateHash").arg(qUpdateHash.GetErrorString());
		return false;
	}

	//Фиксируем изменение пароля
	Protocol(UserID.toInt(), CONST_UID_PROTOCOL_USER_PASSWORD_CREATE, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage),
		HashOld = CheckNullField("HashOld", TcpMessage), //Старый пароль
		Hash = CheckNullField("Hash", TcpMessage); //Новый пароль
	if (!UserID.isValid() || !HashOld.isValid() || !Hash.isValid())
	{
		return false;
	}

	//Получаем текущий хэш и соль пользователя
	ISQuery qSelectHash(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_PASSWORD);
	qSelectHash.BindValue(":UserID", UserID);
	if (!qSelectHash.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.SelectHash").arg(qSelectHash.GetErrorString());
		return false;
	}

	if (!qSelectHash.First()) //Пользователя с таким UserID нет в БД
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.UserNotExist").arg(UserID.toInt());
		return false;
	}

	//Текущие хэш и соль
	QString CurrentHash = qSelectHash.ReadColumn("usrs_hash").toString(),
		CurrentSalt = qSelectHash.ReadColumn("usrs_salt").toString();

	//Солим присланный хэш и проверяем. Если подсоленый хэш не соответствует тому что в БД - значит вводили неправильный текущий пароль (или логин) - ошибка
	if (ISAlgorithm::SaltPassword(HashOld.toString(), CurrentSalt) != CurrentHash)
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.InvalidCurrentLoginOrPassword");
		return false;
	}

	//Генерируем новую соль и солим новый пароль
	if (!ISAlgorithm::GenerateSalt(CurrentSalt, ErrorString)) //Ошибка генерации
	{
		return false;
	}
	QString HashResult = ISAlgorithm::SaltPassword(Hash.toString(), CurrentSalt);

	//Обновляем пароль
	ISQuery qUpdateHash(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH);
	qUpdateHash.BindValue(":Hash", HashResult);
	qUpdateHash.BindValue(":Salt", CurrentSalt);
	qUpdateHash.BindValue(":UserID", UserID);
	if (!qUpdateHash.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.UpdateHash").arg(qUpdateHash.GetErrorString());
		return false;
	}

	//Фиксируем изменение пароля
	Protocol(UserID.toInt(), CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage);
	if (!UserID.isValid())
	{
		return false;
	}

	//Проверяем пользователя на системность
	bool IsSystem = true;
	if (!UserIsSystem(UserID, IsSystem))
	{
		return false;
	}

	if (IsSystem) //Пользователь системный - отказываем в сбросе пароля
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.UserIsSystem");
		return false;
	}

	//Проверяем наличие пароля
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist))
	{
		return false;
	}
	
	if (!Exist) //Если пароля нет - считаем ошибкой
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.PasswordIsNull");
		return false;
	}

	//Сбрасываем пароль
	ISQuery qUpdateHashReset(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH_RESET);
	qUpdateHashReset.BindValue(":UserID", UserID);
	if (!qUpdateHashReset.Execute()) //Не удалось сбросить пароль
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.Reset");
		return false;
	}

	//Фиксируем изменение пароля
	Protocol(UserID.toInt(), CONST_UID_PROTOCOL_USER_PASSWORD_RESET, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserSettingsReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Сбрасываем значения настроек в дефолтные
	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_SETTINGS_RESET);
	qUpdate.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qUpdate.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.UserSettingsReset.Update").arg(qUpdate.GetErrorString());
		return false;
	}

	//Обходим результат
	QVariantMap ResultMap;
	while (qUpdate.Next())
	{
		ResultMap[ISUuid(qUpdate.ReadColumn("stgs_uid"))] = qUpdate.ReadColumn("usst_value");
	}
	TcpAnswer->Parameters["Result"] = ResultMap;
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_SETTINGS_RESET, QVariant(), QVariant(), QVariant());
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Получаем директорию с записями разговоров
	QString RecordDir = CONFIG_STRING(CONST_CONFIG_AMI_RECORD_DIR);
	if (RecordDir.isEmpty()) //Директория не настроена - ошибка
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.RecordDirIsEmpty");
		return false;
	}

	QVariant RecordID = CheckNullField("RecordID", TcpMessage);
	if (!RecordID.isValid())
	{
		return false;
	}

	//Получаем идентификатор записи по её коду
	ISQuery qSelectRecord(ISDatabase::Instance().GetDB(DBConnectionName), QS_ASTERISK_RECORD);
	qSelectRecord.BindValue(":RecordID", RecordID);
	if (!qSelectRecord.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.SelectUniqueID").arg(qSelectRecord.GetErrorString());
		return false;
	}
	if (!qSelectRecord.First())
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.NotFoundUniqueID").arg(RecordID.toInt());
		return false;
	}
	QString UniqueID = qSelectRecord.ReadColumn("ascl_uniqueid").toString();
	
	QFile FileRecord(RecordDir + UniqueID + SYMBOL_POINT + EXTENSION_WAV);
	if (!FileRecord.exists()) //Файл не существует
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.NotFoundRecordCall");
		return false;
	}

	if (!FileRecord.open(QIODevice::ReadOnly)) //Не удалось открыть файл
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.OpenRecordCall").arg(FileRecord.errorString());
		return false;
	}

	//Читаем файл, закрываем и выходим
	TcpAnswer->Parameters["Data"] = FileRecord.readAll().toBase64();
	FileRecord.close();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Получаем список подключенных клиентов, объявляем контейнер для них и запрос
	std::vector<ISClientInfo> VectorClients = ISTcpClients::Instance().GetClients();
	QVariantList VariantList;
	ISQuery qSelectClient(ISDatabase::Instance().GetDB(DBConnectionName), QS_CLIENT);

	//Обходим всех клиентов и получаем по каждому информацию
	for (const ISClientInfo &ClientInfo : VectorClients)
	{
		qSelectClient.BindValue(":UserID", ClientInfo.ID);
		if (!qSelectClient.Execute()) //Ошибка в запросе
		{
			ErrorString = LANG("Carat.Error.Query.GetClients.Select").arg(ClientInfo.ID);
			break;
		}

		if (!qSelectClient.First()) //Пользователь не найден
		{
			ErrorString = LANG("Carat.Error.Query.GetClients.UserNotExist").arg(ClientInfo.ID);
			break;
		}

		//Добавляем информацию о пользователе в список
		VariantList.push_back(QVariantMap
		{
			{ "Address", ClientInfo.Address },
			{ "Port", ClientInfo.Port },
			{ "ID", ClientInfo.ID },
			{ "FIO", qSelectClient.ReadColumn("usrs_fio") },
			{ "Photo", qSelectClient.ReadColumn("usrs_photo").toByteArray().toBase64() }
		});
	}
	
	//Если была ошибка - очищаем потенциально не пустой список и выходим
	if (qSelectClient.GetErrorType() != QSqlError::NoError)
	{
		VariantList.clear();
		return false;
	}
	TcpAnswer->Parameters["Clients"] = VariantList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant TableName = CheckNullField("TableName", TcpMessage);
	if (!TableName.isValid())
	{
		return false;
	}
	QString TableNameString = TableName.toString();

	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableNameString);
	if (!MetaTable)
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.TableNotExist").arg(TableNameString);
		return false;
	}

	//Проверяем наличие идентификаторов
	QVariantList Objects = TcpMessage->Parameters["Objects"].toList();
	if (Objects.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.NotSpecifiedID");
		return false;
	}

	//Формируем конструкцию IN для условия WHERE
	bool Ok = true;
	QString SqlIN;
	for (const QVariant &ID : Objects)
	{
		SqlIN += QString::number(ID.toInt(&Ok)) + SYMBOL_COMMA;
		if (!Ok) //Не удалось привести к числу
		{
			ErrorString = LANG("Carat.Error.Query.RecordDelete.ToInt");
			return false;
		}
	}
	SqlIN.chop(1);

	//Проверяем, нет ли системных записей
	ISQuery qSqlQuery(ISDatabase::Instance().GetDB(DBConnectionName), "SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM " + TableNameString + " WHERE " + MetaTable->Alias + "_issystem AND " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	if (!qSqlQuery.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.Select").arg(qSqlQuery.GetErrorString());
		return false;
	}

	if (!qSqlQuery.First())
	{
		ErrorString = qSqlQuery.GetErrorString();
		return false;
	}

	if (qSqlQuery.ReadColumn("is_exist").toBool())
	{
		ErrorString = Objects.size() == 1 ?
			LANG("Carat.Error.Query.RecordDelete.RecordIsSystem") :
			LANG("Carat.Error.Query.RecordDelete.RecordsIsSystem");
		return false;
	}

	//Удаляем
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), "DELETE FROM " + TableNameString + " WHERE " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	if (!qDelete.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.Delete").arg(qDelete.GetErrorString());
		return false;
	}

	//Протоколируем
	int UseriD = TcpMessage->TcpSocket->GetUserID();
	for (const QVariant &ObjectID : Objects)
	{
		Protocol(UseriD, CONST_UID_PROTOCOL_DELETE_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, QVariant());
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage),
		Message = CheckNullField("Message", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid() || !Message.isValid())
	{
		return false;
	}

	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_DISCUSSION);
	qInsert.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qInsert.BindValue(":TableName", TableName);
	qInsert.BindValue(":ObjectID", ObjectID);
	qInsert.BindValue(":Message", Message);
	if (!qInsert.Execute()) //Ошибка вставки
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionAdd.Insert").arg(qInsert.GetErrorString());
		return false;
	}

	if (!qInsert.First()) //Ошибка перехода к возвращаемому значению
	{
		ErrorString = qInsert.GetErrorString();
		return false;
	}

	TcpAnswer->Parameters["ID"] = qInsert.ReadColumn("dson_id");
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant DiscussionID = CheckNullField("ID", TcpMessage),
		Message = CheckNullField("Message", TcpMessage);
	if (!DiscussionID.isValid() || !Message.isValid())
	{
		return false;
	}

	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), QU_DISCUSSION);
	qUpdate.BindValue(":Message", Message);
	qUpdate.BindValue(":DiscussionID", DiscussionID);
	if (!qUpdate.Execute()) //Не удалось обновить запись
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionEdit.Update").arg(qUpdate.GetErrorString());
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant DiscussionID = CheckNullField("ID", TcpMessage);
	if (!DiscussionID.isValid())
	{
		return false;
	}

	ISQuery qCopy(ISDatabase::Instance().GetDB(DBConnectionName), QI_DISCUSSION_COPY);
	qCopy.BindValue(":DiscussionID", DiscussionID);
	if (!qCopy.Execute()) //Не удалось создать копию
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionCopy.Insert").arg(qCopy.GetErrorString());
		return false;
	}

	if (!qCopy.First()) //Ошибка перехода к возвращаемому значению
	{
		ErrorString = qCopy.GetErrorString();
		return false;
	}
	TcpAnswer->Parameters["ID"] = qCopy.ReadColumn("dson_id");
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetTableData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage);
	if (!TableName.isValid())
	{
		return false;
	}

	//Получаем мета-таблицу
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.TableNotExist").arg(TableName.toString());
		return false;
	}
	unsigned int UserID = TcpMessage->TcpSocket->GetUserID();

	//Получаем сортировку для этой таблицы
	QString SortingField;
	Qt::SortOrder SortingOrder;
	ISQuery qSelectSorting(ISDatabase::Instance().GetDB(DBConnectionName), QS_SORTING);
	qSelectSorting.BindValue(":UserID", UserID);
	qSelectSorting.BindValue(":TableName", MetaTable->Name);
	if (!qSelectSorting.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.SelectSorting").arg(qSelectSorting.GetErrorString());
		return false;
	}

	if (qSelectSorting.First()) //Сортирока есть - получаем её
	{
		SortingField = qSelectSorting.ReadColumn("sgts_fieldname").toString();
		SortingOrder = static_cast<Qt::SortOrder>(qSelectSorting.ReadColumn("sgts_sorting").toUInt());

		//Проверяем не указана ли сортировка в запросе
		//Если указана - проверяем - не нужно ли обновить её в БД
		bool NeedUpdateSorting = false;
		QVariant SortingFieldQuery = TcpMessage->Parameters["SortingField"],
			SortingOrderQuery = TcpMessage->Parameters["SortingOrder"];
		if (SortingFieldQuery.isValid() && SortingOrderQuery.isValid())
		{
			NeedUpdateSorting = SortingField != SortingFieldQuery.toString() ||
				SortingOrder != static_cast<Qt::SortOrder>(SortingOrderQuery.toUInt());
		}

		if (NeedUpdateSorting) //Требуется обновление сортировки в БД
		{
			SortingField = SortingFieldQuery.toString();
			SortingOrder = static_cast<Qt::SortOrder>(SortingOrderQuery.toUInt());

			ISQuery qUpdateSorting(ISDatabase::Instance().GetDB(DBConnectionName), QU_SORTING);
			qUpdateSorting.BindValue(":FieldName", SortingField);
			qUpdateSorting.BindValue(":Sorting", SortingOrder);
			qUpdateSorting.BindValue(":UserID", UserID);
			qUpdateSorting.BindValue(":TableName", MetaTable->Name);
			if (!qUpdateSorting.Execute())
			{
				ErrorString = LANG("Carat.Error.Query.GetTableData.UpdateSorting").arg(qUpdateSorting.GetErrorString());
				return false;
			}
		}
	}
	else //Сортировки нет - добавляем дефолтную
	{
		SortingField = "ID";
		SortingOrder = Qt::AscendingOrder;

		ISQuery qInsertSorting(ISDatabase::Instance().GetDB(DBConnectionName), QI_SORTING);
		qInsertSorting.BindValue(":UserID", UserID);
		qInsertSorting.BindValue(":TableName", MetaTable->Name);
		qInsertSorting.BindValue(":FieldName", SortingField);
		qInsertSorting.BindValue(":Sorting", SortingOrder);
		if (!qInsertSorting.Execute())
		{
			ErrorString = LANG("Carat.Error.Query.GetTableData.InsertSorting").arg(qInsertSorting.GetErrorString());
			return false;
		}
	}

	//Получаем размеры полей
	ISQuery qSelectColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QS_COLUMN_SIZE_TABLE);
	qSelectColumnSize.BindValue(":UserID", UserID);
	if (!qSelectColumnSize.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.SelectColumnSize").arg(qSelectColumnSize.GetErrorString());
		return false;
	}

	//Обходим результаты
	QVariantMap ColumnSizeMap;
	while (qSelectColumnSize.Next())
	{
		ColumnSizeMap[qSelectColumnSize.ReadColumn("clsz_fieldname").toString()] = qSelectColumnSize.ReadColumn("clsz_size");
	}

	//Создаём объект модели
	ISQueryModel QueryModel(MetaTable, ISNamespace::QMT_List);
	QueryModel.SetSorting(SortingField, SortingOrder);

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QueryModel.GetQueryText());
	qSelect.SetShowLongQuery(false);
	if (!qSelect.Execute()) //Запрос не отработал
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.Select").arg(qSelect.GetErrorString());
		return false;
	}

	QSqlRecord SqlRecord = qSelect.GetRecord();
	int FieldCount = SqlRecord.count();
	QVariantList FieldList, RecordList;
	std::vector<ISNamespace::FieldType> VectorType(FieldCount, ISNamespace::FT_Unknown);

	//Заполняем служебную информацию
	QVariantMap ServiceInfoMap = 
	{
		{ "SortingField", SortingField },
		{ "SortingOrder", SortingOrder }
	};

	//Заполняем описание полей
	for (int i = 0; i < FieldCount; ++i)
	{
		PMetaField *MetaField = MetaTable->GetField(SqlRecord.fieldName(i));
		VectorType[i] = MetaField->Type; //Заполняем типы сейчас, чтобы использовать их ниже
		FieldList.append(QVariantMap
		{
			{ "Name", MetaField->Name },
			{ "LocalName", MetaField->LocalListName },
			{ "Type", MetaField->Type },
			{ "IsForeign", MetaField->Foreign ? true : false }
		});
	}

	if (qSelect.First()) //Данные есть
	{
		do
		{
			QVariantList Values; //Список значений
			SqlRecord = qSelect.GetRecord(); //Получаем очередную запись
			for (int i = 0; i < FieldCount; ++i) //Обходим поля и вытаскиваем значения
			{
				//Получаем тип поля
				ISNamespace::FieldType Type = VectorType[i];

				//Если значение содержит NULL - добавляем пустое и переходим к следующему
				QVariant Value = SqlRecord.value(i);
				if (Value.isNull())
				{
					Values.push_back(QVariant());
					continue;
				}
				
				//Значение не содержит NULL - анализируем
				if (Type == ISNamespace::FT_Date)
				{
					Value = ConvertDateToString(Value.toDate(), FORMAT_DATE_V1);
				}
				else if (Type == ISNamespace::FT_Birthday)
				{
					Value = Value.toDate().toString(FORMAT_DATE_V1);
				}
				else if (Type == ISNamespace::FT_Time)
				{
					Value = Value.toTime().toString(FORMAT_TIME_V3);
				}
				else if (Type == ISNamespace::FT_DateTime)
				{
					Value = ConvertDateTimeToString(Value.toDateTime(), FORMAT_DATE_V1, FORMAT_TIME_V1);
				}
				else if (Type == ISNamespace::FT_Phone)
				{
					QString PhoneNumber = Value.toString();
					Value = QString("+7 (%1) %2-%3-%4").arg(PhoneNumber.left(3)).arg(PhoneNumber.mid(3, 3)).arg(PhoneNumber.mid(6, 2)).arg(PhoneNumber.right(2));
				}
				else if (Type == ISNamespace::FT_Seconds)
				{
					Value = QTime(0, 0).addSecs(Value.toInt()).toString(FORMAT_TIME_V3);
				}
				else if (Type == ISNamespace::FT_Double)
				{
					Value = QString::number(Value.toDouble(), 'f', 3); //??? Нужно использовать настройку из БД
				}
				else if (Type == ISNamespace::FT_Money)
				{
					Value = QString::number(Value.toDouble(), 'f', 2);
				}
				else if (Type == ISNamespace::FT_UID)
				{
					Value = ISUuid(Value);
				}
				Values.push_back(Value);
			}
			RecordList.push_back(Values); //Добавляем список значений в список записей
		} while (qSelect.Next()); //Обходим выборку	
	}
	TcpAnswer->Parameters["ServiceInfo"] = ServiceInfoMap;
	TcpAnswer->Parameters["ColumnSize"] = ColumnSizeMap;
	TcpAnswer->Parameters["FieldList"] = FieldList;
	TcpAnswer->Parameters["RecordList"] = RecordList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::NoteRecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//Получаем мета-таблицу
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		ErrorString = LANG("Carat.Error.Query.GetNoteRecord.TableNotExist").arg(TableName.toString());
		return false;
	}

	//Получаем примечание
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_NOTE_RECORD);
	qSelect.BindValue(":TableName", MetaTable->Name);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.GetNoteRecord.Select").arg(qSelect.GetErrorString());
		return false;
	}

	QVariant Note;
	if (qSelect.First()) //Если запись есть - вытаскиваем её
	{
		Note = qSelect.ReadColumn("nobj_note");
	}
	TcpAnswer->Parameters["Note"] = Note;

	//Протоколируем и выходим
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_NOTE_RECORD_SHOW, MetaTable->Name, MetaTable->LocalListName, ObjectID);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::NoteRecordSet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//Получаем мета-таблицу
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		ErrorString = LANG("Carat.Error.Query.GetNoteRecord.TableNotExist").arg(TableName.toString());
		return false;
	}

	//Проверяем наличие записи
	QString NoteDB, Note = TcpMessage->Parameters["Note"].toString();
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_NOTE_RECORD);
	qSelect.BindValue(":TableName", MetaTable->Name);
	qSelect.BindValue(":ObjectID", ObjectID);
	bool Exist = qSelect.ExecuteFirst();
	if (Exist)
	{
		NoteDB = qSelect.ReadColumn("nobj_note").toString();
	}

	//Если примечание такое же как и в БД - ничего не делаем
	if (Note == NoteDB)
	{
		return true;
	}

	//Примечания отличаются - проверяем что делать дальше
	//Если примечание в БД есть, а новое пустое - удаляем
	if (Exist && Note.isEmpty())
	{
		ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), QD_NOTE_RECORD);
		qDelete.BindValue(":TableName", MetaTable->Name);
		qDelete.BindValue(":ObjectID", ObjectID);
		if (!qDelete.Execute())
		{
			ErrorString = LANG("Carat.Error.Query.SetNoteRecord.Delete").arg(qDelete.GetErrorString());
			return false;
		}
		Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_NOTE_RECORD_DELETE, MetaTable->Name, MetaTable->LocalListName, ObjectID);
	}
	else //Добавляем/обновляем
	{
		ISQuery qUpsert(ISDatabase::Instance().GetDB(DBConnectionName), Exist ? QU_NOTE_RECORD : QI_NOTE_RECORD);
		qUpsert.BindValue(":Note", Note);
		qUpsert.BindValue(":TableName", MetaTable->Name);
		qUpsert.BindValue(":ObjectID", ObjectID);
		if (!qUpsert.Execute()) //Ошибка запроса
		{
			ErrorString = (Exist ?
				LANG("Carat.Error.Query.SetNoteRecord.Update") :
				LANG("Carat.Error.Query.SetNoteRecord.Insert")).arg(qUpsert.GetErrorString());
			return false;
		}
		Protocol(TcpMessage->TcpSocket->GetUserID(), Exist ? CONST_UID_PROTOCOL_NOTE_RECORD_EDIT : CONST_UID_PROTOCOL_NOTE_RECORD_ADD,
			MetaTable->Name, MetaTable->LocalListName, ObjectID, Note);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant FileName = CheckNullField("FileName", TcpMessage),
		Data = CheckNullField("Data", TcpMessage);
	if (!FileName.isValid())
	{
		return false;
	}

	//Получаем имя файла, его расширение и массив файл
	QFileInfo FileInfo(FileName.toString());
	QString Name = FileInfo.baseName(), Expansion = FileInfo.suffix();
	QByteArray ByteArray = QByteArray::fromBase64(Data.toByteArray());

	//Проверяем размер
	int Size = ByteArray.size(), MaxSizeMB = 10; //??? Нужно использовать настройку из БД
	if (Size > (((1000 * 1024) * MaxSizeMB)))
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageAdd.Size").arg(Name).arg(MaxSizeMB);
		return false;
	}

	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_FILE_STORAGE);
	qInsert.SetShowLongQuery(false);
	qInsert.BindValue(":Owner", TcpMessage->TcpSocket->GetUserID());
	qInsert.BindValue(":Name", Name);
	qInsert.BindValue(":Expansion", Expansion);
	qInsert.BindValue(":Size", ISSystem::FileSizeFromString(Size));
	qInsert.BindValue(":Data", ByteArray);
	if (!qInsert.ExecuteFirst())
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageAdd.Insert").arg(Name).arg(qInsert.GetErrorString());
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_FILE_STORAGE_ADD, QVariant(), QVariant(), qInsert.ReadColumn("sgfs_id"), Name);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant ID = CheckNullField("ID", TcpMessage),
		Name = CheckNullField("Name", TcpMessage);
	if (!ID.isValid() || !Name.isValid())
	{
		return false;
	}

	//Копируем файл
	ISQuery qInsertCopy(ISDatabase::Instance().GetDB(DBConnectionName), QI_FILE_STORAGE_COPY);
	qInsertCopy.SetShowLongQuery(false);
	qInsertCopy.BindValue(":Name", Name);
	qInsertCopy.BindValue(":ObjectID", ID);
	if (!qInsertCopy.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageCopy.Insert").arg(qInsertCopy.GetErrorString());
		return false;
	}

	//Если файл не был скопирован, значит его не существует - ошибка
	if (qInsertCopy.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageCopy.NotExist").arg(ID.toInt());
		return false;
	}

	//Если удалось перейти на первую строку - получаем новый идентификатор записи,
	//иначе - идентификатор исходной записи
	ID = qInsertCopy.First() ? qInsertCopy.ReadColumn("sgfs_id") : ID;
	TcpAnswer->Parameters["NewID"] = ID;
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_FILE_STORAGE_COPY, QVariant(), QVariant(), ID, Name);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant ID = CheckNullField("ID", TcpMessage);
	if (!ID.isValid())
	{
		return false;
	}

	//Вытаскиваем файл
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_FILE_STORAGE);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":ObjectID", ID);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageGet.Select").arg(qSelect.GetErrorString());
		return false;
	}

	if (!qSelect.First()) //Такой файл не существует
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageGet.NotExist").arg(ID.toInt());
		return false;
	}
	TcpAnswer->Parameters["Data"] = qSelect.ReadColumn("sgfs_data").toByteArray().toBase64();
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_FILE_STORAGE_SAVE, QVariant(), QVariant(), qSelect.ReadColumn("sgfs_id"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SearchTaskText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant Value = CheckNullField("Value", TcpMessage);
	if (!Value.isValid())
	{
		return false;
	}

	//Выполняем поисковый запрос
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_TASK_SEARCH_TEXT);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":Value", Value);
	if (!qSelect.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.SearchTaskText.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//Обходим результаты поиска
	QVariantList ResultList;
	while (qSelect.Next())
	{
		ResultList.append(QVariantMap
		{
			{ "ID", qSelect.ReadColumn("task_id") },
			{ "ParentID", qSelect.ReadColumn("task_parent") },
			{ "Name", qSelect.ReadColumn("task_name") }
		});
	}
	TcpAnswer->Parameters["Results"] = ResultList;
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_TASK_SEARCH_TEXT, QVariant(), QVariant(), QVariant(), Value);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SearchTaskID(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant ID = CheckNullField("ID", TcpMessage);
	if (!ID.isValid())
	{
		return false;
	}

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_TASK_SEARCH_ID);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":ID", ID);
	if (!qSelect.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.SearchTaskID.Select").arg(qSelect.GetErrorString());
		return false;
	}

	if (!qSelect.First())
	{
		ErrorString = qSelect.GetErrorString();
		return false;
	}
	TcpAnswer->Parameters["Result"] = qSelect.ReadColumn("result");
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_TASK_SEARCH_ID, QVariant(), QVariant(), QVariant(), ID);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SearchFullText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant Value = CheckNullField("Value", TcpMessage);
	if (!Value.isValid())
	{
		return false;
	}

	//Формируем запрос
	QString SqlText = "WITH r AS \n(\n";
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables()) //Обходим все таблицы
	{
		SqlText += "SELECT " + MetaTable->Alias + "_id AS id, '" + MetaTable->Name + "' AS table_name \n";
		SqlText += "FROM " + MetaTable->Name + " \n";
		SqlText += "WHERE lower(concat(";
		for (PMetaField *MetaField : MetaTable->Fields) //Обходим поля конкретной таблицы
		{
			if (MetaField->IsSystem || !MetaField->QueryText.isEmpty()) //Если поле является системным - пропускаем его
			{
				continue;
			}
			SqlText += MetaTable->Alias + '_' + MetaField->Name + ",";
		}
		SqlText.chop(1);
		SqlText += ")) LIKE '%' || lower(:Value) || '%'\n";
		SqlText += "UNION \n";
	}
	SqlText.chop(8);
	SqlText += " \n) \n";
	SqlText += "SELECT * \n";
	SqlText += "FROM r \n";
	SqlText += "ORDER BY table_name, id";

	//Выполняем запрос
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":Value", Value);
	if (!qSelect.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.SearchFullText.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//Анализируем ответ
	QVariantList ResultList;
	while (qSelect.Next())
	{
		QVariant ID = qSelect.ReadColumn("id"),
			TableName = qSelect.ReadColumn("table_name");

		//Получаем имя объекта
		QString ObjectName;
		if (!GetObjectName(ISMetaData::Instance().GetMetaTable(TableName.toString()), ID.toUInt(), ObjectName))
		{
			return false;
		}

		ResultList.append(QVariantMap
		{
			{ "ID", ID },
			{ "TableName", TableName },
			{ "ObjectName", ObjectName }
		});
	}
	TcpAnswer->Parameters["Result"] = ResultList;
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_SEARCH_FULL_TEXT, QVariant(), QVariant(), QVariant(), Value);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetCalendarEvents(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant Month = CheckNullField("Month", TcpMessage),
		Year = CheckNullField("Year", TcpMessage);
	if (!Month.isValid() || !Year.isValid())
	{
		return false;
	}

	//Запрашиваем события
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_CALENDAR_EVENTS);
	qSelect.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qSelect.BindValue(":Month", Month);
	qSelect.BindValue(":Year", Year);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.GetCalendarEvents.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//Обходим результаты
	QVariantList ResultList;
	while (qSelect.Next())
	{
		ResultList.append(QVariantMap
		{
			{ "ID", qSelect.ReadColumn("cldr_id") },
			{ "Day", qSelect.ReadColumn("day") },
			{ "TimeAlert", qSelect.ReadColumn("cldr_timealert").toTime().toString(FORMAT_TIME_V3) },
			{ "Name", qSelect.ReadColumn("cldr_name") },
			{ "Text", qSelect.ReadColumn("cldr_text") },
			{ "Closed", qSelect.ReadColumn("cldr_closed") }
		});
	}
	TcpAnswer->Parameters["Result"] = ResultList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CalendarDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);
	
	QVariant ID = CheckNullField("ID", TcpMessage);
	if (!ID.isValid())
	{
		return false;
	}

	//Удаляем событие
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), QD_CALENDAR);
	qDelete.BindValue(":ObjectID", ID);
	if (!qDelete.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.CalendarDelete.Delete").arg(qDelete.GetErrorString());
		return false;
	}

	//Если ни одна запись не была удалена - значит такой записи нет - ошибка
	if (qDelete.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.CalendarDelete.NotExist").arg(ID.toInt());
		return false;
	}

	if (!qDelete.First())
	{
		ErrorString = qDelete.GetErrorString();
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_CALENDAR_DELETE, QVariant(), QVariant(), QVariant(), qDelete.ReadColumn("cldr_name"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetInternalLists(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Запрашиваем справочники
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_INTERNAL_LISTS);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.GetInternalLists.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//Обходим результаты запроса
	QVariantList Lists;
	while (qSelect.Next())
	{
		Lists.append(qSelect.ReadColumn("intd_tablename"));
	}
	TcpAnswer->Parameters["Lists"] = Lists;
	return true;
}
//-----------------------------------------------------------------------------
