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
static QString QS_FAVORITE = PREPARE_QUERY("SELECT fvts_tablename, array_to_string(fvts_objectsid, ',') AS fvts_objectsid "
										   "FROM _favorites "
										   "WHERE fvts_user = :UserID");
//-----------------------------------------------------------------------------
static QString QS_HISTORY = PREPARE_QUERY("SELECT htry_datetime, htry_tablename, htry_objectid "
										  "FROM _history "
										  "WHERE htry_user = :UserID "
										  "ORDER BY htry_id");
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
static QString QS_COLUMN_SIZE = PREPARE_QUERY("SELECT clsz_fieldname, clsz_size "
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
static QString QI_COLUMN_SIZE = PREPARE_QUERY("INSERT INTO _columnsize(clsz_user, clsz_tablename, clsz_fieldname, clsz_size) "
											  "VALUES(:UserID, :TableName, :FieldName, :Size)");
//-----------------------------------------------------------------------------
static QString QU_COLUMN_SIZE = PREPARE_QUERY("UPDATE _columnsize SET "
											  "clsz_size = :Size "
											  "WHERE clsz_user = :UserID "
											  "AND clsz_tablename = :TableName "
											  "AND clsz_fieldname = :FieldName");
//-----------------------------------------------------------------------------
static QString QU_SETTING = PREPARE_QUERY("UPDATE _usersettings SET "
										  "usst_value = :Value "
										  "WHERE usst_user = :UserID "
										  "AND usst_setting = (SELECT stgs_id FROM _settings WHERE stgs_uid = :SettingUID)");
//-----------------------------------------------------------------------------
static QString QD_FAVIROTES_USER = PREPARE_QUERY("DELETE FROM _favorites "
												 "WHERE fvts_user = :UserID");
//-----------------------------------------------------------------------------
static QString QD_FAVORITES = PREPARE_QUERY("DELETE FROM _favorites "
											"WHERE fvts_user = :UserID "
											"AND fvts_tablename NOT IN(:Tables)");
//-----------------------------------------------------------------------------
static QString QI_FAVORITE = PREPARE_QUERY("INSERT INTO _favorites(fvts_user, fvts_tablename, fvts_objectsid) "
										   "VALUES(:UserID, :TableName, :ObjectsID)");
//-----------------------------------------------------------------------------
static QString QU_FAVORITE = PREPARE_QUERY("UPDATE _favorites SET "
										   "fvts_objectsid = :ObjectsID "
										   "WHERE fvts_user = :UserID "
										   "AND fvts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QS_GROUP_RIGHT_SYSTEM = PREPARE_QUERY("SELECT stms_uid, stms_localname "
													 "FROM _systems "
													 "ORDER BY stms_orderid");
//-----------------------------------------------------------------------------
static QString QS_GROUP_RIGHT_SUBSYSTEM = PREPARE_QUERY("SELECT sbsm_uid, sbsm_localname, "
														"(SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM _groupaccesssubsystem WHERE gass_group = :GroupID AND gass_subsystem = sbsm_uid) "
														"FROM _subsystems "
														"WHERE sbsm_system = :SystemUID "
														"ORDER BY sbsm_orderid");
//-----------------------------------------------------------------------------
static QString QS_GROUP_RIGHT_TABLE_TYPE = PREPARE_QUERY("SELECT gatt_uid, gatt_name, gatt_icon "
														 "FROM _groupaccesstabletype "
														 "ORDER BY gatt_order");
//-----------------------------------------------------------------------------
static QString QS_GROUP_RIGHT_TABLE = PREPARE_QUERY("SELECT gatt_uid "
													"FROM _groupaccesstable "
													"LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_accesstype "
													"WHERE gatb_group = :GroupID "
													"AND gatb_table = :TableName");
//-----------------------------------------------------------------------------
static QString QS_GROUP_RIGHT_SPECIAL_PARENT = PREPARE_QUERY("SELECT gast_uid, gast_name "
															 "FROM _groupaccessspecialtype "
															 "WHERE gast_parent IS NULL "
															 "ORDER BY gast_order");
//-----------------------------------------------------------------------------
static QString QS_GROUP_RIGHT_SPECIAL = PREPARE_QUERY("SELECT gast_uid, gast_name, gast_hint, "
													  "(SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM _groupaccessspecial WHERE gasp_group = :GroupID AND gasp_specialaccess = gast_id) "
													  "FROM _groupaccessspecialtype "
													  "WHERE gast_parent = :ParentUID "
													  "ORDER BY gast_order");
//-----------------------------------------------------------------------------
static QString QI_GROUP_RIGHT_SUBSYSTEM = PREPARE_QUERY("INSERT INTO _groupaccesssubsystem(gass_group, gass_subsystem) "
														"VALUES(:GroupID, :SubSystemUID) "
														"RETURNING (SELECT sbsm_localname FROM _subsystems WHERE sbsm_uid = :SubSystemUID)");
//-----------------------------------------------------------------------------
static QString QD_GROUP_RIGHT_SUBSYSTEM = PREPARE_QUERY("DELETE FROM _groupaccesssubsystem "
														"WHERE gass_group = :GroupID "
														"AND gass_subsystem = :SubSystemUID "
														"RETURNING (SELECT sbsm_localname FROM _subsystems WHERE sbsm_uid = :SubSystemUID)");
//-----------------------------------------------------------------------------
static QString QI_GROUP_RIGHT_TABLE = PREPARE_QUERY("INSERT INTO _groupaccesstable(gatb_group, gatb_table, gatb_accesstype) "
													"VALUES(:GroupID, :TableName, (SELECT gatt_id FROM _groupaccesstabletype WHERE gatt_uid = :AccessUID)) "
													"RETURNING (SELECT gatt_name FROM _groupaccesstabletype WHERE gatt_uid = :AccessUID)");
//-----------------------------------------------------------------------------
static QString QD_GROUP_RIGHT_TABLE = PREPARE_QUERY("DELETE FROM _groupaccesstable "
													"WHERE gatb_group = :GroupID "
													"AND gatb_table = :TableName "
													"AND gatb_accesstype = (SELECT gatt_id FROM _groupaccesstabletype WHERE gatt_uid = :AccessUID) "
													"RETURNING (SELECT gatt_name FROM _groupaccesstabletype WHERE gatt_uid = :AccessUID)");
//-----------------------------------------------------------------------------
static QString QI_GROUP_RIGHT_SPECIAL = PREPARE_QUERY("INSERT INTO _groupaccessspecial(gasp_group, gasp_specialaccess) "
													  "VALUES(:GroupID, (SELECT gast_id FROM _groupaccessspecialtype WHERE gast_uid = :SpecialRightUID)) "
													  "RETURNING (SELECT gast_name FROM _groupaccessspecialtype WHERE gast_uid = :SpecialRightUID)");
//-----------------------------------------------------------------------------
static QString QD_GROUP_RIGHT_SPECIAL = PREPARE_QUERY("DELETE FROM _groupaccessspecial "
													  "WHERE gasp_group = :GroupID "
													  "AND gasp_specialaccess = (SELECT gast_id FROM _groupaccessspecialtype WHERE gast_uid = :SpecialRightUID) "
													  "RETURNING (SELECT gast_name FROM _groupaccessspecialtype WHERE gast_uid = :SpecialRightUID)");
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
	//�����, ��������� � ���� �������� ����� ��������� �� ������ ����� ����������
	connect(thread(), &QThread::finished, thread(), &QThread::deleteLater);

	//��������� ��� ����������� � ��
	DBConnectionName = QString::number(CURRENT_THREAD_ID());
	
	//�������� ������������ � ��
	IsStarted = ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword);
	if (!IsStarted)
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
	}

	qProtocol = new ISQuery(ISDatabase::Instance().GetDB(DBConnectionName), QI_PROTOCOL);

	//�������� �� ������ ��� ������
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
		ISSleep(1); //�������� �� ����������� � ��� ���������� ������ �������

		//���������, �� ���������� �� ������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_stopped = IsStopped;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (is_stopped) //���� ���� ��������� ���������� - ������� �� �����
		{
			ISLOGGER_I(__CLASS__, QString("Stopping %1...").arg(DBConnectionName));
			delete qProtocol;
			ISDatabase::Instance().Disconnect(DBConnectionName);
			break;
		}

		//�������� ������� ���������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		ISTcpMessage *tcp_message = CurrentMessage;
		if (tcp_message) //���� ��������� ���� - ������������� ���� ������ �������
		{
			IsRunning = true;
			CurrentMessage = nullptr;
		}
		CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//���� ��������� ��������� �� ��������� - ��������� ���
		if (tcp_message)
		{
			bool Result = false;
			long long PerfomanceMsec = 0;
			ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpSocket); //������� ��������� �����

			if (tcp_message->IsValid()) //���� ��������� �������� - ��������� � ����������
			{
				//���� ������ �� ��������������� � ������ ��� �� ������������� - ������
				if (tcp_message->Type != ISNamespace::AMT_Auth && !tcp_message->TcpSocket->GetAuthorized())
				{
					ErrorString = LANG("Carat.Error.NotAuthorized");
				}
				else //������ ������������� - ����������
				{
					ISTimePoint TimePoint = ISAlgorithm::GetTick(); //���������� ������� �����
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
					case ISNamespace::AMT_SaveMetaData: Result = SaveMetaData(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetGroupRights: Result = GetGroupRights(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GroupRightSubSystemAdd: Result = GroupRightSubSystemAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GroupRightSubSystemDelete: Result = GroupRightSubSystemDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GroupRightTableAdd: Result = GroupRightTableAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GroupRightTableDelete: Result = GroupRightTableDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GroupRightSpecialAdd: Result = GroupRightSpecialAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GroupRightSpecialDelete: Result = GroupRightSpecialDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetRecordValue: Result = GetRecordValue(tcp_message, TcpAnswer); break;
					default:
						ErrorString = LANG("Carat.Error.NotExistFunction").arg(tcp_message->TypeName);
					}
					PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
				}
			}
			else //��������� �� ��������
			{
				ErrorString = tcp_message->GetErrorString();
			}

			//��������� ���-���������
			QString LogText = QString("%1 message \"%2\". ID: %3 Size: %4 Chunk: %5 Parse msec: %6 MSec: %7").
				arg(Result ? "Done" : "Failed").
				arg(tcp_message->TypeName).
				arg(tcp_message->MessageID).
				arg(tcp_message->Size).
				arg(tcp_message->ChunkCount).
				arg(tcp_message->ParseMSec).
				arg(PerfomanceMsec);
			if (!Result) //������ �������� � ������� - ������������� ����� ������ � ������ � ���-���������, � ��� �� ������� ������������ �� ������ ��������� ������
			{
				TcpAnswer->SetError(ErrorString);
				LogText.append("\nError string: " + ErrorString);
				ErrorString.clear();
				TcpAnswer->Parameters.clear();
			}

			//������� ���������, ����������, ��������� ����� � ������� ������� � ��������� ������
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
void ISTcpWorker::Protocol(unsigned int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName, const QVariant &TableLocalName, const QVariant &ObjectID, const QVariant &Information)
{
	qProtocol->BindValue(":DateTime", QDateTime::currentDateTime());
	qProtocol->BindValue(":UserID", UserID);
	qProtocol->BindValue(":TableName", TableName);
	qProtocol->BindValue(":TypeUID", ActionTypeUID);
	qProtocol->BindValue(":ObjectID", ObjectID);
	qProtocol->BindValue(":TableLocalName", TableLocalName);
	qProtocol->BindValue(":Information", Information);
	if (!qProtocol->Execute()) //�� ������� �������� ������ � ��������
	{
		ISLOGGER_E(__CLASS__, "Not insert protocol: " + qProtocol->GetErrorString());
	}
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(const QVariant &UserID, bool &Exist)
{
	ISQuery qSelectHashIsNull(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_PASSWORD_IS_NULL);
	qSelectHashIsNull.BindValue(":UserID", UserID);
	if (!qSelectHashIsNull.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.CheckExistUserPassword").arg(qSelectHashIsNull.GetErrorString());
		return false;
	}

	if (!qSelectHashIsNull.First()) //�� ������� ������� �� ������ ������, �.�. ������������ � ����� UserID �� ����������
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
	if (!qSelectIsSystem.Execute()) //������ �������
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
	if (Date == QDate::currentDate().addDays(-1)) //�����
	{
		Result = LANG("Carat.Yesterday");
	}
	else if (Date == QDate::currentDate()) //�������
	{
		Result = LANG("Carat.Today");
	}
	else if (Date == QDate::currentDate().addDays(1)) //������
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
	//���� TitleName � ����-������� �� �������� - ���������� ������������� �������
	if (MetaTable->TitleName.isEmpty())
	{
		ObjectName = QString::number(ObjectID);
		return true;
	}

	//��������� ������
	QString QueryText = "SELECT ";
	QStringList StringList = MetaTable->TitleName.split(';');
	if (StringList.count() > 1) //���� ��� ������� �������� �� ���������� �����
	{
		QueryText += "concat(";
		for (const QString &FieldName : StringList) //����� �����
		{
			PMetaForeign *MetaForeign = MetaTable->GetField(FieldName)->Foreign;
			QueryText += MetaForeign ?
				("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "), ' ', ") :
				(MetaTable->Alias + '_' + FieldName + ", ' ', ");
		}
		QueryText.chop(7);
		QueryText += ") \n";
	}
	else //���� ������� ������ ���� ����
	{
		PMetaForeign *MetaForeign = MetaTable->GetField(MetaTable->TitleName)->Foreign;
		QueryText += MetaForeign ?
			("(" + ISMetaDataHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, MetaTable->TitleName) + ") \n") :
			(MetaTable->Alias + '_' + MetaTable->TitleName + " \n");
	}
	QueryText += "FROM " + MetaTable->Name + " \n";
	QueryText += "WHERE " + MetaTable->Alias + "_id = :ObjectID";

	//����������� ���
	ISQuery qSelectName(ISDatabase::Instance().GetDB(DBConnectionName), QueryText);
	qSelectName.SetShowLongQuery(false);
	qSelectName.BindValue(":ObjectID", ObjectID);
	if (!qSelectName.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.GetObjectName").arg(qSelectName.GetErrorString());
		return false;
	}

	if (!qSelectName.First()) //������ �� �������
	{
		return true;
	}
	
	//�����������
	QVariant Value = qSelectName.ReadColumn(0);
	switch (Value.type())
	{
	case QVariant::Date: ObjectName = Value.toDate().toString(FORMAT_DATE_V2); break;
	case QVariant::Time: ObjectName = Value.toTime().toString(FORMAT_TIME_V1); break;
	case QVariant::DateTime: ObjectName = Value.toDateTime().toString(FORMAT_DATE_TIME_V2); break;
	default:
		ObjectName = qSelectName.ReadColumn(0).toString();
	}

	//������� ��������� ������� � ����� ����� �������
	ISAlgorithm::RemoveLastSymbolLoop(ObjectName, SYMBOL_SPACE);
	return true;
}
//-----------------------------------------------------------------------------
PMetaTable* ISTcpWorker::GetMetaTable(const QString &TableName)
{
	PMetaTable *MetaTable = ISMetaData::Instance().GetMetaTable(TableName);
	if (!MetaTable)
	{
		ErrorString = LANG("Carat.Error.GetMetaTable").arg(TableName);
	}
	return MetaTable;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//���������, �� ����������� �� ��� ������. ���� ����������� - ������� � �������
	if (TcpMessage->TcpSocket->GetAuthorized())
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AlreadyAuthorized");
		return false;
	}

	//���� ���� ������� ������ ��� ������� - �� ��� ����� � ���������
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

	//��������� ������ ����
	if ((size_t)HashString.size() != CARAT_HASH_SIZE)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.InvalidHashSize");
		return false;
	}

	//��������� ���������� ����
	for (const QChar &Char : HashString)
	{
		int ASCII = (int)Char.toLatin1(); //��������������� ������� ������ � ASCII-���
		if ((ASCII >= 48 && ASCII <= 57) || (ASCII >= 65 && ASCII <= 70)) //���� ������� ������ ������ � �������� [0-9] ��� [a-z] - ��� ����
		{
			continue;
		}
		else //����� - ��� ����������
		{
			ErrorString = LANG("Carat.Error.Query.Auth.InvalidHash");
			return false;
		}
	}

	{
		//����������� ��� ���� �� ��
		ISQuery qSelectHash(ISDatabase::Instance().GetDB(DBConnectionName), QS_USERS_HASH);
		if (!qSelectHash.Execute()) //������ �������
		{
			ErrorString = LANG("Carat.Error.Query.Auth.SelectHash").arg(qSelectHash.GetErrorString());
			return false;
		}

		//���� ������ ������ �� ������, ������ � �� ��� �� ������ ������������
		if (qSelectHash.GetCountResultRows() == 0)
		{
			ErrorString = LANG("Carat.Error.Query.Auth.NoUsers");
			return false;
		}

		//���� ������������
		bool IsFound = false;
		while (qSelectHash.Next())
		{
			//�������� ��� � ����
			QString CurrentHash = qSelectHash.ReadColumn("usrs_hash").toString(),
				CurrentSalt = qSelectHash.ReadColumn("usrs_salt").toString();

			//����� ���������� ��� ������� �����
			QString HashResult = ISAlgorithm::SaltPassword(HashString, CurrentSalt);
			IsFound = HashResult == CurrentHash;
			if (IsFound) //�����
			{
				Hash = HashResult;
				break;
			}
		}

		QString IPAddress = TcpMessage->TcpSocket->GetAddress();
		if (IsFound) //����� ������������ - ������� ����� �� Fail2Ban
		{
			ISFail2Ban::Instance().Remove(IPAddress);
		}
		else //�� ����� ������������ - ��������� ����� � Fail2Ban
		{
			ISFail2Ban::Instance().Add(IPAddress);

			//���� ����� ������������ - �������� �� ����, ����� - ������������� � ������������ ����� ������ ��� ������
			ErrorString = ISFail2Ban::Instance().IsLock(IPAddress)
				? LANG("Carat.Error.Query.Auth.Fail2Ban").arg(ISFail2Ban::Instance().GetUnlockDateTime(IPAddress).toString(FORMAT_DATE_TIME_V2))
				: LANG("Carat.Error.Query.Auth.InvalidLoginOrPassword");
			return false;
		}
	}

	//�������� ������������
	ISQuery qSelectAuth(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_AUTH);
	qSelectAuth.BindValue(":Hash", Hash);
	if (!qSelectAuth.ExecuteFirst()) //������ �������� � �������
	{
		ErrorString = LANG("Carat.Error.Query.Auth.SelectLogin").arg(qSelectAuth.GetErrorString());
		return false;
	}

	unsigned int UserID = qSelectAuth.ReadColumn("usrs_id").toUInt();
	bool IsSystem = qSelectAuth.ReadColumn("usrs_issystem").toBool();
	QString UserFIO = qSelectAuth.ReadColumn("usrs_fio").toString();
	int GroupID = qSelectAuth.ReadColumn("usrs_group").toInt();
	bool GroupFullAccess = qSelectAuth.ReadColumn("usgp_fullaccess").toBool();

	//������ � �� ��������
	if (!qSelectAuth.ReadColumn("sgdb_useraccessdatabase").toBool() && !IsSystem)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.ConnectionBan");
		return false;
	}

	//���� � ������������ ��� ����� �������
	if (!qSelectAuth.ReadColumn("usrs_accessallowed").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.Auth.LoginNoAccess");
		return false;
	}

	//�������� ������� �������� �� ���������� ������������ � ������
	if (!IsSystem && GroupID == 0)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.LoginLinkGroup");
		return false;
	}

	//���� ��� ������������ ��������� ����������� ����� �������� ������� ������
	if (qSelectAuth.ReadColumn("usrs_accountlifetime").toBool())
	{
		QDate DateStart = qSelectAuth.ReadColumn("usrs_accountlifetimestart").toDate();
		QDate DateEnd = qSelectAuth.ReadColumn("usrs_accountlifetimeend").toDate();
		if (DateStart.isValid() && DateEnd.isValid()) //���� ���� ������ � ��������� ����� �������� �����������
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

	//������������� ����� ������
	TcpMessage->TcpSocket->SetAuthorized(true);
	TcpMessage->TcpSocket->SetUserID(UserID);
	TcpMessage->TcpSocket->SetUserGroupID(GroupID);
	TcpMessage->TcpSocket->SetUserIsSystem(IsSystem);

	//��������� ������ �������
	bool IsNeedUpdate = false; //�� ��������� ���� ���������� ������ ���� false (����� ������ �������� ���������� ������)
	QVariant Version = TcpMessage->SystemParameters["Version"];
	if (Version.isValid()) //���� ������ �������
	{
		//��������� ��� ������ �� ����������
		QString VersionClientString = Version.toString();
		ISVersion VersionClient(VersionClientString);
		if (VersionClient.IsValid()) //������ �������
		{
			IsNeedUpdate = VersionClient < ISVersionInfo::Instance().Info.Version;
		}
		else //������ ���������
		{
			ISLOGGER_W(__CLASS__, "Client version invalid: " + VersionClientString);
		}
	}

	//����� ���������� � ������������ � ������������
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

    //������������ ������������ � "������", ������������� � �������
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
	if (!Ok) //�� ������� �������� �������� � �����
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueInvalid");
		return false;
	}

	if (!TimeoutInt) //���� �������� ����� ���� - ������
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueIsNull");
		return false;
	}

	if (TimeoutInt < 0) //���� �������� ������ ���� - ������
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
	//�������� ��������� ��
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

	//�������� ����� �� �������
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
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserGroupAccessTable").arg(qSelectAccessTables.GetErrorString());
		return false;
	}

	//�������� ����������� �����
	QVariantList AccessSpecialList;
	ISQuery qSelectAccessSpecial(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_SPECIAL);
	qSelectAccessSpecial.BindValue(":GroupID", TcpMessage->TcpSocket->GetUserGroupID());
	if (qSelectAccessSpecial.Execute())
	{
		while (qSelectAccessSpecial.Next())
		{
			AccessSpecialList.append(qSelectAccessSpecial.ReadColumn("gast_uid"));
		}
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial").arg(qSelectAccessSpecial.GetErrorString());
		return false;
	}

	//�������� ������� � ����������
	QVariantList SystemSubSystemList;
	ISQuery qSelectSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_SYSTEM),
		qSelectSubSystem(ISDatabase::Instance().GetDB(DBConnectionName), QS_SUBSYSTEM);
	if (qSelectSystem.Execute()) //����������� �������
	{
		while (qSelectSystem.Next())
		{
			QVariantList SubSystemsList;
			qSelectSubSystem.BindValue(":SystemUID", qSelectSystem.ReadColumn("stms_uid"));
			qSelectSubSystem.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qSelectSubSystem.BindValue(":UserIsSystem", TcpMessage->TcpSocket->GetUserIsSystem());
			if (qSelectSubSystem.Execute()) //����������� ���������� ������� �������
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
			
			//��������� ������� ������ ���� �� ��� ��������� �����-������ ����������
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
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Systems").arg(qSelectSystem.GetErrorString());
		return false;
	}

	//�������� ������
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
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Report").arg(qSelectReport.GetErrorString());
		return false;
	}

	//�������� ��������� �������
	QVariantMap FavoriteMap;
	ISQuery qSelectFavorite(ISDatabase::Instance().GetDB(DBConnectionName), QS_FAVORITE);
	qSelectFavorite.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectFavorite.Execute())
	{
		while (qSelectFavorite.Next())
		{
			QString TableName = qSelectFavorite.ReadColumn("fvts_tablename").toString();
			QStringList Objects = qSelectFavorite.ReadColumn("fvts_objectsid").toString().split(',');
			FavoriteMap[TableName] = Objects;
		}
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Favorite").arg(qSelectFavorite.GetErrorString());
		return false;
	}

	//�������� �������
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
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.History").arg(qSelectHistory.GetErrorString());
		return false;
	}

	//�������� ���������������� ���������
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

					if (qSelectSettingUser.ReadColumn("count").toInt())  //���� ����� ��������� � ������������ ��� ���� - �������� � ��������
					{
						SettingMap["Value"] = qSelectSettingUser.ReadColumn("usst_value");
					}
					else //����� ��������� � ������������ ��� - ��������� �� ��������� �� ���������
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
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsGroup").arg(qSelectSettingGroup.GetErrorString());
		return false;
	}

	//�������� ���������
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
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetMetaData.Paragraph").arg(qSelectParagraph.GetErrorString());
		return false;
	}

	//������ ����-������
	QVariantList MetaDataList;
	QStringList Filter("*.xsn"); //������
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //�������� ����-������ ������
	FileInfoList.append(QDir(":_" + ISVersionInfo::Instance().ConfigurationInfo.Name).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //�������� ����-������ ������������
	for (const QFileInfo &FileInfo : FileInfoList) //����� ���� XSN ������
	{
		QFile FileXSN(FileInfo.filePath());
		if (!FileXSN.open(QIODevice::ReadOnly)) //�� ������� ������� ���� �� ������ - ������� � �������
		{
			ErrorString = LANG("Carat.Error.Query.GetMetaData.FileXSN").arg(FileInfo.fileName()).arg(FileXSN.errorString());
			return false;
		}
		MetaDataList.append(FileXSN.readAll());
		FileXSN.close();
	}

	TcpAnswer->Parameters["SettingsDB"] = SettingsDBMap;
	TcpAnswer->Parameters["AccessTables"] = AccessTablesMap;
	TcpAnswer->Parameters["AccessSpecial"] = AccessSpecialList;
	TcpAnswer->Parameters["SystemSubSystem"] = SystemSubSystemList;
	TcpAnswer->Parameters["Printing"] = PrintingList;
	TcpAnswer->Parameters["Favorite"] = FavoriteMap;
	TcpAnswer->Parameters["History"] = HistoryList;
	TcpAnswer->Parameters["Settings"] = Settings;
	TcpAnswer->Parameters["Paragraphs"] = ParagraphList;
	TcpAnswer->Parameters["MetaData"] = MetaDataList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//�������� ���������� � ��������������
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

	//�������� ��������������� (�� ����) ������ ������ � ��������� ��� �� �������
	QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
	if (StringList.isEmpty()) //���� ���������� ��� - �������
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//������� ������ ������ � ���� ����� ���������
	QString FilePath;
	for (const QString &FileName : StringList)
	{
		QStringList FileNameList = FileName.split(SYMBOL_POINT);
		if (FileNameList.size() != 4) //���� ��� ����� ���������� - ���������� ���
		{
			continue;
		}
		FilePath = Dir.absolutePath() + '/' + FileName;
		break;
	}

	//���� �� ������ - �������
	if (FilePath.isEmpty())
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//��������� ����
	QFile File(FilePath);
	if (!File.open(QIODevice::ReadOnly)) //�� ������� ������� ����
	{
		ISLOGGER_E(__CLASS__, "Not read file \"" + FilePath + "\": " + File.errorString());
		ErrorString = LANG("Carat.Error.Query.GetLastClient.Other");
		return false;
	}

	//������ � ����� ��������� ����
	QByteArray ByteArray = File.readAll();
	File.close();

	//�������������, ������������ � base64 � �����
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_GET_UPDATE_CLIENT);
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

	//��������� ������� ������
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist)) //�� ������� ��������� ������� ������
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

	//��������� ������������ �� �����������
	bool IsSystem = true;
	if (!UserIsSystem(UserID, IsSystem))
	{
		return false;
	}

	if (IsSystem) //������������ ��������� - ���������� � ������ ������
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.UserIsSystem");
		return false;
	}

	//��������� ������� ������
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist)) //�� ������� ��������� ������� ������
	{
		return false;
	}

	if (Exist) //������ ��� ���������� - ������� �� ������
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.AlreadyExist");
		return false;
	}

	//���������� ���� � ����� ������
	QString Salt;
	if (!ISAlgorithm::GenerateSalt(Salt, ErrorString)) //������ ���������
	{
		return false;
	}
	QString HashResult = ISAlgorithm::SaltPassword(Hash.toString(), Salt);

	//������������� ������
	ISQuery qUpdateHash(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH);
	qUpdateHash.BindValue(":Hash", HashResult);
	qUpdateHash.BindValue(":Salt", Salt);
	qUpdateHash.BindValue(":UserID", UserID);
	if (!qUpdateHash.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.UpdateHash").arg(qUpdateHash.GetErrorString());
		return false;
	}

	//��������� ��������� ������
	Protocol(UserID.toUInt(), CONST_UID_PROTOCOL_USER_PASSWORD_CREATE, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage),
		HashOld = CheckNullField("HashOld", TcpMessage), //������ ������
		Hash = CheckNullField("Hash", TcpMessage); //����� ������
	if (!UserID.isValid() || !HashOld.isValid() || !Hash.isValid())
	{
		return false;
	}

	//�������� ������� ��� � ���� ������������
	ISQuery qSelectHash(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_PASSWORD);
	qSelectHash.BindValue(":UserID", UserID);
	if (!qSelectHash.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.SelectHash").arg(qSelectHash.GetErrorString());
		return false;
	}

	if (!qSelectHash.First()) //������������ � ����� UserID ��� � ��
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.UserNotExist").arg(UserID.toInt());
		return false;
	}

	//������� ��� � ����
	QString CurrentHash = qSelectHash.ReadColumn("usrs_hash").toString(),
		CurrentSalt = qSelectHash.ReadColumn("usrs_salt").toString();

	//����� ���������� ��� � ���������. ���� ���������� ��� �� ������������� ���� ��� � �� - ������ ������� ������������ ������� ������ (��� �����) - ������
	if (ISAlgorithm::SaltPassword(HashOld.toString(), CurrentSalt) != CurrentHash)
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.InvalidCurrentLoginOrPassword");
		return false;
	}

	//���������� ����� ���� � ����� ����� ������
	if (!ISAlgorithm::GenerateSalt(CurrentSalt, ErrorString)) //������ ���������
	{
		return false;
	}
	QString HashResult = ISAlgorithm::SaltPassword(Hash.toString(), CurrentSalt);

	//��������� ������
	ISQuery qUpdateHash(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH);
	qUpdateHash.BindValue(":Hash", HashResult);
	qUpdateHash.BindValue(":Salt", CurrentSalt);
	qUpdateHash.BindValue(":UserID", UserID);
	if (!qUpdateHash.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginEdit.UpdateHash").arg(qUpdateHash.GetErrorString());
		return false;
	}

	//��������� ��������� ������
	Protocol(UserID.toUInt(), CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID);
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

	//��������� ������������ �� �����������
	bool IsSystem = true;
	if (!UserIsSystem(UserID, IsSystem))
	{
		return false;
	}

	if (IsSystem) //������������ ��������� - ���������� � ������ ������
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.UserIsSystem");
		return false;
	}

	//��������� ������� ������
	bool Exist = true;
	if (!UserPasswordExist(UserID, Exist))
	{
		return false;
	}
	
	if (!Exist) //���� ������ ��� - ������� �������
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.PasswordIsNull");
		return false;
	}

	//���������� ������
	ISQuery qUpdateHashReset(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH_RESET);
	qUpdateHashReset.BindValue(":UserID", UserID);
	if (!qUpdateHashReset.Execute()) //�� ������� �������� ������
	{
		ErrorString = LANG("Carat.Error.Query.UserLoginReset.Reset");
		return false;
	}

	//��������� ��������� ������
	Protocol(UserID.toUInt(), CONST_UID_PROTOCOL_USER_PASSWORD_RESET, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserSettingsReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//���������� �������� �������� � ���������
	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_SETTINGS_RESET);
	qUpdate.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qUpdate.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.UserSettingsReset.Update").arg(qUpdate.GetErrorString());
		return false;
	}

	//������� ���������
	QVariantMap ResultMap;
	while (qUpdate.Next())
	{
		ResultMap[ISUuid(qUpdate.ReadColumn("stgs_uid"))] = qUpdate.ReadColumn("usst_value");
	}
	TcpAnswer->Parameters["Result"] = ResultMap;
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_SETTINGS_RESET);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//�������� ���������� � �������� ����������
	QString RecordDir = CONFIG_STRING(CONST_CONFIG_AMI_RECORD_DIR);
	if (RecordDir.isEmpty()) //���������� �� ��������� - ������
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.RecordDirIsEmpty");
		return false;
	}

	QVariant RecordID = CheckNullField("RecordID", TcpMessage);
	if (!RecordID.isValid())
	{
		return false;
	}

	//�������� ������������� ������ �� � ����
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
	if (!FileRecord.exists()) //���� �� ����������
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.NotFoundRecordCall");
		return false;
	}

	if (!FileRecord.open(QIODevice::ReadOnly)) //�� ������� ������� ����
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordCall.OpenRecordCall").arg(FileRecord.errorString());
		return false;
	}

	//������ ����, ��������� � �������
	TcpAnswer->Parameters["Data"] = FileRecord.readAll().toBase64();
	FileRecord.close();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//�������� ������ ������������ ��������, ��������� ��������� ��� ��� � ������
	std::vector<ISClientInfo> VectorClients = ISTcpClients::Instance().GetClients();
	QVariantList VariantList;
	ISQuery qSelectClient(ISDatabase::Instance().GetDB(DBConnectionName), QS_CLIENT);

	//������� ���� �������� � �������� �� ������� ����������
	for (const ISClientInfo &ClientInfo : VectorClients)
	{
		qSelectClient.BindValue(":UserID", ClientInfo.ID);
		if (!qSelectClient.Execute()) //������ � �������
		{
			ErrorString = LANG("Carat.Error.Query.GetClients.Select").arg(ClientInfo.ID);
			break;
		}

		if (!qSelectClient.First()) //������������ �� ������
		{
			ErrorString = LANG("Carat.Error.Query.GetClients.UserNotExist").arg(ClientInfo.ID);
			break;
		}

		//��������� ���������� � ������������ � ������
		VariantList.push_back(QVariantMap
		{
			{ "Address", ClientInfo.Address },
			{ "Port", ClientInfo.Port },
			{ "ID", ClientInfo.ID },
			{ "FIO", qSelectClient.ReadColumn("usrs_fio") },
			{ "Photo", qSelectClient.ReadColumn("usrs_photo").toByteArray().toBase64() }
		});
	}
	
	//���� ���� ������ - ������� ������������ �� ������ ������ � �������
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
	
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//��������� ������� ���������������
	QVariantList Objects = TcpMessage->Parameters["Objects"].toList();
	if (Objects.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.NotSpecifiedID");
		return false;
	}

	//��������� ����������� IN ��� ������� WHERE
	bool Ok = true;
	QString SqlIN;
	for (const QVariant &ID : Objects)
	{
		SqlIN += QString::number(ID.toInt(&Ok)) + SYMBOL_COMMA;
		if (!Ok) //�� ������� �������� � �����
		{
			ErrorString = LANG("Carat.Error.Query.RecordDelete.ToInt");
			return false;
		}
	}
	SqlIN.chop(1);

	//�������� ����� ��������� ������� ��� ���������
	ISStringMap ObjectNameMap;
	for (const QVariant &ID : Objects)
	{
		QString ObjectName;
		if (!GetObjectName(MetaTable, ID.toUInt(), ObjectName))
		{
			return false;
		}
		ObjectNameMap[ID.toString()] = ObjectName;
	}

	//���������, ��� �� ��������� �������
	ISQuery qSqlQuery(ISDatabase::Instance().GetDB(DBConnectionName), "SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_issystem AND " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	qSqlQuery.SetShowLongQuery(false);
	if (!qSqlQuery.Execute()) //������ �������
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

	//�������
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), "DELETE FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	qDelete.SetShowLongQuery(false);
	if (!qDelete.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.RecordDelete.Delete").arg(qDelete.GetErrorString());
		return false;
	}

	//�������������
	unsigned int UserID = TcpMessage->TcpSocket->GetUserID();
	for (const QVariant &ObjectID : Objects)
	{
		Protocol(UserID, CONST_UID_PROTOCOL_DELETE_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectNameMap[ObjectID.toString()]);
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
	if (!qInsert.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionAdd.Insert").arg(qInsert.GetErrorString());
		return false;
	}

	if (!qInsert.First()) //������ �������� � ������������� ��������
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
	if (!qUpdate.Execute()) //�� ������� �������� ������
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
	if (!qCopy.Execute()) //�� ������� ������� �����
	{
		ErrorString = LANG("Carat.Error.Query.DiscussionCopy.Insert").arg(qCopy.GetErrorString());
		return false;
	}

	if (!qCopy.First()) //������ �������� � ������������� ��������
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

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}
	unsigned int UserID = TcpMessage->TcpSocket->GetUserID();

	//�������� ���������� ��� ���� �������
	QString SortingField;
	Qt::SortOrder SortingOrder;
	ISQuery qSelectSorting(ISDatabase::Instance().GetDB(DBConnectionName), QS_SORTING);
	qSelectSorting.BindValue(":UserID", UserID);
	qSelectSorting.BindValue(":TableName", MetaTable->Name);
	if (!qSelectSorting.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.SelectSorting").arg(qSelectSorting.GetErrorString());
		return false;
	}

	if (qSelectSorting.First()) //��������� ���� - �������� �
	{
		SortingField = qSelectSorting.ReadColumn("sgts_fieldname").toString();
		SortingOrder = static_cast<Qt::SortOrder>(qSelectSorting.ReadColumn("sgts_sorting").toUInt());

		//��������� �� ������� �� ���������� � �������
		//���� ������� - ��������� - �� ����� �� �������� � � ��
		bool NeedUpdateSorting = false;
		QVariant SortingFieldQuery = TcpMessage->Parameters["SortingField"],
			SortingOrderQuery = TcpMessage->Parameters["SortingOrder"];
		if (SortingFieldQuery.isValid() && SortingOrderQuery.isValid())
		{
			NeedUpdateSorting = SortingField != SortingFieldQuery.toString() ||
				SortingOrder != static_cast<Qt::SortOrder>(SortingOrderQuery.toUInt());
		}

		if (NeedUpdateSorting) //��������� ���������� ���������� � ��
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
	else //���������� ��� - ��������� ���������
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

	//�������� ������� �����
	ISQuery qSelectColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QS_COLUMN_SIZE);
	qSelectColumnSize.BindValue(":UserID", UserID);
	if (!qSelectColumnSize.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.SelectColumnSize").arg(qSelectColumnSize.GetErrorString());
		return false;
	}

	//������� ����������
	QVariantMap ColumnSizeMap;
	while (qSelectColumnSize.Next())
	{
		ColumnSizeMap[qSelectColumnSize.ReadColumn("clsz_fieldname").toString()] = qSelectColumnSize.ReadColumn("clsz_size");
	}

	//������ ������ ������
	ISQueryModel QueryModel(MetaTable, ISNamespace::QMT_List);
	QueryModel.SetSorting(SortingField, SortingOrder);

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QueryModel.GetQueryText());
	qSelect.SetShowLongQuery(false);
	if (!qSelect.Execute()) //������ �� ���������
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.Select").arg(qSelect.GetErrorString());
		return false;
	}

	QSqlRecord SqlRecord = qSelect.GetRecord();
	int FieldCount = SqlRecord.count();
	QVariantList FieldList, RecordList;
	std::vector<ISNamespace::FieldType> VectorType(FieldCount, ISNamespace::FT_Unknown);

	//��������� ��������� ����������
	QVariantMap ServiceInfoMap = 
	{
		{ "SortingField", SortingField },
		{ "SortingOrder", SortingOrder }
	};

	//��������� �������� �����
	for (int i = 0; i < FieldCount; ++i)
	{
		PMetaField *MetaField = MetaTable->GetField(SqlRecord.fieldName(i));
		VectorType[i] = MetaField->Type; //��������� ���� ������, ����� ������������ �� ����
		FieldList.append(QVariantMap
		{
			{ "Name", MetaField->Name },
			{ "LocalName", MetaField->LocalListName },
			{ "Type", MetaField->Type },
			{ "IsForeign", MetaField->Foreign ? true : false }
		});
	}

	if (qSelect.First()) //������ ����
	{
		do
		{
			QVariantList Values; //������ ��������
			SqlRecord = qSelect.GetRecord(); //�������� ��������� ������
			for (int i = 0; i < FieldCount; ++i) //������� ���� � ����������� ��������
			{
				//�������� ��� ����
				ISNamespace::FieldType Type = VectorType[i];

				//���� �������� �������� NULL - ��������� ������ � ��������� � ����������
				QVariant Value = SqlRecord.value(i);
				if (Value.isNull())
				{
					Values.push_back(QVariant());
					continue;
				}
				
				//�������� �� �������� NULL - �����������
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
					Value = QString::number(Value.toDouble(), 'f', 3); //??? ����� ������������ ��������� �� ��
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
			RecordList.push_back(Values); //��������� ������ �������� � ������ �������
		} while (qSelect.Next()); //������� �������	
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

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//�������� ����������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_NOTE_RECORD);
	qSelect.BindValue(":TableName", MetaTable->Name);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (!qSelect.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GetNoteRecord.Select").arg(qSelect.GetErrorString());
		return false;
	}

	QVariant Note;
	if (qSelect.First()) //���� ������ ���� - ����������� �
	{
		Note = qSelect.ReadColumn("nobj_note");
	}
	TcpAnswer->Parameters["Note"] = Note;

	//������������� � �������
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

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//��������� ������� ������
	QString NoteDB, Note = TcpMessage->Parameters["Note"].toString();
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_NOTE_RECORD);
	qSelect.BindValue(":TableName", MetaTable->Name);
	qSelect.BindValue(":ObjectID", ObjectID);
	bool Exist = qSelect.ExecuteFirst();
	if (Exist)
	{
		NoteDB = qSelect.ReadColumn("nobj_note").toString();
	}

	//���� ���������� ����� �� ��� � � �� - ������ �� ������
	if (Note == NoteDB)
	{
		return true;
	}

	//���������� ���������� - ��������� ��� ������ ������
	//���� ���������� � �� ����, � ����� ������ - �������
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
	else //���������/���������
	{
		ISQuery qUpsert(ISDatabase::Instance().GetDB(DBConnectionName), Exist ? QU_NOTE_RECORD : QI_NOTE_RECORD);
		qUpsert.BindValue(":Note", Note);
		qUpsert.BindValue(":TableName", MetaTable->Name);
		qUpsert.BindValue(":ObjectID", ObjectID);
		if (!qUpsert.Execute()) //������ �������
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

	//�������� ��� �����, ��� ���������� � ������ ����
	QFileInfo FileInfo(FileName.toString());
	QString Name = FileInfo.baseName(), Expansion = FileInfo.suffix();
	QByteArray ByteArray = QByteArray::fromBase64(Data.toByteArray());

	//��������� ������
	int Size = ByteArray.size(), MaxSizeMB = 10; //??? ����� ������������ ��������� �� ��
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

	//�������� ����
	ISQuery qInsertCopy(ISDatabase::Instance().GetDB(DBConnectionName), QI_FILE_STORAGE_COPY);
	qInsertCopy.SetShowLongQuery(false);
	qInsertCopy.BindValue(":Name", Name);
	qInsertCopy.BindValue(":ObjectID", ID);
	if (!qInsertCopy.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageCopy.Insert").arg(qInsertCopy.GetErrorString());
		return false;
	}

	//���� ���� �� ��� ����������, ������ ��� �� ���������� - ������
	if (qInsertCopy.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageCopy.NotExist").arg(ID.toInt());
		return false;
	}

	//���� ������� ������� �� ������ ������ - �������� ����� ������������� ������,
	//����� - ������������� �������� ������
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

	//����������� ����
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_FILE_STORAGE);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":ObjectID", ID);
	if (!qSelect.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.FileStorageGet.Select").arg(qSelect.GetErrorString());
		return false;
	}

	if (!qSelect.First()) //����� ���� �� ����������
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

	//��������� ��������� ������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_TASK_SEARCH_TEXT);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":Value", Value);
	if (!qSelect.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.SearchTaskText.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//������� ���������� ������
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

	//��������� ������
	QString SqlText = "WITH r AS \n(\n";
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables()) //������� ��� �������
	{
		SqlText += "SELECT " + MetaTable->Alias + "_id AS id, '" + MetaTable->Name + "' AS table_name \n";
		SqlText += "FROM " + MetaTable->Name + " \n";
		SqlText += "WHERE lower(concat(";
		for (PMetaField *MetaField : MetaTable->Fields) //������� ���� ���������� �������
		{
			if (MetaField->IsSystem || !MetaField->QueryText.isEmpty()) //���� ���� �������� ��������� - ���������� ���
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

	//��������� ������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qSelect.SetShowLongQuery(false);
	qSelect.BindValue(":Value", Value);
	if (!qSelect.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.SearchFullText.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//����������� �����
	QVariantList ResultList;
	while (qSelect.Next())
	{
		QVariant ID = qSelect.ReadColumn("id"),
			TableName = qSelect.ReadColumn("table_name");

		//�������� ��� �������
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

	//����������� �������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_CALENDAR_EVENTS);
	qSelect.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qSelect.BindValue(":Month", Month);
	qSelect.BindValue(":Year", Year);
	if (!qSelect.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GetCalendarEvents.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//������� ����������
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

	//������� �������
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), QD_CALENDAR);
	qDelete.BindValue(":ObjectID", ID);
	if (!qDelete.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.CalendarDelete.Delete").arg(qDelete.GetErrorString());
		return false;
	}

	//���� �� ���� ������ �� ���� ������� - ������ ����� ������ ��� - ������
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

	//����������� �����������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_INTERNAL_LISTS);
	if (!qSelect.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GetInternalLists.Select").arg(qSelect.GetErrorString());
		return false;
	}

	//������� ���������� �������
	QVariantList Lists;
	while (qSelect.Next())
	{
		Lists.append(qSelect.ReadColumn("intd_tablename"));
	}
	TcpAnswer->Parameters["Lists"] = Lists;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SaveMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//������������� � ������, ����� ���-�� ������ ����� ������
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_EXIT_APPLICATION);

	//������� ������� ��� �������� �����
	ISQuery qInsertColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QI_COLUMN_SIZE),
		qUpdateColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QU_COLUMN_SIZE);

	//�������� ������� ����� � ������� ��
	QVariantMap ColumnSize = TcpMessage->Parameters["ColumnSize"].toMap();
	for (const auto &TableItem : ColumnSize.toStdMap())
	{
		for (const auto &FieldItem : TableItem.second.toMap().toStdMap())
		{
			//������ �������� ������ ����
			qInsertColumnSize.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qInsertColumnSize.BindValue(":TableName", TableItem.first);
			qInsertColumnSize.BindValue(":FieldName", FieldItem.first);
			qInsertColumnSize.BindValue(":Size", FieldItem.second);
			if (!qInsertColumnSize.Execute()) //���� ������� �� ������� - ��������� �������
			{
				if (qInsertColumnSize.GetErrorNumber() == 23505) //������� - ��������� ������������ - ���������
				{
					qUpdateColumnSize.BindValue(":Size", FieldItem.second);
					qUpdateColumnSize.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
					qUpdateColumnSize.BindValue(":TableName", TableItem.first);
					qUpdateColumnSize.BindValue(":FieldName", FieldItem.first);
					if (!qUpdateColumnSize.Execute()) //�������� �� ����������
					{
						ErrorString = LANG("Carat.Error.Query.SaveMetaData.UpdateColumnSize").arg(qUpdateColumnSize.GetErrorString());
						return false;
					}
				}
				else //������� �� ������� �� ������ �������
				{
					ErrorString = LANG("Carat.Error.Query.SaveMetaData.InsertColumnSize").arg(qInsertColumnSize.GetErrorString());
					return false;
				}
			}
		}
	}

	//�������� �������� ���������
	QVariantMap SettingsMap = TcpMessage->Parameters["Settings"].toMap();
	if (!SettingsMap.isEmpty())
	{
		//������� ������ ��� ���������� ���������
		ISQuery qUpdateSetting(ISDatabase::Instance().GetDB(DBConnectionName), QU_SETTING);

		for (const auto &MapItem : SettingsMap.toStdMap())
		{
			qUpdateSetting.BindValue(":Value", MapItem.second);
			qUpdateSetting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qUpdateSetting.BindValue(":SettingUID", MapItem.first);
			if (!qUpdateSetting.Execute())
			{
				ErrorString = LANG("Carat.Error.Query.SaveMetaData.UpdateSetting").arg(qUpdateSetting.GetErrorString());
				return false;
			}
		}
	}

	//�������� ��������� �������
	QVariantMap FavoritesMap = TcpMessage->Parameters["Favorites"].toMap();
	if (FavoritesMap.isEmpty()) //���� �������� ��� - ������� ��, ��� ���� � ��
	{
		ISQuery qDeleteFavorites(ISDatabase::Instance().GetDB(DBConnectionName), QD_FAVIROTES_USER);
		qDeleteFavorites.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
		if (!qDeleteFavorites.Execute()) //�� ������� �������
		{
			ErrorString = LANG("Carat.Error.Query.SaveMetaData.DeleteUserFavorites").arg(qDeleteFavorites.GetErrorString());
			return false;
		}
	}
	else //������� ����
	{
		//�������
		QStringList Tables = FavoritesMap.keys();
		ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), QD_FAVORITES.replace(":Tables", "'{" + Tables.join(',') + "}'"));
		qDelete.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
		if (!qDelete.Execute())
		{
			ErrorString = LANG("Carat.Error.Query.SaveMetaData.DeleteFavorites").arg(qDelete.GetErrorString());
			return false;
		}

		//���������
		for (const auto &MapItem : FavoritesMap.toStdMap())
		{
			QString Objects;
			for (const QVariant &ObjectID : MapItem.second.toList())
			{
				Objects += ObjectID.toString() + ',';
			}
			Objects.chop(1);

			//��������� ��������� �������
			ISQuery qInsertFavorite(ISDatabase::Instance().GetDB(DBConnectionName), QI_FAVORITE.replace(":ObjectsID", "'{" + Objects + "}'"));
			qInsertFavorite.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qInsertFavorite.BindValue(":TableName", MapItem.first);
			if (!qInsertFavorite.Execute()) //������� �� �������
			{
				if (qInsertFavorite.GetErrorNumber() == 23505) //������� �� ������� ��-�� ����������� ������� - ���������
				{
					ISQuery qUpdateFavorites(ISDatabase::Instance().GetDB(DBConnectionName), QU_FAVORITE.replace(":ObjectsID", "'{" + Objects + "}'"));
					qUpdateFavorites.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
					qUpdateFavorites.BindValue(":TableName", MapItem.first);
					if (!qUpdateFavorites.Execute()) //�� ������� ��������
					{
						ErrorString = LANG("Carat.Error.Query.SaveMetaData.UpdateFavorite").arg(qUpdateFavorites.GetErrorString());
						return false;
					}
				}
				else //������� �� ������� �� ������ �������
				{
					ErrorString = LANG("Carat.Error.Query.SaveMetaData.InsertFavorite").arg(qInsertFavorite.GetErrorString());
					return false;
				}
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetGroupRights(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant GroupID = CheckNullField("GroupID", TcpMessage);
	if (!GroupID.isValid())
	{
		return false;
	}

	//�������� ������� � ����������
	QVariantList SystemsList;
	ISQuery qSelectSystems(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SYSTEM),
		qSelectSubSystems(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SUBSYSTEM);
	if (qSelectSystems.Execute()) //����������� ������ ������
	{
		while (qSelectSystems.Next())
		{
			QVariantMap SystemMap = { { "LocalName", qSelectSystems.ReadColumn("stms_localname") } };

			qSelectSubSystems.BindValue(":GroupID", GroupID);
			qSelectSubSystems.BindValue(":SystemUID", qSelectSystems.ReadColumn("stms_uid"));
			if (qSelectSubSystems.Execute())
			{
				while (qSelectSubSystems.Next())
				{
					QVariantList SubSystemsList = SystemMap["SubSystems"].toList();
					SubSystemsList.append(QVariantMap
					{
						{ "UID", ISUuid(qSelectSubSystems.ReadColumn("sbsm_uid")) },
						{ "LocalName", qSelectSubSystems.ReadColumn("sbsm_localname") },
						{ "IsExist", qSelectSubSystems.ReadColumn("is_exist") }
					});
					SystemMap["SubSystems"] = SubSystemsList;
				}
			}
			else
			{
				ErrorString = LANG("Carat.Error.Query.GetGroupRights.SelectSubSystems").arg(qSelectSubSystems.GetErrorString());
				return false;
			}
			SystemsList.append(SystemMap);
		}
	}
	else //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GetGroupRights.SelectSystems").arg(qSelectSystems.GetErrorString());
		return false;
	}

	//�������� ���� ���� ������� �� �������
	QVariantList RightsTableTypeList;
	ISQuery qSelectAccessTablesType(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_TABLE_TYPE);
	if (qSelectAccessTablesType.Execute())
	{
		while (qSelectAccessTablesType.Next())
		{
			RightsTableTypeList.append(QVariantMap
			{
				{ "UID", ISUuid(qSelectAccessTablesType.ReadColumn("gatt_uid")) },
				{ "LocalName", qSelectAccessTablesType.ReadColumn("gatt_name") },
				{ "Icon", qSelectAccessTablesType.ReadColumn("gatt_icon") }
			});
		}
	}
	else
	{
		ErrorString = LANG("Carat.Error.Query.GetGroupRights.RightTableType").arg(qSelectAccessTablesType.GetErrorString());
		return false;
	}

	//������� �������
	QVariantList TablesList;
	ISQuery qSelectTables(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_TABLE);
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		QVariantMap TableMap =
		{
			{ "TableName", MetaTable->Name },
			{ "LocalName", MetaTable->LocalListName },
			{ "Rights", QVariantList() }
		};
		qSelectTables.BindValue(":GroupID", GroupID);
		qSelectTables.BindValue(":TableName", MetaTable->Name);
		if (qSelectTables.Execute()) //����������� ����� �� ������� �������
		{
			while (qSelectTables.Next()) //������� ����� �� ������� �������
			{
				QVariantList RightList = TableMap["Rights"].toList();
				RightList.append(ISUuid(qSelectTables.ReadColumn("gatt_uid")));
				TableMap["Rights"] = RightList;
			}
		}
		else //������ �������
		{
			ErrorString = LANG("Carat.Error.Query.GetGroupRights.SelectTables").arg(qSelectTables.GetErrorString());
			return false;
		}
		TablesList.append(TableMap);
	}

	//�������� ����. �����
	QVariantList SpecialList;
	ISQuery qSelectSpecialParent(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SPECIAL_PARENT),
		qSelectSpecial(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SPECIAL);
	if (qSelectSpecialParent.Execute()) //����������� ������ ����. ����
	{
		while (qSelectSpecialParent.Next()) //������� ������ ����. ����
		{
			QVariantMap SpecialGroupMap = { { "LocalName", qSelectSpecialParent.ReadColumn("gast_name") } };

			qSelectSpecial.BindValue(":GroupID", GroupID);
			qSelectSpecial.BindValue(":ParentUID", qSelectSpecialParent.ReadColumn("gast_uid"));
			if (qSelectSpecial.Execute()) //����������� ����. �����
			{
				while (qSelectSpecial.Next()) //������� ����. �����
				{
					QVariantList VariantList = SpecialGroupMap["Rights"].toList();
					VariantList.append(QVariantMap
					{
						{ "UID", ISUuid(qSelectSpecial.ReadColumn("gast_uid")) },
						{ "LocalName", qSelectSpecial.ReadColumn("gast_name") },
						{ "Hint", qSelectSpecial.ReadColumn("gast_hint") },
						{ "IsExist", qSelectSpecial.ReadColumn("is_exist") }
					});
					SpecialGroupMap["Rights"] = VariantList;
				}
			}
			else //������ ������� � ����. ������
			{
				ErrorString = LANG("Carat.Error.Query.GetGroupRights.SelectSpecial").arg(qSelectSpecial.GetErrorString());
				return false;
			}
			SpecialList.append(SpecialGroupMap);
		}
	}
	else //������ ������� � ������� ����. ����
	{
		ErrorString = LANG("Carat.Error.Query.GetGroupRights.SelectSpecialGroup").arg(qSelectSpecialParent.GetErrorString());
		return false;
	}

	TcpAnswer->Parameters["Systems"] = SystemsList;
	TcpAnswer->Parameters["RightsTableType"] = RightsTableTypeList;
	TcpAnswer->Parameters["Tables"] = TablesList;
	TcpAnswer->Parameters["Special"] = SpecialList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSubSystemAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant GroupID = CheckNullField("GroupID", TcpMessage),
		SubSystemUID = CheckNullField("UID", TcpMessage);
	if (!GroupID.isValid() || !SubSystemUID.isValid())
	{
		return false;
	}

	//��������� �����
	ISQuery qInsertSubSystemRight(ISDatabase::Instance().GetDB(DBConnectionName), QI_GROUP_RIGHT_SUBSYSTEM);
	qInsertSubSystemRight.BindValue(":GroupID", GroupID);
	qInsertSubSystemRight.BindValue(":SubSystemUID", SubSystemUID);
	if (!qInsertSubSystemRight.Execute()) //������ �������
	{
		ErrorString = qInsertSubSystemRight.GetErrorNumber() == 23505 ?
			LANG("Carat.Error.Query.GroupRightSubSystemAdd.AlreadyExist") :
			LANG("Carat.Error.Query.GroupRightSubSystemAdd.Insert").arg(qInsertSubSystemRight.GetErrorString());
		return false;
	}

	//�� ������� ��������� ��� ����������
	if (!qInsertSubSystemRight.First())
	{
		ErrorString = qInsertSubSystemRight.GetErrorString();
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM, QVariant(), QVariant(), QVariant(), qInsertSubSystemRight.ReadColumn("sbsm_localname"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSubSystemDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant GroupID = CheckNullField("GroupID", TcpMessage),
		SubSystemUID = CheckNullField("UID", TcpMessage);
	if (!GroupID.isValid() || !SubSystemUID.isValid())
	{
		return false;
	}

	//������� �����
	ISQuery qDeleteSubSystemRight(ISDatabase::Instance().GetDB(DBConnectionName), QD_GROUP_RIGHT_SUBSYSTEM);
	qDeleteSubSystemRight.BindValue(":GroupID", GroupID);
	qDeleteSubSystemRight.BindValue(":SubSystemUID", SubSystemUID);
	if (!qDeleteSubSystemRight.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightSubSystemDelete.Delete").arg(qDeleteSubSystemRight.GetErrorString());
		return false;
	}

	//���� �� ���� ������ �� ���� ���������, ������ ������ ����� ��� - ������
	if (qDeleteSubSystemRight.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightSubSystemDelete.NotExist");
		return false;
	}

	if (!qDeleteSubSystemRight.First()) //�� ������� ������������� �� ������ ������
	{
		ErrorString = qDeleteSubSystemRight.GetErrorString();
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM, QVariant(), QVariant(), QVariant(), qDeleteSubSystemRight.ReadColumn("sbsm_localname"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightTableAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant GroupID = CheckNullField("GroupID", TcpMessage),
		TableName = CheckNullField("TableName", TcpMessage),
		AccessUID = CheckNullField("AccessUID", TcpMessage);
	if (!GroupID.isValid() || !TableName.isValid() || !AccessUID.isValid())
	{
		return false;
	}

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//��������� �����
	ISQuery qInsertTableRight(ISDatabase::Instance().GetDB(DBConnectionName), QI_GROUP_RIGHT_TABLE);
	qInsertTableRight.BindValue(":GroupID", GroupID);
	qInsertTableRight.BindValue(":TableName", MetaTable->Name);
	qInsertTableRight.BindValue(":AccessUID", AccessUID);
	if (!qInsertTableRight.Execute()) //������� �� �������
	{
		//���������� ��� ������
		ErrorString = qInsertTableRight.GetErrorNumber() == 23505 ?
			LANG("Carat.Error.Query.GroupRightTableAdd.AlreadyExist") :
			LANG("Carat.Error.Query.GroupRightTableAdd.Insert").arg(qInsertTableRight.GetErrorString());
		return false;
	}

	//�� ������� ���������
	if (!qInsertTableRight.First())
	{
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE, MetaTable->Name, MetaTable->LocalListName, QVariant(), qInsertTableRight.ReadColumn("gatt_name"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightTableDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);
	
	QVariant GroupID = CheckNullField("GroupID", TcpMessage),
		TableName = CheckNullField("TableName", TcpMessage),
		AccessUID = CheckNullField("AccessUID", TcpMessage);
	if (!GroupID.isValid() || !TableName.isValid() || !AccessUID.isValid())
	{
		return false;
	}

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//������� �����
	ISQuery qDeleteTableRight(ISDatabase::Instance().GetDB(DBConnectionName), QD_GROUP_RIGHT_TABLE);
	qDeleteTableRight.BindValue(":GroupID", GroupID);
	qDeleteTableRight.BindValue(":TableName", MetaTable->Name);
	qDeleteTableRight.BindValue(":AccessUID", AccessUID);
	if (!qDeleteTableRight.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightTableDelete.Delete").arg(qDeleteTableRight.GetErrorString());
		return false;
	}

	//���� �� ���� ������ �� ���� ���������, ������ ������ ����� ��� - ������
	if (qDeleteTableRight.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightTableDelete.NotExist");
		return false;
	}

	if (!qDeleteTableRight.First()) //�� ������� ������������� �� ������ ������
	{
		ErrorString = qDeleteTableRight.GetErrorString();
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE, MetaTable->Name, MetaTable->LocalListName, QVariant(), qDeleteTableRight.ReadColumn("gatt_name"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSpecialAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant GroupID = CheckNullField("GroupID", TcpMessage),
		SpecialRightUID = CheckNullField("SpecialRightUID", TcpMessage);
	if (!GroupID.isValid() || !SpecialRightUID.isValid())
	{
		return false;
	}

	//��������� ����. �����
	ISQuery qInsertSpecialRight(ISDatabase::Instance().GetDB(DBConnectionName), QI_GROUP_RIGHT_SPECIAL);
	qInsertSpecialRight.BindValue(":GroupID", GroupID);
	qInsertSpecialRight.BindValue(":SpecialRightUID", SpecialRightUID);
	if (!qInsertSpecialRight.Execute()) //������� �� ������� � ������ ������� � ��������� ������������
	{
		//���������� ��� ������
		ErrorString = qInsertSpecialRight.GetErrorNumber() == 23505 ?
			LANG("Carat.Error.Query.GroupRightSpecialAdd.AlreadyExist") :
			LANG("Carat.Error.Query.GroupRightSpecialAdd.Insert").arg(qInsertSpecialRight.GetErrorString());
		return false;
	}

	if (!qInsertSpecialRight.First())
	{
		ErrorString = qInsertSpecialRight.GetErrorString();
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL, QVariant(), QVariant(), QVariant(), qInsertSpecialRight.ReadColumn("gast_name"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSpecialDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant GroupID = CheckNullField("GroupID", TcpMessage),
		SpecialRightUID = CheckNullField("SpecialRightUID", TcpMessage);
	if (!GroupID.isValid() || !SpecialRightUID.isValid())
	{
		return false;
	}

	//������� ����. �����
	ISQuery qDeleteSpecialRight(ISDatabase::Instance().GetDB(DBConnectionName), QD_GROUP_RIGHT_SPECIAL);
	qDeleteSpecialRight.BindValue(":GroupID", GroupID);
	qDeleteSpecialRight.BindValue(":SpecialRightUID", SpecialRightUID);
	if (!qDeleteSpecialRight.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightSpecialDelete.Delete").arg(qDeleteSpecialRight.GetErrorString());
		return false;
	}

	//���� �� ���� ������ �� ���� ���������, ������ ������ ����� ��� - ������
	if (qDeleteSpecialRight.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightSpecialDelete.NotExist");
		return false;
	}

	if (!qDeleteSpecialRight.First()) //�� ������� ������������� �� ������ ������
	{
		ErrorString = qDeleteSpecialRight.GetErrorString();
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL, QVariant(), QVariant(), QVariant(), qDeleteSpecialRight.ReadColumn("gast_name"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordValue(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		FieldName = CheckNullField("FieldName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !FieldName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	PMetaField *MetaField = MetaTable->GetField(FieldName.toString());
	if (!MetaField)
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordValue.InvalidFieldName").arg(FieldName.toString());
		return false;
	}

	ISQuery qSelectValue(ISDatabase::Instance().GetDB(DBConnectionName),
		QString("SELECT %1_%2 FROM %3 WHERE %1_id = :ObjectID").arg(MetaTable->Alias).arg(MetaField->Name).arg(MetaTable->Name));
	qSelectValue.BindValue(":ObjectID", ObjectID);
	if (!qSelectValue.Execute())
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordValue.Select").arg(qSelectValue.GetErrorString());
		return false;
	}

	if (!qSelectValue.First())
	{
		ErrorString = LANG("Carat.Error.Query.GetRecordValue.RecordNotFound").arg(ObjectID.toInt());
		return false;
	}
	QVariant Value = qSelectValue.ReadColumn(0);
	TcpAnswer->Parameters["Value"] = Value.isNull() ? QVariant() : Value;
	return true;
}
//-----------------------------------------------------------------------------
