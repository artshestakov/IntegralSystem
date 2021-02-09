#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISLocalization.h"
#include "ISSystem.h"
#include "ISTrace.h"
#include "ISTcpQueue.h"
#include "ISConfig.h"
#include "ISTcpClients.h"
#include "ISMetaData.h"
#include "ISFail2Ban.h"
#include "ISConfigurations.h"
#include "ISVersionInfo.h"
#include "ISProperty.h"
#include "ISTcpWorkerHelper.h"
//-----------------------------------------------------------------------------
static QString QS_USERS_HASH = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
											 "FROM _users "
											 "WHERE usrs_hash IS NOT NULL "
											 "AND usrs_salt IS NOT NULL");
//-----------------------------------------------------------------------------
static QString QS_USER_AUTH = PREPARE_QUERY("SELECT usrs_id, usrs_issystem, usrs_group, usrs_fio, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, usgp_fullaccess, "
											"(SELECT sgdb_useraccessdatabase FROM _settingsdatabase WHERE sgdb_uid = :SettingUID), "
											"udvc_hash, udvc_salt "
											"FROM _users "
											"LEFT JOIN _usergroup ON usgp_id = usrs_group "
											"LEFT JOIN _userdevice ON udvc_user = usrs_id "
											"WHERE usrs_hash = :Hash");
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("SELECT protocol_user(:UserID, :TableName, :TableLocalName, :TypeUID, :ObjectID, :Information)");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_TABLE = PREPARE_QUERY("SELECT gatb_table, gatt_uid "
													 "FROM _groupaccesstable "
													 "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_AccessType "
													 "WHERE gatb_group = (SELECT usrs_group FROM _users WHERE usrs_id = :UserID)");
//-----------------------------------------------------------------------------
static QString QS_GROUP_ACCESS_SPECIAL = PREPARE_QUERY("SELECT gast_uid "
													   "FROM _groupaccessspecial "
													   "LEFT JOIN _groupaccessspecialtype ON gast_id = gasp_specialaccess "
													   "WHERE gasp_group = (SELECT usrs_group FROM _users WHERE usrs_id = :UserID)");
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
static QString QS_FAVORITE = PREPARE_QUERY("SELECT fvts_tablename, fvts_objectid "
										   "FROM _favorites "
										   "WHERE fvts_user = :UserID");
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
static QString QS_TASK_PRIORITY = PREPARE_QUERY("SELECT tspr_id, tspr_name, tspr_tooltip, tspr_stylesheet, tspr_icon "
												"FROM _taskpriority "
												"ORDER BY tspr_order");
//-----------------------------------------------------------------------------
static QString QU_USER_HASH = PREPARE_QUERY("UPDATE _users SET "
											"usrs_hash = :Hash, "
											"usrs_salt = :Salt "
											"WHERE usrs_id = :UserID "
											"RETURNING usrs_fio");
//-----------------------------------------------------------------------------
static QString QS_USER_PASSWORD = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
												"FROM _users "
												"WHERE usrs_id = :UserID");
//-----------------------------------------------------------------------------
static QString QU_USER_HASH_RESET = PREPARE_QUERY("UPDATE _users SET "
												  "usrs_hash = NULL, "
												  "usrs_salt = NULL "
												  "WHERE usrs_id = :UserID "
												  "RETURNING usrs_fio");
//-----------------------------------------------------------------------------
static QString QU_USER_SETTINGS_RESET = PREPARE_QUERY("UPDATE _usersettings SET "
													  "usst_value = (SELECT stgs_defaultvalue FROM _settings WHERE stgs_id = usst_setting) "
													  "WHERE usst_user = :UserID "
													  "RETURNING (SELECT stgs_uid FROM _settings WHERE stgs_id = usst_setting), usst_value");
//-----------------------------------------------------------------------------
static QString QS_USER_DEVICE = PREPARE_QUERY("SELECT COUNT(*) "
											  "FROM _userdevice "
											  "WHERE udvc_user = :UserID");
//-----------------------------------------------------------------------------
static QString QI_USER_DEVICE = PREPARE_QUERY("INSERT INTO _userdevice(udvc_user, udvc_hash, udvc_salt) "
											  "VALUES(:UserID, :Hash, :Salt) "
											  "RETURNING (SELECT usrs_fio FROM _users WHERE usrs_id = :UserID)");
//-----------------------------------------------------------------------------
static QString QD_USER_DEVICE = PREPARE_QUERY("DELETE FROM _userdevice "
											  "WHERE udvc_user = :UserID "
											  "RETURNING (SELECT usrs_fio FROM _users WHERE usrs_id = :UserID)");
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
static QString QS_CLIENTS = PREPARE_QUERY("SELECT usrs_id, usrs_fio, usgp_name, usrs_photo "
										  "FROM _users "
										  "LEFT JOIN _usergroup ON usgp_id = usrs_group "
										  "ORDER BY usrs_fio");
//-----------------------------------------------------------------------------
static QString QS_RECORD_INFO = PREPARE_QUERY("SELECT "
											  "( "
											  "SELECT prtc_datetime AS create_date "
											  "FROM _protocol "
											  "LEFT JOIN _users ON usrs_id = prtc_user "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{D1348312-298F-4A7C-B584-9BA8C4952CD3}') "
											  "ORDER BY prtc_datetime ASC "
											  "LIMIT 1 "
											  "), "
											  "( "
											  "SELECT usrs_fio AS create_user "
											  "FROM _protocol "
											  "LEFT JOIN _users ON usrs_id = prtc_user "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{D1348312-298F-4A7C-B584-9BA8C4952CD3}') "
											  "ORDER BY prtc_datetime ASC "
											  "LIMIT 1 "
											  "), "
											  "( "
											  "SELECT prtc_datetime AS edit_date "
											  "FROM _protocol "
											  "LEFT JOIN _users ON usrs_id = prtc_user "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{0361643D-0A62-4F51-84BD-313F53115EFD}') "
											  "ORDER BY prtc_datetime DESC "
											  "LIMIT 1 "
											  "), "
											  "( "
											  "SELECT usrs_fio AS edit_user "
											  "FROM _protocol "
											  "LEFT JOIN _users ON usrs_id = prtc_user "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{0361643D-0A62-4F51-84BD-313F53115EFD}') "
											  "ORDER BY prtc_datetime DESC "
											  "LIMIT 1 "
											  "), "
											  "( "
											  "SELECT COUNT(*) AS copy_count "
											  "FROM _protocol "
											  "LEFT JOIN _users ON usrs_id = prtc_user "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{EFA8FE45-1174-4D2E-BBE6-4940380961D4}') "
											  "), "
											  "( "
											  "SELECT COUNT(*) AS edit_count "
											  "FROM _protocol "
											  "LEFT JOIN _users ON usrs_id = prtc_user "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{0361643D-0A62-4F51-84BD-313F53115EFD}') "
											  "), "
											  "( "
											  "SELECT COUNT(*) AS show_count "
											  "FROM _protocol "
											  "LEFT JOIN _users ON usrs_id = prtc_user "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{117E8972-97DC-4E72-93AC-DC3BB50D11CF}') "
											  "), "
											  "( "
											  "SELECT COUNT(*) favorite_count "
											  "FROM _favorites "
											  "WHERE fvts_tablename = :TableName "
											  "AND fvts_objectid = :ObjectID "
											  "), "
											  "( "
											  "SELECT COUNT(*) AS protocol_count "
											  "FROM _protocol "
											  "WHERE prtc_tablename = :TableName "
											  "AND prtc_objectid = :ObjectID "
											  "), "
											  "( "
											  "SELECT COUNT(*) AS discussion_count "
											  "FROM _discussion "
											  "WHERE dson_tablename = :TableName "
											  "AND dson_objectid = :ObjectID "
											  ") ");
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
static QString QS_RIGHT_SHOW_TABLE = PREPARE_QUERY("SELECT check_access_user_table(:UserID, :TableName, :AccessTypeUID)");
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
static QString QI_FAVORITE = PREPARE_QUERY("INSERT INTO _favorites(fvts_user, fvts_tablename, fvts_objectid) "
										   "VALUES(:UserID, :TableName, :ObjectID)");
//-----------------------------------------------------------------------------
static QString QD_FAVORITE = PREPARE_QUERY("DELETE FROM _favorites "
										   "WHERE fvts_user = :UserID "
										   "AND fvts_tablename = :TableName "
										   "AND fvts_objectid = :ObjectID");
//-----------------------------------------------------------------------------
static QString QS_FAVORITE_NAMES_TABLE = PREPARE_QUERY("SELECT fvts_tablename, fvts_objectid "
													   "FROM _favorites "
													   "WHERE fvts_user = :UserID "
													   "AND fvts_tablename = :TableName "
													   "ORDER BY fvts_tablename");
//-----------------------------------------------------------------------------
static QString QS_FAVORITE_NAMES_ALL = PREPARE_QUERY("SELECT fvts_tablename, fvts_objectid "
													 "FROM _favorites "
													 "WHERE fvts_user = :UserID "
													 "ORDER BY fvts_tablename");
//-----------------------------------------------------------------------------
static QString QD_FAVORITES_TABLE = PREPARE_QUERY("DELETE FROM _favorites "
												  "WHERE fvts_user = :UserID "
												  "AND fvts_tablename = :TableName");
//-----------------------------------------------------------------------------
static QString QD_FAVORITES_ALL = PREPARE_QUERY("DELETE FROM _favorites "
												"WHERE fvts_user = :UserID");
//-----------------------------------------------------------------------------
static QString QU_CALENDAR_CLOSE = PREPARE_QUERY("UPDATE _calendar SET "
												 "cldr_closed = true "
												 "WHERE cldr_id = :CalendarID "
												 "RETURNING cldr_name");
//-----------------------------------------------------------------------------
static QString QS_HISTORY = PREPARE_QUERY("SELECT prtc_datetime, prtc_tablename, prtc_objectid "
										  "FROM _protocol "
										  "WHERE prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{117E8972-97DC-4E72-93AC-DC3BB50D11CF}') "
										  "AND prtc_user = :UserID "
										  "ORDER BY prtc_datetime DESC");
//-----------------------------------------------------------------------------
static QString QI_TASK_COMMENT = PREPARE_QUERY("INSERT INTO _taskcomment(tcom_owner, tcom_task, tcom_parent, tcom_comment) "
											   "VALUES(:UserID, :TaskID, :ParentCommentID, :Comment)");
//-----------------------------------------------------------------------------
static QString QS_SERVER_INFO = PREPARE_QUERY("SELECT "
											  "(SELECT pg_size_pretty(pg_database_size(current_database()))) AS database_size_full, "
											  "(SELECT pg_size_pretty(sum(pg_indexes_size(tablename::VARCHAR))) AS database_size_indexes FROM pg_tables WHERE schemaname = current_schema()), "
											  "(SELECT pg_size_pretty(sum(pg_relation_size(tablename::VARCHAR))) AS database_size_data FROM pg_tables WHERE schemaname = current_schema()), "
											  "(SELECT pg_catalog.pg_get_userbyid(datdba) AS database_owner FROM pg_catalog.pg_database WHERE datname = current_database()), "
											  "(SELECT pg_encoding_to_char(encoding) AS database_encoding FROM pg_database WHERE datname = current_database()), "
											  "(SELECT now() - pg_postmaster_start_time() AS database_uptime), "
											  "(SELECT pg_backend_pid() AS database_backend_pid), "
											  "(SELECT version()) AS database_version, "
											  "(SELECT setting AS database_cluster_path FROM pg_settings WHERE name = 'data_directory'), "
											  "(SELECT COUNT(*) AS table_count FROM information_schema.tables WHERE table_catalog = current_database() AND table_schema = current_schema()), "
											  "(SELECT COUNT(*) AS field_count FROM information_schema.columns WHERE table_catalog = current_database() AND table_schema = current_schema()), "
											  "(SELECT COUNT(*) AS sequence_count FROM information_schema.sequences WHERE sequence_catalog = current_database() AND sequence_schema = current_schema()), "
											  "(SELECT COUNT(*) AS index_count FROM pg_indexes WHERE schemaname = current_schema()), "
											  "(SELECT COUNT(*) AS foreign_count FROM information_schema.constraint_table_usage WHERE constraint_catalog = current_database() AND constraint_schema = current_schema()), "
											  "(SELECT get_rows_count() AS rows_count), "
											  "(SELECT COUNT(*) AS protocol_count FROM _protocol), "
											  "(SELECT COUNT(*) AS users_count FROM _users)");
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	IsStarted(false),
	qProtocol(nullptr),
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
void ISTcpWorker::SetDB(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password)
{
	DBHost = db_host;
	DBPort = db_port;
	DBName = db_name;
	DBUser = db_user;
	DBPassword = db_password;
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
	if (IsStarted)
	{
		qProtocol = new ISQuery(ISDatabase::Instance().GetDB(DBConnectionName), QI_PROTOCOL);
	}
	else
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
	}

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
			ISLOGGER_I(__CLASS__, "Stopping ...");
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
			unsigned long long PerfomanceMsec = 0;
			ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpSocket); //Заранее формируем ответ

			if (tcp_message->IsValid()) //Если сообщение валидное - переходим к выполнению
			{
				//Если запрос не авторизационный и клиент ещё не авторизовался - ошибка
				if (tcp_message->Type != ISNamespace::ApiMessageType::Auth && !tcp_message->TcpSocket->GetAuthorized())
				{
					ErrorString = LANG("Carat.Error.NotAuthorized");
				}
				else //Клиент авторизовался - продолжаем
				{
					ISTimePoint TimePoint = ISAlgorithm::GetTick(); //Запоминаем текущее время
					Result = Execute(tcp_message, TcpAnswer);
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
			ISTcpClients::Instance().UpdateLastQuery(tcp_message->TcpSocket->GetSocketDescriptor(), tcp_message->Type, Result, PerfomanceMsec);
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
void ISTcpWorker::Protocol(unsigned int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName, const QVariant &TableLocalName, const QVariant &ObjectID, const QVariant &Information)
{
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
		return ErrorQuery(LANG("Carat.Error.CheckExistUserPassword"), qSelectHashIsNull);
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
		return ErrorQuery(LANG("Carat.Error.CheckUserIsSystem"), qSelectIsSystem);
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
	for (const QString &FieldName : StringList) //Обходим список полей
	{
		PMetaForeign *MetaForeign = MetaTable->GetField(FieldName)->Foreign;
		QueryText += MetaForeign ?
			("(" + ISTcpWorkerHelper::GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "),") :
			(MetaTable->Alias + '_' + FieldName + ',');
	}
	QueryText.chop(1);
	QueryText += "\nFROM " + MetaTable->Name;
	QueryText += "\nWHERE " + MetaTable->Alias + "_id = :ObjectID";

	//Запрашиваем имя
	ISQuery qSelectName(ISDatabase::Instance().GetDB(DBConnectionName), QueryText);
	qSelectName.SetShowLongQuery(false);
	qSelectName.BindValue(":ObjectID", ObjectID);
	if (!qSelectName.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.GetObjectName"), qSelectName);
	}

	if (!qSelectName.First()) //Запись не найдена
	{
		return true;
	}
	
	//Получаем запись и анализируем её
	QSqlRecord SqlRecord = qSelectName.GetRecord();
	for (int i = 0, c = SqlRecord.count(); i < c; ++i)
	{
		QVariant Value = SqlRecord.value(i);
		switch (Value.type())
		{
		case QVariant::Date: ObjectName += Value.toDate().toString(FORMAT_DATE_V2); break;
		case QVariant::Time: ObjectName += Value.toTime().toString(FORMAT_TIME_V1); break;
		case QVariant::DateTime: ObjectName += Value.toDateTime().toString(FORMAT_DATE_TIME_V2); break;
		default:
			ObjectName += Value.toString();
		}
		ObjectName += ' ';
	}

	//Удаляем возможные пробелы в конце имени объекта
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
bool ISTcpWorker::Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	switch (TcpMessage->Type)
	{
	case ISNamespace::ApiMessageType::Unknown: break;
	case ISNamespace::ApiMessageType::Auth: return Auth(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::Sleep: return Sleep(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetMetaData: return GetMetaData(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetLastClient: return GetLastClient(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::UserPasswordExist: return UserPasswordExist(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::UserPasswordCreate: return UserPasswordCreate(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::UserPasswordEdit: return UserPasswordEdit(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::UserPasswordReset: return UserPasswordReset(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::UserSettingsReset: return UserSettingsReset(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::UserDeviceAdd: return UserDeviceAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::UserDeviceDelete: return UserDeviceDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetRecordCall: return GetRecordCall(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetClients: return GetClients(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::RecordAdd: return RecordAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::RecordEdit: return RecordEdit(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::RecordDelete: return RecordDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::RecordGet: return RecordGet(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::RecordGetInfo: return RecordGetInfo(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::DiscussionAdd: return DiscussionAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::DiscussionEdit: return DiscussionEdit(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::DiscussionCopy: return DiscussionCopy(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetTableData: return GetTableData(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetTableQuery: return GetTableQuery(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::NoteRecordGet: return NoteRecordGet(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::NoteRecordSet: return NoteRecordSet(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::FileStorageAdd: return FileStorageAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::FileStorageCopy: return FileStorageCopy(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::FileStorageGet: return FileStorageGet(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::SearchTaskText: return SearchTaskText(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::SearchTaskID: return SearchTaskID(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::SearchFullText: return SearchFullText(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetCalendarEvents: return GetCalendarEvents(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::CalendarDelete: return CalendarDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetInternalLists: return GetInternalLists(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::SaveMetaData: return SaveMetaData(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetGroupRights: return GetGroupRights(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GroupRightSubSystemAdd: return GroupRightSubSystemAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GroupRightSubSystemDelete: return GroupRightSubSystemDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GroupRightTableAdd: return GroupRightTableAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GroupRightTableDelete: return GroupRightTableDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GroupRightSpecialAdd: return GroupRightSpecialAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GroupRightSpecialDelete: return GroupRightSpecialDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetRecordValue: return GetRecordValue(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::RecordFavoriteAdd: return RecordFavoriteAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::RecordFavoriteDelete: return RecordFavoriteDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetFavoriteNames: return GetFavoriteNames(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::FavoritesDelete: return FavoritesDelete(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::LogGetStructure: return LogGetStructure(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::LogGetContent: return LogGetContent(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::CalendarClose: return CalendarClose(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetHistoryList: return GetHistoryList(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::TaskCommentAdd: return TaskCommentAdd(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetForeignList: return GetForeignList(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::GetServerInfo: return GetServerInfo(TcpMessage, TcpAnswer); break;
	case ISNamespace::ApiMessageType::OrganizationFromINN: return OrganizationFormINN(TcpMessage, TcpAnswer); break;
	default:
		ErrorString = LANG("Carat.Error.NotExistFunction").arg(TcpMessage->TypeName);
	}
	return false;
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
bool ISTcpWorker::ErrorQuery(const QString &LocalError, ISQuery &SqlQuery)
{
	ErrorString = LocalError;
	ISLOGGER_E(__CLASS__, QString("Sql query:\n%1\n%2").arg(SqlQuery.GetSqlText()).arg(SqlQuery.GetErrorString()));
	return false;
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
	if (ISConfigurations::Instance().Get().DateExpired.isValid() &&
		QDate::currentDate() > ISConfigurations::Instance().Get().DateExpired)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AuthDenied");
		return false;
	}

	//Проверяем, не заблокирован ли адрес
	QString IPAddress = TcpMessage->TcpSocket->GetAddress();
	if (ISFail2Ban::Instance().IsLock(IPAddress))
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AddressIsLocked").arg(IPAddress).arg(ISFail2Ban::Instance().GetUnlockDateTime(IPAddress).toString(FORMAT_DATE_TIME_V2));
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
			return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectHash"), qSelectHash);
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

		if (IsFound) //Нашли пользователя - удаляем адрес из Fail2Ban
		{
			ISFail2Ban::Instance().Remove(IPAddress);
		}
		else //Не нашли пользователя - добавляем адрес в Fail2Ban
		{
			//Если адрес заблокирован - сообщаем об этом
			//Иначе - предупреждаем о неправильном вводе логина или пароля
			ErrorString = ISFail2Ban::Instance().Add(IPAddress)
				? LANG("Carat.Error.Query.Auth.Fail2Ban").arg(CARAT_BAN_ATTEMPT_COUNT).arg(IPAddress).arg(ISFail2Ban::Instance().GetUnlockDateTime(IPAddress).toString(FORMAT_DATE_TIME_V2))
				: LANG("Carat.Error.Query.Auth.InvalidLoginOrPassword");
			return false;
		}
	}

	//Проверка пользователя
	ISQuery qSelectAuth(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_AUTH);
	qSelectAuth.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	qSelectAuth.BindValue(":Hash", Hash);
	if (!qSelectAuth.ExecuteFirst()) //Запрос выполнен с ошибкой
	{
		return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectLogin"), qSelectAuth);
	}

	unsigned int UserID = qSelectAuth.ReadColumn("usrs_id").toUInt();
	bool IsSystem = qSelectAuth.ReadColumn("usrs_issystem").toBool();
	QString UserFIO = qSelectAuth.ReadColumn("usrs_fio").toString();
	int GroupID = qSelectAuth.ReadColumn("usrs_group").toInt();
	bool GroupFullAccess = qSelectAuth.ReadColumn("usgp_fullaccess").toBool();
	QString DeviceHash = qSelectAuth.ReadColumn("udvc_hash").toString();
	QString DeviceSalt = qSelectAuth.ReadColumn("udvc_salt").toString();

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

	if (!DeviceHash.isEmpty() && !DeviceSalt.isEmpty()) //Устройство привязано
	{
		//Проверяем, передал ли клиент хеши устройств
		if (!TcpMessage->Parameters.contains("DeviceList"))
		{
			ErrorString = LANG("Carat.Error.Query.Auth.DeviceList.NotExist");
			return false;
		}

		//Проверяем наличие устройств в списке
		QStringList DeviceList = TcpMessage->Parameters["DeviceList"].toStringList();
		if (DeviceList.isEmpty())
		{
			ErrorString = LANG("Carat.Error.Query.Auth.DeviceList.Empty");
			return false;
		}

		//Обходим список устройств
		bool DeviceChecked = false;
		for (const QString &DeviceListHash : DeviceList)
		{
			DeviceChecked = ISAlgorithm::SaltPassword(DeviceListHash, DeviceSalt) == DeviceHash;
			if (DeviceChecked)
			{
				break;
			}
		}

		if (!DeviceChecked)
		{
			ErrorString = LANG("Carat.Error.Query.Auth.Device.NotConnected");
			return false;
		}
	}

	//Проверяем версию клиента
	bool IsNeedUpdate = false, VersionIsValid = false; //По умолчанию флаг обновления должен быть false (вдруг клиент отправил невалидную версию)
	unsigned int VersionLast = 0;
	if (TcpMessage->Parameters.contains("Version")) //Если версия указана
	{
		//Проверяем его версию на валидность
		QVariant Version = TcpMessage->Parameters["Version"];
		unsigned int VersionClient = Version.toUInt(&VersionIsValid);
		if (VersionIsValid) //Версия валидна - ищем последний файл
		{
			//Получаем директорию с обновлениями
			QString UpdateClientDir = CONFIG_STRING(CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR);
			if (!UpdateClientDir.isEmpty()) //Если директория настроена - идём дальше
			{
				//Проверяем существование директории с обновлениями
				QDir Dir(UpdateClientDir);
				if (!Dir.exists())
				{
					ISLOGGER_W(__CLASS__, "Folder \"" + UpdateClientDir + "\" not exist. Check config parameter \"" + CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR + "\"");
					ErrorString = LANG("Carat.Error.Query.Auth.UpdatePathNotExist");
					return false;
				}

				//Получаем отсортированный (по дате) список файлов и проверяем его на пустоту
				QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
				if (!StringList.isEmpty()) //Если обновления есть - вытаскиваем версию последнего файла
				{
					QString FilePath = Dir.absolutePath() + '/' + StringList.front();
					VersionLast = ISAlgorithm::ExtractVersionFile(FilePath);
					if (VersionLast > 0) //Формат имени файла валиден
					{
						IsNeedUpdate = VersionLast > VersionClient;
					}
					else //Невалидный формат имени файла
					{
						ISLOGGER_W(__CLASS__, "Invalid format update file name: " + FilePath);
					}
				}
				//Обновлений нет - идём дальше
			}
			//Директория не настроена - логируем предупреждение и идём дальше
			ISLOGGER_W(__CLASS__, "Not setting directory updates");
		}
		else //Версия невалидна
		{
			ISLOGGER_W(__CLASS__, "Client version invalid: " + Version.toString());
		}
	}

	//Проверяем, не подключен ли уже это пользователь
	//Если пользователь уже подключен - вытаскиваем информацию о подключении
	if (ISTcpClients::Instance().ExistUserID(UserID))
	{
		ISClientInfo ClientInfo = ISTcpClients::Instance().GetClient(UserID);
		TcpAnswer->Parameters["AlreadyConnected"] = QVariantMap
		{
			{ "DateTime", ClientInfo.DTConnected.toString(FORMAT_DATE_TIME_V2) },
			{ "IPAddress", ClientInfo.Address }
		};
	}

	//Устанавливаем флаги сокету
	TcpMessage->TcpSocket->SetAuthorized(true);
	TcpMessage->TcpSocket->SetUserID(UserID);
	TcpMessage->TcpSocket->SetUserIsSystem(IsSystem);

	//Отдаём информацию о пользователе и конфигурации
	TcpAnswer->Parameters["UserID"] = UserID;
	TcpAnswer->Parameters["UserIsSystem"] = IsSystem;
	TcpAnswer->Parameters["UserFIO"] = UserFIO;
	TcpAnswer->Parameters["UserGroupID"] = GroupID;
	TcpAnswer->Parameters["UserGroupFullAccess"] = GroupFullAccess;
	TcpAnswer->Parameters["Server"] = QVariantMap
	{
		{ "Version", ISVersionInfo::Instance().Info.Version }
	};
	TcpAnswer->Parameters["UpdateClient"] = QVariantMap
	{
		{ "IsNeed", IsNeedUpdate },
		{ "NewVersion", IsNeedUpdate ? VersionLast : QVariant() },
	};
	TcpAnswer->Parameters["Configuration"] = QVariantMap
	{
		{ "Name", ISConfigurations::Instance().Get().Name },
		{ "UID", ISConfigurations::Instance().Get().UID },
		{ "LocalName", ISConfigurations::Instance().Get().LocalName },
		{ "DesktopForm", ISConfigurations::Instance().Get().DesktopForm },
		{ "DateExpired", ISConfigurations::Instance().Get().DateExpired.toString(FORMAT_DATE_V2) },
		{ "LogoName", ISConfigurations::Instance().Get().LogoName }
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
	unsigned int TimeoutInt = Timeout.toUInt(&Ok);
	if (!Ok) //Не удалось привести значение к числу
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueInvalid");
		return false;
	}

	if (TimeoutInt == 0) //Если значение равно нулю - ошибка
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueIsNull");
		return false;
	}
	ISSleep(TimeoutInt);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Готовим запрос для получения настроек БД
	PMetaTable *MetaTable = GetMetaTable("_SettingsDatabase");
	QString SqlText = "SELECT sgdb_id AS \"ID\",";
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		if (!MetaField->IsSystem)
		{
			SqlText += MetaTable->Alias + '_' + MetaField->Name.toLower() + " AS \"" + MetaField->Name + "\",";
		}
	}
	SqlText.chop(1);
	SqlText += " FROM _settingsdatabase WHERE sgdb_uid = :SettingUID";

	//Получаем настройки БД
	QVariantMap SettingsDBMap;
	ISQuery qSelectSettingsDB(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qSelectSettingsDB.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectSettingsDB.Execute())
	{
		if (qSelectSettingsDB.First())
		{
			QSqlRecord SqlRecord = qSelectSettingsDB.GetRecord();
			for (int i = 0, c = SqlRecord.count(); i < c; ++i)
			{
				QSqlField SqlField = SqlRecord.field(i);
				SettingsDBMap[SqlField.name()] = SqlField.value();
			}
		}
		else
		{
			ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsDBNotActive");
			return false;
		}
	}
	else
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsDB"), qSelectSettingsDB);
	}

	//Получаем права на таблицы
	QVariantMap AccessTablesMap;
	ISQuery qSelectAccessTables(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_TABLE);
	qSelectAccessTables.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
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
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessTable"), qSelectAccessTables);
	}

	//Получаем специальные права
	QVariantList AccessSpecialList;
	ISQuery qSelectAccessSpecial(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_ACCESS_SPECIAL);
	qSelectAccessSpecial.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectAccessSpecial.Execute())
	{
		while (qSelectAccessSpecial.Next())
		{
			AccessSpecialList.append(ISUuid(qSelectAccessSpecial.ReadColumn("gast_uid")));
		}
	}
	else
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial"), qSelectAccessSpecial);
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
						{ "UID", ISUuid(qSelectSubSystem.ReadColumn("sbsm_uid")) },
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
				return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SubSystems"), qSelectSubSystem);
			}
			
			//Добавляем систему только если по ней разрешены какие-нибудь подсистемы
			if (qSelectSubSystem.GetCountResultRows() > 0)
			{
				SystemSubSystemList.append(QVariantMap
				{
					{ "UID", ISUuid(qSelectSystem.ReadColumn("stms_uid")) },
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
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Systems"), qSelectSystem);
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
			QVariantList Objects = FavoriteMap[TableName].toList();
			Objects.append(qSelectFavorite.ReadColumn("fvts_objectid"));
			FavoriteMap[TableName] = Objects;
		}
	}
	else
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Favorite"), qSelectFavorite);
	}

	//Получаем размеры полей
	QVariantMap ColumnSizeMap;
	ISQuery qSelectColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QS_COLUMN_SIZE);
	qSelectColumnSize.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (qSelectColumnSize.Execute())
	{
		while (qSelectColumnSize.Next())
		{
			QString TableName = qSelectColumnSize.ReadColumn("clsz_tablename").toString();
			QVariantMap VariantMap = ColumnSizeMap[TableName].toMap();
			VariantMap[qSelectColumnSize.ReadColumn("clsz_fieldname").toString()] = qSelectColumnSize.ReadColumn("clsz_size");
			ColumnSizeMap[TableName] = VariantMap;
		}
	}
	else
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.ColumnSize"), qSelectColumnSize);
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
							//???
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
				return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsUser"), qSelectSettingUser);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsGroup"), qSelectSettingGroup);
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
				{ "UID", ISUuid(qSelectParagraph.ReadColumn("prhs_uid")) },
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
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Paragraph"), qSelectParagraph);
	}

	//Получаем приоритеты задач
	QVariantList TaskPriorityList;
	ISQuery qSelectTaskPriority(ISDatabase::Instance().GetDB(DBConnectionName), QS_TASK_PRIORITY);
	if (qSelectTaskPriority.Execute())
	{
		while (qSelectTaskPriority.Next())
		{
			TaskPriorityList.append(QVariantMap
			{
				{ "ID", qSelectTaskPriority.ReadColumn("tspr_id") },
				{ "LocalName", qSelectTaskPriority.ReadColumn("tspr_name") },
				{ "ToolTip", qSelectTaskPriority.ReadColumn("tspr_tooltip") },
				{ "StyleSheet", qSelectTaskPriority.ReadColumn("tspr_stylesheet") },
				{ "Icon", qSelectTaskPriority.ReadColumn("tspr_icon") },
			});
		}
	}
	else
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.TaskPriority"), qSelectTaskPriority);
	}

	//Читаем мета-данные
	QVariantList MetaDataList;
	QStringList Filter("*.xsn"); //Фильтр
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //Загрузка мета-данных движка
	FileInfoList.append(QDir(":_" + ISConfigurations::Instance().Get().Name).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //Загрузка мета-данных конфигурации
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

	TcpAnswer->Parameters["SettingsDB"] = SettingsDBMap;
	TcpAnswer->Parameters["AccessTables"] = AccessTablesMap;
	TcpAnswer->Parameters["AccessSpecial"] = AccessSpecialList;
	TcpAnswer->Parameters["SystemSubSystem"] = SystemSubSystemList;
	TcpAnswer->Parameters["Favorite"] = FavoriteMap;
	TcpAnswer->Parameters["ColumnSize"] = ColumnSizeMap;
	TcpAnswer->Parameters["Settings"] = Settings;
	TcpAnswer->Parameters["Paragraphs"] = ParagraphList;
	TcpAnswer->Parameters["MetaData"] = MetaDataList;
	TcpAnswer->Parameters["TaskPriority"] = TaskPriorityList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Получаем отсортированный (по дате) список файлов
	//в настроенной директории и проверяем его на пустоту
	QDir Dir(CONFIG_STRING(CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR));
	QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
	if (StringList.isEmpty()) //Если обновлений нет - выходим
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//Открываем файл
	QString FilePath = Dir.absolutePath() + '/' + StringList.front();
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

	//Протоколируем, конвертируем в base64 и отдаём
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
		ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.GenerateSalt");
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
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordCreate.UpdateHash"), qUpdateHash);
	}
	qUpdateHash.First(); //Проверка не требуется, она выполняется выше

	//Фиксируем создание пароля
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_PASSWORD_CREATE, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID, qUpdateHash.ReadColumn("usrs_fio"));
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
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.SelectHash"), qSelectHash);
	}

	if (!qSelectHash.First()) //Пользователя с таким UserID нет в БД
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordEdit.UserNotExist").arg(UserID.toInt());
		return false;
	}

	//Текущие хэш и соль
	QString CurrentHash = qSelectHash.ReadColumn("usrs_hash").toString(),
		CurrentSalt = qSelectHash.ReadColumn("usrs_salt").toString();

	//Солим присланный хэш и проверяем. Если подсоленый хэш не соответствует тому что в БД - значит вводили неправильный текущий пароль (или логин) - ошибка
	if (ISAlgorithm::SaltPassword(HashOld.toString(), CurrentSalt) != CurrentHash)
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordEdit.InvalidCurrentLoginOrPassword");
		return false;
	}

	//Генерируем новую соль и солим новый пароль
	if (!ISAlgorithm::GenerateSalt(CurrentSalt, ErrorString)) //Ошибка генерации
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordEdit.GenerateSalt");
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
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.UpdateHash"), qUpdateHash);
	}

	//Фиксируем изменение пароля
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID, qUpdateHash.ReadColumn("usrs_fio"));
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
		ErrorString = LANG("Carat.Error.Query.UserPasswordReset.UserIsSystem");
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
		ErrorString = LANG("Carat.Error.Query.UserPasswordReset.PasswordIsNull");
		return false;
	}

	//Сбрасываем пароль
	ISQuery qUpdateHashReset(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH_RESET);
	qUpdateHashReset.BindValue(":UserID", UserID);
	if (!qUpdateHashReset.Execute()) //Не удалось сбросить пароль
	{
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordReset.Reset"), qUpdateHashReset);
	}

	//Фиксируем сброс пароля
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_PASSWORD_RESET, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID, qUpdateHashReset.ReadColumn("usrs_fio"));
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
		return ErrorQuery(LANG("Carat.Error.Query.UserSettingsReset.Update"), qUpdate);
	}

	//Обходим результат
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
bool ISTcpWorker::UserDeviceAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage),
		Hash = CheckNullField("Hash", TcpMessage);
	if (!UserID.isValid() || !Hash.isValid())
	{
		return false;
	}

	//Проверяем, не привязано ли уже устройство
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_DEVICE);
	qSelect.BindValue(":UserID", UserID);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.UserDeviceAdd.Select"), qSelect);
	}

	if (!qSelect.First()) //Какие-то проблемы
	{
		ErrorString = qSelect.GetErrorString();
		return false;
	}

	//Устройство уже привязано - ошибка
	if (qSelect.ReadColumn("count").toInt() > 0)
	{
		ErrorString = LANG("Carat.Error.Query.UserDeviceAdd.AlreadyExist");
		return false;
	}

	//Генерируем соль
	QString Salt;
	if (!ISAlgorithm::GenerateSalt(Salt, ErrorString))
	{
		ErrorString = LANG("Carat.Error.Query.UserDeviceAdd.GenerateSalt");
		return false;
	}

	//Солим хэш (тем же алгоритмом, что и пароль пользователя). Затем добавляем устройство в БД
	QString HashResult = ISAlgorithm::SaltPassword(Hash.toString(), Salt);
	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_USER_DEVICE);
	qInsert.BindValue(":UserID", UserID);
	qInsert.BindValue(":Hash", HashResult);
	qInsert.BindValue(":Salt", Salt);
	if (!qInsert.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.UserDeviceAdd.Insert"), qInsert);
	}
	QVariant UserFIO = qInsert.First() ? qInsert.ReadColumn("usrs_fio") : QVariant();
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_DEVICE_ADD, QVariant(), QVariant(), QVariant(), UserFIO);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserDeviceDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant UserID = CheckNullField("UserID", TcpMessage);
	if (!UserID.isValid())
	{
		return false;
	}

	//Проверяем наличие устройства
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_DEVICE);
	qSelect.BindValue(":UserID", UserID);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.UserDeviceDelete.Select"), qSelect);
	}

	if (!qSelect.First()) //Какие-то проблемы
	{
		ErrorString = qSelect.GetErrorString();
		return false;
	}

	//Устройство не привязано - ошибка
	if (qSelect.ReadColumn("count").toInt() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.UserDeviceDelete.NotExist");
		return false;
	}

	//Удаляем устройство
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), QD_USER_DEVICE);
	qDelete.BindValue(":UserID", UserID);
	if (!qDelete.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.UserDeviceDelete.Delete"), qDelete);
	}
	QVariant UserFIO = qDelete.First() ? qDelete.ReadColumn("usrs_fio") : QVariant();
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_DEVICE_DELETE, QVariant(), QVariant(), QVariant(), UserFIO);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetRecordCall.SelectUniqueID"), qSelectRecord);
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

	//Запрашиваем список пользователей
	QVariantList VariantList, VariantListOffline;
	ISQuery qSelectClients(ISDatabase::Instance().GetDB(DBConnectionName), QS_CLIENTS);
	if (!qSelectClients.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetClients.Select"), qSelectClients);
	}

	//Получаем список идентификаторов клиентов и обходим результаты запроса
	ISVectorUInt VectorClientsID = ISTcpClients::Instance().GetClientsID();
	while (qSelectClients.Next())
	{
		//Получаем идентификатор текущего пользователя и проверяем, онлайн ли он
		//ищем информацию по клиенту
		unsigned int UserID = qSelectClients.ReadColumn("usrs_id").toUInt();
		bool IsOnline = ISAlgorithm::VectorContains(VectorClientsID, UserID);
		ISClientInfo ClientInfo = IsOnline ? ISTcpClients::Instance().GetClient(UserID) : ISClientInfo();
		
		//Если клиент в сети - посчитаем, сколько он находится в сети
		unsigned int Days = 0, Hours = 0, Minutes = 0, Seconds = 0;
		if (IsOnline)
		{
			ISAlgorithm::ConvertSecondToTime(ClientInfo.DTConnected.secsTo(QDateTime::currentDateTime()),
				Days, Hours, Minutes, Seconds);
		}

		//Если клиент в сети - добавляем в один список, иначе - в другой
		(IsOnline ? VariantList : VariantListOffline).push_back(QVariantMap
		{
			{ "IsOnline", IsOnline },
			{ "Address", IsOnline ? ClientInfo.Address : QVariant() },
			{ "Port", IsOnline ? ClientInfo.Port : QVariant() },
			{ "UserID", IsOnline ? ClientInfo.ID : QVariant() },
			{ "DateTimeConnected", IsOnline ? ClientInfo.DTConnected.toString(FORMAT_DATE_TIME_V2) : QVariant() },
			{ "DateTimeLastQuery", IsOnline ? ClientInfo.LastQueryDT.toString(FORMAT_DATE_TIME_V2) : QVariant() },
			{ "WorkingHours", IsOnline ? QVariantMap
				{
					{ "Days", Days },
					{ "Hours", Hours },
					{ "Minutes", Minutes },
					{ "Seconds", Seconds }
				} : QVariant()
			},
			{ "FIO", qSelectClients.ReadColumn("usrs_fio") },
			{ "GroupName", qSelectClients.ReadColumn("usgp_name") },
			{ "Photo", qSelectClients.ReadColumn("usrs_photo").toByteArray().toBase64() }
		});
	}
	VariantList.append(VariantListOffline);
	TcpAnswer->Parameters["Clients"] = VariantList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		IsCopy = CheckNullField("IsCopy", TcpMessage);
	if (!TableName.isValid() || !IsCopy.isValid())
	{
		return false;
	}

	//Получаем указатель на мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Проверяем наличие списка значений
	if (!TcpMessage->Parameters.contains("Values"))
	{
		ErrorString = LANG("Carat.Error.Query.RecordAdd.ValuesNotExist");
		return false;
	}

	//Проверяем список значений на пустоту
	QVariantMap Values = TcpMessage->Parameters["Values"].toMap();
	if (Values.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.RecordAdd.EmptyValues");
		return false;
	}

	//Обходим значения и готовим запрос
	QString InsertText = "INSERT INTO " + MetaTable->Name.toLower() + '(',
		ValuesText = "VALUES(";
	for (const auto &MapItem : Values.toStdMap())
	{
		if (!MetaTable->GetField(MapItem.first)) //Такого поля нет - ошибка
		{
			ErrorString = LANG("Carat.Error.Query.RecordAdd.FieldNotFound");
			return false;
		}
		InsertText += MetaTable->Alias + '_' + MapItem.first.toLower() + ',';
		ValuesText += ':' + MapItem.first + ',';
	}
	InsertText.chop(1);
	ValuesText.chop(1);

	//Создаём объект запроса и заполняем его параметрами
	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName),
		InsertText + ")\n" + ValuesText + ")\nRETURNING " + MetaTable->Alias + "_id");
	qInsert.SetShowLongQuery(false);
	for (const auto &MapItem : Values.toStdMap())
	{
		if (!qInsert.BindValue(':' + MapItem.first, MapItem.second))
		{
			return ErrorQuery(LANG("Carat.Error.Query.RecordAdd.BindValue"), qInsert);
		}
	}

	//Выполняем запрос
	if (!qInsert.Execute())
	{
		switch (qInsert.GetErrorNumber())
		{
		case 23505: return ErrorQuery(LANG("Carat.Error.Query.RecordAdd.Insert.23505"), qInsert);
		default:
			return ErrorQuery(LANG("Carat.Error.Query.RecordAdd.Insert"), qInsert);
		}
	}

	//Переходим на первую запись
	if (!qInsert.First())
	{
		ErrorString = LANG("Carat.Error.Query.RecordAdd.First");
		return false;
	}

	//Вытаскиваем новый идентификатор и получаем имя записи
	QVariant ObjectID = qInsert.ReadColumn(0);
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//Протоколируем и записываем ответ
	Protocol(TcpMessage->TcpSocket->GetUserID(),
		IsCopy.toBool() ? CONST_UID_PROTOCOL_CREATE_COPY_OBJECT: CONST_UID_PROTOCOL_CREATE_OBJECT,
		MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
	TcpAnswer->Parameters["ObjectID"] = ObjectID;
	TcpAnswer->Parameters["ObjectName"] = ObjectName;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//Получаем указатель на мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Проверяем наличие списка значений
	if (!TcpMessage->Parameters.contains("Values"))
	{
		ErrorString = LANG("Carat.Error.Query.RecordEdit.ValuesNotExist");
		return false;
	}

	//Проверяем список значений на пустоту
	QVariantMap Values = TcpMessage->Parameters["Values"].toMap();
	if (Values.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.RecordEdit.EmptyValues");
		return false;
	}

	//Обходим значения и готовим запрос
	QString SqlText = "UPDATE " + MetaTable->Name.toLower() + " SET\n";
	for (const auto &MapItem : Values.toStdMap())
	{
		if (!MetaTable->GetField(MapItem.first)) //Такого поля нет - ошибка
		{
			ErrorString = LANG("Carat.Error.Query.RecordEdit.FieldNotFound");
			return false;
		}
		SqlText += MetaTable->Alias + '_' + MapItem.first.toLower() + " = :" + MapItem.first + ",\n";
	}
	SqlText.chop(2);
	SqlText += "\nWHERE " + MetaTable->Alias + "_id = :ObjectID";

	//Создаём объекта запроса и заполняем его параметрами
	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qUpdate.BindValue(":ObjectID", ObjectID);
	for (const auto &MapItem : Values.toStdMap())
	{
		if (!qUpdate.BindValue(':' + MapItem.first, MapItem.second))
		{
			return ErrorQuery(LANG("Carat.Error.Query.RecordEdit.BindValue"), qUpdate);
		}
	}

	//Выполняем запрос
	if (!qUpdate.Execute())
	{
		switch (qUpdate.GetErrorNumber())
		{
		case 23505: return ErrorQuery(LANG("Carat.Error.Query.RecordEdit.Update.23505"), qUpdate);
		default:
			return ErrorQuery(LANG("Carat.Error.Query.RecordEdit.Update"), qUpdate);
		}
	}

	//Получаем имя объекта
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//Протоколируем и записываем ответ
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_EDIT_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
	TcpAnswer->Parameters["ObjectName"] = ObjectName;
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

	//Получаем имена удаляемых записей для протокола
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

	//Проверяем, нет ли системных записей
	ISQuery qSqlQuery(ISDatabase::Instance().GetDB(DBConnectionName), "SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_issystem AND " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	qSqlQuery.SetShowLongQuery(false);
	if (!qSqlQuery.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordDelete.Select"), qSqlQuery);
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

	//Удаляем запись
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), "DELETE FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	qDelete.SetShowLongQuery(false);
	if (!qDelete.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordDelete.Delete"), qDelete);
	}

	//Удаляем записи из избранного
	ISQuery qDeleteFavorite(ISDatabase::Instance().GetDB(DBConnectionName), QD_FAVORITE);
	for (const QVariant &ID : Objects)
	{
		qDeleteFavorite.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
		qDeleteFavorite.BindValue(":TableName", MetaTable->Name);
		qDeleteFavorite.BindValue(":ObjectID", ID);
		if (!qDeleteFavorite.Execute())
		{
			ISLOGGER_W(__CLASS__, qDeleteFavorite.GetErrorString());
		}
	}

	//Протоколируем
	unsigned int UserID = TcpMessage->TcpSocket->GetUserID();
	for (const QVariant &ObjectID : Objects)
	{
		Protocol(UserID, CONST_UID_PROTOCOL_DELETE_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectNameMap[ObjectID.toString()]);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//Получаем мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Формируем SQL-Запрос
	QString SqlText = "SELECT\n";
	for (PMetaField *MetaField : MetaTable->Fields) //Обходим поля мета-таблицы
	{
		//Если поле является идентификатором или оно скрыто на форме объекта - пропускаем
		if (MetaField->IsFieldID() || MetaField->HideFromObject)
		{
			continue;
		}

		if (MetaField->Foreign) //Если поле ссылается на справочник
		{
			//Получаем мета-таблицу, на которую ссылается внешний ключ
			PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass);

			//Формируем подзапрос
			SqlText += "(SELECT " + MetaTableForeign->Alias + "_id || ',' || concat(";
			QStringList StringList = MetaField->Foreign->ForeignViewNameField.split(';');
			for (const QString &FieldName : StringList)
			{
				SqlText += MetaTableForeign->Alias + '_' + FieldName.toLower() + ',';
			}
			SqlText.chop(1);
			SqlText += ") FROM " + MetaTableForeign->Name.toLower() +
				" WHERE " + MetaTableForeign->Alias + "_id = " + MetaTable->Alias + '_' + MetaField->Name.toLower() + ") AS \"" + MetaField->Name + "\",\n";
		}
		else //Поле стандартное
		{
			if (MetaField->QueryText.isEmpty()) //Поле не является виртуальным
			{
				SqlText += MetaTable->Alias + '_' + MetaField->Name.toLower();
			}
			else //Поле является виртуальным
			{
				SqlText += '(' + MetaField->QueryText + ')';
			}
			SqlText += " AS \"" + MetaField->Name + "\",\n"; //Дополняем именование поля в выборке
		}
	}
	SqlText.chop(2);
	SqlText += "\nFROM " + MetaTable->Name.toLower() + "\n";
	SqlText += "WHERE " + MetaTable->Alias + "_id = :ID";

	//Выполняем запрос
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qSelect.BindValue(":ID", ObjectID);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordGet.Select"), qSelect);
	}

	//Если переход на первую строку не удался - такая запись не существует
	if (!qSelect.First())
	{
		ErrorString = LANG("Carat.Error.Query.RecordGet.NotExist").arg(ObjectID.toUInt());
		return false;
	}

	//Получаем запись и проверяем, не системная ли она
	QSqlRecord SqlRecord = qSelect.GetRecord();
	if (SqlRecord.value("IsSystem").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.RecordGet.IsSystem").arg(ObjectID.toUInt());
		return false;
	}

	//Обходим поля записи
	QVariantMap Values;
	for (int i = 0, c = SqlRecord.count(); i < c; ++i)
	{
		QSqlField SqlField = SqlRecord.field(i);
		QVariant Value = SqlField.value();

		PMetaField *MetaField = MetaTable->GetField(SqlField.name());
		if (MetaField->Type == ISNamespace::FieldType::Image) //Если поле является изображением, приводим его к base64
		{
			Value = Value.toByteArray().toBase64();
		}
		Values[MetaField->Name] = Value.isNull() ? QVariant() : Value;
	}

	//Получаем имя объекта
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}
	TcpAnswer->Parameters["Values"] = Values;
	TcpAnswer->Parameters["ObjectName"] = ObjectName;
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_SHOW_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordGetInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//Получаем мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_RECORD_INFO);
	qSelect.BindValue(":TableName", TableName);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (!qSelect.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordGetInfo.Select"), qSelect);
	}

	if (!qSelect.First())
	{
		ErrorString = LANG("Carat.Error.Query.RecordGetInfo.NotExist");
		return false;
	}

	//Получаем имя объекта
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	QVariant CreateDate = qSelect.ReadColumn("create_date"),
		CreateUser = qSelect.ReadColumn("create_user"),
		EditUser = qSelect.ReadColumn("edit_user"),
		EditDate = qSelect.ReadColumn("edit_date");

	TcpAnswer->Parameters["TableName"] = MetaTable->LocalListName;
	TcpAnswer->Parameters["ObjectName"] = ObjectName;
	TcpAnswer->Parameters["CreateDate"] = CreateDate.isNull() ? LANG("Carat.NoData") : ISTcpWorkerHelper::ConvertDateTimeToString(CreateDate.toDateTime(), FORMAT_TIME_V3);
	TcpAnswer->Parameters["CreateUser"] = CreateUser.isNull() ? LANG("Carat.NoData") : CreateUser;
	TcpAnswer->Parameters["EditDate"] = EditDate.isNull() ? LANG("Carat.NoData") : ISTcpWorkerHelper::ConvertDateTimeToString(EditDate.toDateTime(), FORMAT_TIME_V3);
	TcpAnswer->Parameters["EditUser"] = EditUser.isNull() ? LANG("Carat.NoData") : EditUser;
	TcpAnswer->Parameters["CopyCount"] = qSelect.ReadColumn("copy_count");
	TcpAnswer->Parameters["EditCount"] = qSelect.ReadColumn("edit_count");
	TcpAnswer->Parameters["ShowCount"] = qSelect.ReadColumn("show_count");
	TcpAnswer->Parameters["FavoriteCount"] = qSelect.ReadColumn("favorite_count");
	TcpAnswer->Parameters["ProtocolCount"] = qSelect.ReadColumn("protocol_count");
	TcpAnswer->Parameters["DiscussionCount"] = qSelect.ReadColumn("discussion_count");
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_RECORD_INFO, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
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
		return ErrorQuery(LANG("Carat.Error.Query.DiscussionAdd.Insert"), qInsert);
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
		return ErrorQuery(LANG("Carat.Error.Query.DiscussionEdit.Update"), qUpdate);
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
		return ErrorQuery(LANG("Carat.Error.Query.DiscussionCopy.Insert"), qCopy);
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
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}
	unsigned int UserID = TcpMessage->TcpSocket->GetUserID();

	//Проверяем наличие права на просмотр данных
	ISQuery qSelectRight(ISDatabase::Instance().GetDB(DBConnectionName), QS_RIGHT_SHOW_TABLE);
	qSelectRight.BindValue(":UserID", UserID);
	qSelectRight.BindValue(":TableName", MetaTable->Name);
	qSelectRight.BindValue(":AccessTypeUID", CONST_UID_GROUP_ACCESS_TYPE_SHOW);
	if (!qSelectRight.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetTableData.SelectRightShow"), qSelectRight);
	}
	if (!qSelectRight.First())
	{
		ErrorString = qSelectRight.GetErrorString();
		return false;
	}

	//Прав доступа на просмотр данных нет - ошибка
	if (!qSelectRight.ReadColumn(0).toBool())
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.NoRightShow").arg(MetaTable->LocalListName);
		return false;
	}

	//Получаем сортировку для этой таблицы
	QString SortingField;
	Qt::SortOrder SortingOrder;
	ISQuery qSelectSorting(ISDatabase::Instance().GetDB(DBConnectionName), QS_SORTING);
	qSelectSorting.BindValue(":UserID", UserID);
	qSelectSorting.BindValue(":TableName", MetaTable->Name);
	if (!qSelectSorting.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetTableData.SelectSorting"), qSelectSorting);
	}

	if (qSelectSorting.First()) //Сортирока есть - получаем её
	{
		SortingField = qSelectSorting.ReadColumn("sgts_fieldname").toString();
		SortingOrder = static_cast<Qt::SortOrder>(qSelectSorting.ReadColumn("sgts_sorting").toUInt());

		//Проверяем не указана ли сортировка в запросе
		//Если указана - проверяем - не нужно ли обновить её в БД
		QVariantMap SortingFieldQuery = TcpMessage->Parameters["Sorting"].toMap();
		if (!SortingFieldQuery.isEmpty())
		{
			//Если новая сортировка отличается от текущей - сохраняем её в БД
			if (SortingField != SortingFieldQuery["Field"].toString() ||
				SortingOrder != static_cast<Qt::SortOrder>(SortingFieldQuery["Order"].toUInt()))
			{
				SortingField = SortingFieldQuery["Field"].toString();
				SortingOrder = static_cast<Qt::SortOrder>(SortingFieldQuery["Order"].toUInt());

				ISQuery qUpdateSorting(ISDatabase::Instance().GetDB(DBConnectionName), QU_SORTING);
				qUpdateSorting.BindValue(":FieldName", SortingField);
				qUpdateSorting.BindValue(":Sorting", SortingOrder);
				qUpdateSorting.BindValue(":UserID", UserID);
				qUpdateSorting.BindValue(":TableName", MetaTable->Name);
				if (!qUpdateSorting.Execute())
				{
					return ErrorQuery(LANG("Carat.Error.Query.GetTableData.UpdateSorting"), qUpdateSorting);
				}
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
			return ErrorQuery(LANG("Carat.Error.Query.GetTableData.InsertSorting"), qInsertSorting);
		}
	}

	//Заполняем служебную информацию
	QVariantMap ServiceInfoMap =
	{
		{ "SortingField", SortingField },
		{ "SortingOrder", SortingOrder }
	};

	//Формируем запрос на выборку
	QString SqlText = "SELECT\n",
		SqlTextJoins;
	size_t Index = 0;
	ISStringMap ForeignFields;

	//Обходим поля мета-таблицы
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		//Если поле скрыто в таблице - пропускаем его
		if (MetaField->HideFromList)
		{
			continue;
		}

		if (MetaField->Foreign) //Если на поле установлен внешний ключ
		{
			PMetaTable *MetaTableFK = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass);
			QString RandomAlias = MetaTableFK->Alias + QString::number(++Index);
			SqlTextJoins += "LEFT JOIN " + MetaTableFK->Name.toLower() + ' ' + RandomAlias + " ON " + MetaTable->Alias + '.' + MetaTable->Alias + '_' + MetaField->Name.toLower() + " = " + RandomAlias + '.' + MetaTableFK->Alias + '_' + MetaField->Foreign->ForeignField.toLower() + "\n";

			QString Temp;
			QStringList StringList = MetaField->Foreign->ForeignViewNameField.split(';');
			Temp += "concat(";
			for (const QString &FieldName : StringList)
			{
				Temp += RandomAlias + '.' + MetaTableFK->Alias + '_' + FieldName.toLower() + ", ' ',";
			}
			Temp.chop(6);
			Temp += ')';
			SqlText += Temp;
			ForeignFields.emplace(MetaField->Name, Temp);
		}
		else //Поле стандартное
		{
			if (MetaField->QueryText.isEmpty()) //Поле не виртуальное
			{
				SqlText += MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + '_' + MetaField->Name.toLower();
			}
			else //Поле является виртуальным
			{
				SqlText += '(' + MetaField->QueryText + ')';
			}
		}
		SqlText += " AS \"" + MetaField->Name + "\",\n";
	}
	SqlText.chop(2);
	SqlTextJoins.chop(1);

	SqlText += "\nFROM " + MetaTable->Name.toLower() + ' ' + MetaTable->Alias + '\n';
	SqlText += SqlTextJoins;

	//Если фильтрация указана - устанавливаем
	QVariantMap FilterMap = TcpMessage->Parameters.contains("Filter") ? TcpMessage->Parameters["Filter"].toMap() : QVariantMap();
	if (!FilterMap.isEmpty())
	{
		SqlText += "\nWHERE ";
		for (const auto &MapItem : FilterMap.toStdMap())
		{
			SqlText += MetaTable->Alias + '_' + MapItem.first + " = :" + MapItem.first + "\nAND ";
		}
		SqlText.chop(5);
	}

	//Если указаны поисковые параметры
	QVariantList SearchList = TcpMessage->Parameters.contains("Search") ? TcpMessage->Parameters["Search"].toList() : QVariantList();
	bool IsSearch = !SearchList.isEmpty();
	if (IsSearch)
	{
		SqlText += "\nWHERE\n";
		for (const QVariant &Variant : SearchList) //Обходим поисковые условия
		{
			QVariantMap Map = Variant.toMap();
			QString FieldName = Map["FieldName"].toString();
			ISNamespace::SearchType OperatorType = static_cast<ISNamespace::SearchType>(Map["Operator"].toUInt());
			QVariantList ValueList = Map["Values"].toList();

			SqlText += MetaTable->Alias + '_' + MetaTable->GetField(FieldName)->Name.toLower();			
			if (ValueList.size() > 1) //Если значений поиска несколько - используем оператор "IN"
			{
				switch (OperatorType)
				{
				case ISNamespace::SearchType::Equally: SqlText += " IN("; break;
				case ISNamespace::SearchType::NotEqually: SqlText += " NOT IN("; break;
				default:
					break;
				}
				for (const QVariant &Value : ValueList) //Обходим список значений
				{
					QString UIDLite = GENERATE_UUID_LITE;
					SqlText += ':' + UIDLite + ',';
					FilterMap[UIDLite] = Value;
				}
				SqlText.chop(1);
				SqlText += ')';
			}
			else //Одно значение для поиска - используем обычный оператор "="
			{
				QString UIDLite = GENERATE_UUID_LITE;
				switch (OperatorType)
				{
				case ISNamespace::SearchType::Equally: SqlText += " = :" + UIDLite; break;
				case ISNamespace::SearchType::NotEqually: SqlText += " != :" + UIDLite; break;
				case ISNamespace::SearchType::Begins: SqlText += " LIKE :" + UIDLite + " || '%'"; break;
				case ISNamespace::SearchType::Ends: SqlText += " LIKE '%' || :" + UIDLite; break;
				case ISNamespace::SearchType::Contains: SqlText += " LIKE '%' || :" + UIDLite + " || '%'"; break;
				default:
					break;
				}
				FilterMap[UIDLite] = ValueList.front();
			}
			SqlText += "\nAND ";
		}
		SqlText.chop(5);
	}

	//Анализируем сортировку
	PMetaField *MetaFieldSorting = MetaTable->GetField(SortingField);
	bool SortingIsVirtual = !MetaFieldSorting->QueryText.isEmpty(),
		SortingIsForeign = MetaFieldSorting->Foreign;
	if (SortingIsVirtual) //Если поле является виртуальным - в качестве сортировки будет выступать запрос
	{
		SortingField = '(' + MetaFieldSorting->QueryText + ')';
	}
	else //Поле не виртуальное: если на поле установлен внешний ключ - используем сформированное имя - иначе просто имя поля
	{
		SortingField = SortingIsForeign ? ForeignFields[MetaFieldSorting->Name] : MetaFieldSorting->Name;
	}

	//Учитываем сортировку и её направление
	SqlText += "\nORDER BY " + (SortingIsVirtual || SortingIsForeign ? SortingField :
		MetaTable->Alias + '.' + MetaTable->Alias + '_' + SortingField) + ' ' +
		(SortingOrder == Qt::AscendingOrder ? "ASC" : "DESC");

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qSelect.SetShowLongQuery(false);

	//заполняем параметры запроса
	for (const auto &MapItem : FilterMap.toStdMap())
	{
		qSelect.BindValue(":" + MapItem.first, MapItem.second);
	}

	if (!qSelect.Execute()) //Запрос не отработал
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetTableData.Select"), qSelect);
	}

	QSqlRecord SqlRecord = qSelect.GetRecord();
	int FieldCount = SqlRecord.count();
	QVariantList FieldList, RecordList;
	std::vector<ISNamespace::FieldType> VectorType(FieldCount, ISNamespace::FieldType::Unknown);
	std::vector<bool> VectorForeign(FieldCount, false);

	//Заполняем описание полей
	for (int i = 0; i < FieldCount; ++i)
	{
		PMetaField *MetaField = MetaTable->GetField(SqlRecord.fieldName(i));
		VectorType[i] = MetaField->Type; //Заполняем типы сейчас, чтобы использовать их ниже
		VectorForeign[i] = MetaField->Foreign ? true : false;
		
		QVariantMap m;
		m["Index"] = i;
		m["Name"] = MetaField->Name;
		m["LocalName"] = MetaField->LocalListName;
		m["Type"] = static_cast<int>(MetaField->Type);
		m["IsForeign"] = MetaField->Foreign ? true : false;
		m["IsSysten"] = MetaField->IsSystem;
		FieldList.push_back(m);
	}

	if (qSelect.First()) //Данные есть
	{
		//Получаем необходимые настройки БД
		unsigned int Precision = ISTcpWorkerHelper::GetSettingDB(DBConnectionName, CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA).toInt();

		do
		{
			QVariantList Values; //Список значений
			SqlRecord = qSelect.GetRecord(); //Получаем очередную запись
			for (int i = 0; i < FieldCount; ++i) //Обходим поля и вытаскиваем значения
			{
				//Получаем тип поля
				ISNamespace::FieldType Type = VectorType[i];

				//Получаем наличие внешнего ключа на поле
				bool IsForeign = VectorForeign[i];

				//Если значение содержит NULL - добавляем пустое и переходим к следующему
				QVariant Value = SqlRecord.value(i);
				if (Value.isNull())
				{
					Values.push_back(QVariant());
					continue;
				}
				
				//Значение не содержит NULL - анализируем
				if (Type == ISNamespace::FieldType::Int || Type == ISNamespace::FieldType::BigInt)
				{
					if (!IsForeign)
					{
						Value = ISAlgorithm::FormatNumber(Value.toLongLong());
					}
				}
				else if (Type == ISNamespace::FieldType::Date)
				{
					Value = ISTcpWorkerHelper::ConvertDateToString(Value.toDate());
				}
				else if (Type == ISNamespace::FieldType::Time)
				{
					Value = Value.toTime().toString(FORMAT_TIME_V3);
				}
				else if (Type == ISNamespace::FieldType::DateTime)
				{
					Value = ISTcpWorkerHelper::ConvertDateTimeToString(Value.toDateTime(), FORMAT_TIME_V1);
				}
				else if (Type == ISNamespace::FieldType::Birthday)
				{
					Value = Value.toDate().toString(FORMAT_DATE_V1);
				}
				else if (Type == ISNamespace::FieldType::Phone)
				{
					QString PhoneNumber = Value.toString();
					Value = QString("+7 (%1) %2-%3-%4").arg(PhoneNumber.left(3)).arg(PhoneNumber.mid(3, 3)).arg(PhoneNumber.mid(6, 2)).arg(PhoneNumber.right(2));
				}
				else if (Type == ISNamespace::FieldType::Seconds)
				{
					Value = QTime(0, 0).addSecs(Value.toInt()).toString(FORMAT_TIME_V3);
				}
				else if (Type == ISNamespace::FieldType::Double)
				{
					Value = ISAlgorithm::FormatNumber(Value.toDouble(), ' ', Precision);
				}
				else if (Type == ISNamespace::FieldType::Money)
				{
					Value = ISAlgorithm::FormatNumber(Value.toDouble(), ' ', 2);
				}
				else if (Type == ISNamespace::FieldType::UID)
				{
					Value = ISUuid(Value);
				}
				else if (Type == ISNamespace::FieldType::ProtocolDT)
				{
					Value = ISTcpWorkerHelper::ConvertDateTimeToString(Value.toDateTime(), FORMAT_TIME_V3);
				}
				Values.push_back(Value);
			}
			RecordList.push_back(Values); //Добавляем список значений в список записей
		} while (qSelect.Next()); //Обходим выборку	
	}

	TcpAnswer->Parameters["ServiceInfo"] = ServiceInfoMap;
	TcpAnswer->Parameters["FieldList"] = FieldList;
	TcpAnswer->Parameters["RecordList"] = RecordList;
	Protocol(TcpMessage->TcpSocket->GetUserID(), IsSearch ? CONST_UID_PROTOCOL_SEARCH : CONST_UID_PROTOCOL_GET_TABLE_DATA, MetaTable->Name, MetaTable->LocalListName);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetTableQuery(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant QueryName = CheckNullField("QueryName", TcpMessage);
	if (!QueryName.isValid())
	{
		return false;
	}

	//Получаем все SQL-запросы из памяти
	QString SqlText;
	QFileInfoList FileInfoList = QDir(":SQL").entryInfoList(QStringList() << "*.sql");
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		QString s = FileInfo.completeBaseName();
		if (s == QueryName) //Нашли запрос - читаем его
		{
			QFile FileSQL(FileInfo.filePath());
			if (!FileSQL.open(QIODevice::ReadOnly)) //Не удалось открыть файл - ошибка
			{
				ErrorString = LANG("Carat.Error.Query.GetTableQuery.FileOpen").arg(FileSQL.errorString());
				return false;
			}

			//Читаем, закрываем и выходим из цикла
			SqlText = FileSQL.readAll();
			FileSQL.close();
			break;
		}
	}

	//Если запрос не найден - ошибка
	if (SqlText.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.GetTableQuery.NotFound");
		return false;
	}

	//Формируем объект запроса, обходим параметры и выполняем
	QVariantMap ParametersMap = TcpMessage->Parameters["Parameters"].toMap();
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	for (const auto &MapItem : ParametersMap.toStdMap())
	{
		qSelect.BindValue(':' + MapItem.first, MapItem.second);
	}
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetTableQuery.Select"), qSelect);
	}

	//Обходим поля
	QStringList FieldList;
	QSqlRecord SqlRecord = qSelect.GetRecord();
	int FieldCount = SqlRecord.count();
	for (int i = 0; i < FieldCount; ++i)
	{
		FieldList.append(SqlRecord.fieldName(i));
	}

	QVariantList RecordList;
	if (qSelect.First()) //Данные есть - обходим записи
	{
		do
		{
			//Получаем очередную строку
			SqlRecord = qSelect.GetRecord();

			//Наполняем список значениями и складываем в список записей
			QStringList Values;
			for (int i = 0; i < FieldCount; ++i)
			{
				Values.append(SqlRecord.value(i).toString());
			}
			RecordList.append(Values);
		} while (qSelect.Next());
	}
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
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Получаем примечание
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_NOTE_RECORD);
	qSelect.BindValue(":TableName", MetaTable->Name);
	qSelect.BindValue(":ObjectID", ObjectID);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetNoteRecord.Select"), qSelect);
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
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
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
			return ErrorQuery(LANG("Carat.Error.Query.SetNoteRecord.Delete"), qDelete);
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
	int Size = ByteArray.size(), MaxSizeMB = ISTcpWorkerHelper::GetSettingDB(DBConnectionName, CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE).toInt();
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
	qInsert.BindValue(":Size", ISAlgorithm::StringFromSize(Size));
	qInsert.BindValue(":Data", ByteArray);
	if (!qInsert.ExecuteFirst())
	{
		return ErrorQuery(LANG("Carat.Error.Query.FileStorageAdd.Insert"), qInsert);
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
		return ErrorQuery(LANG("Carat.Error.Query.FileStorageCopy.Insert"), qInsertCopy);
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
		return ErrorQuery(LANG("Carat.Error.Query.FileStorageGet.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.SearchTaskText.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.SearchTaskID.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.SearchFullText.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetCalendarEvents.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.CalendarDelete.Delete"), qDelete);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetInternalLists.Select"), qSelect);
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
bool ISTcpWorker::SaveMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//Протоколируем в начале, вдруг где-то дальше будет ошибка
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_EXIT_APPLICATION);

	//Готовим запросы для размеров полей
	ISQuery qInsertColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QI_COLUMN_SIZE),
		qUpdateColumnSize(ISDatabase::Instance().GetDB(DBConnectionName), QU_COLUMN_SIZE);

	//Получаем размеры полей и обходим их
	QVariantMap ColumnSize = TcpMessage->Parameters["ColumnSize"].toMap();
	for (const auto &TableItem : ColumnSize.toStdMap())
	{
		for (const auto &FieldItem : TableItem.second.toMap().toStdMap())
		{
			//Пытаем добавить размер поля
			qInsertColumnSize.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qInsertColumnSize.BindValue(":TableName", TableItem.first);
			qInsertColumnSize.BindValue(":FieldName", FieldItem.first);
			qInsertColumnSize.BindValue(":Size", FieldItem.second);
			if (!qInsertColumnSize.Execute()) //Если вставка не удалась - проверяем причину
			{
				if (qInsertColumnSize.GetErrorNumber() == 23505) //Причина - наружение уникальности - обновляем
				{
					qUpdateColumnSize.BindValue(":Size", FieldItem.second);
					qUpdateColumnSize.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
					qUpdateColumnSize.BindValue(":TableName", TableItem.first);
					qUpdateColumnSize.BindValue(":FieldName", FieldItem.first);
					if (!qUpdateColumnSize.Execute()) //Обновить не получилось
					{
						return ErrorQuery(LANG("Carat.Error.Query.SaveMetaData.UpdateColumnSize"), qUpdateColumnSize);
					}
				}
				else //Вставка не удалась по другой причине
				{
					return ErrorQuery(LANG("Carat.Error.Query.SaveMetaData.InsertColumnSize"), qInsertColumnSize);
				}
			}
		}
	}

	//Получаем изменные настройки
	QVariantMap SettingsMap = TcpMessage->Parameters["Settings"].toMap();
	if (!SettingsMap.isEmpty())
	{
		//Готовим запрос для обновления настройки
		ISQuery qUpdateSetting(ISDatabase::Instance().GetDB(DBConnectionName), QU_SETTING);

		for (const auto &MapItem : SettingsMap.toStdMap())
		{
			qUpdateSetting.BindValue(":Value", MapItem.second);
			qUpdateSetting.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
			qUpdateSetting.BindValue(":SettingUID", MapItem.first);
			if (!qUpdateSetting.Execute())
			{
				return ErrorQuery(LANG("Carat.Error.Query.SaveMetaData.UpdateSetting"), qUpdateSetting);
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

	//Получаем системы и подсистемы
	QVariantList SystemsList;
	ISQuery qSelectSystems(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SYSTEM),
		qSelectSubSystems(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SUBSYSTEM);
	if (qSelectSystems.Execute()) //Запрашиваем список систем
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
				return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSubSystems"), qSelectSubSystems);
			}
			SystemsList.append(SystemMap);
		}
	}
	else //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSystems"), qSelectSystems);
	}

	//Получаем типы прав доступа на таблицы
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
		return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.RightTableType"), qSelectAccessTablesType);
	}

	//Обходим таблицы
	QVariantList TablesList;
	QVariantMap TablesMap; //Этот контейнер нужен для сортировки списка таблиц по алфавиту
	ISQuery qSelectTables(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_TABLE);
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		//Если таблица является системной - идём дальше
		if (MetaTable->IsSystem)
		{
			continue;
		}

		QVariantMap TableMap =
		{
			{ "TableName", MetaTable->Name },
			{ "Rights", QVariantList() }
		};
		qSelectTables.BindValue(":GroupID", GroupID);
		qSelectTables.BindValue(":TableName", MetaTable->Name);
		if (qSelectTables.Execute()) //Запрашиваем права на текущую таблицу
		{
			while (qSelectTables.Next()) //Обходим права на текущую таблицу
			{
				QVariantList RightList = TableMap["Rights"].toList();
				RightList.append(ISUuid(qSelectTables.ReadColumn("gatt_uid")));
				TableMap["Rights"] = RightList;
			}
		}
		else //Ошибка запроса
		{
			return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectTables"), qSelectTables);
		}
		TablesMap[MetaTable->LocalListName] = TableMap;
	}

	for (const auto &MapItem : TablesMap.toStdMap())
	{
		QVariantMap TempMap = MapItem.second.toMap();
		TempMap["LocalName"] = MapItem.first;
		TablesList.append(TempMap);
	}

	//Получаем спец. права
	QVariantList SpecialList;
	ISQuery qSelectSpecialParent(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SPECIAL_PARENT),
		qSelectSpecial(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_SPECIAL);
	if (qSelectSpecialParent.Execute()) //Запрашиваем группы спец. прав
	{
		while (qSelectSpecialParent.Next()) //Обходим группы спец. прав
		{
			QVariantMap SpecialGroupMap = { { "LocalName", qSelectSpecialParent.ReadColumn("gast_name") } };

			qSelectSpecial.BindValue(":GroupID", GroupID);
			qSelectSpecial.BindValue(":ParentUID", qSelectSpecialParent.ReadColumn("gast_uid"));
			if (qSelectSpecial.Execute()) //Запрашиваем спец. права
			{
				while (qSelectSpecial.Next()) //Обходим спец. права
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
			else //Ошибка запроса к спец. правам
			{
				return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSpecial"), qSelectSpecial);
			}
			SpecialList.append(SpecialGroupMap);
		}
	}
	else //Ошибка запроса к группам спец. прав
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSpecialGroup"), qSelectSpecialParent);
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

	//Добавляем право
	ISQuery qInsertSubSystemRight(ISDatabase::Instance().GetDB(DBConnectionName), QI_GROUP_RIGHT_SUBSYSTEM);
	qInsertSubSystemRight.BindValue(":GroupID", GroupID);
	qInsertSubSystemRight.BindValue(":SubSystemUID", SubSystemUID);
	if (!qInsertSubSystemRight.Execute()) //Ошибка запроса
	{
		ErrorString = qInsertSubSystemRight.GetErrorNumber() == 23505 ?
			LANG("Carat.Error.Query.GroupRightSubSystemAdd.AlreadyExist") :
			LANG("Carat.Error.Query.GroupRightSubSystemAdd.Insert").arg(qInsertSubSystemRight.GetErrorString());
		return false;
	}

	//Не удалось прочитать имя подсистемы
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

	//Удаляем право
	ISQuery qDeleteSubSystemRight(ISDatabase::Instance().GetDB(DBConnectionName), QD_GROUP_RIGHT_SUBSYSTEM);
	qDeleteSubSystemRight.BindValue(":GroupID", GroupID);
	qDeleteSubSystemRight.BindValue(":SubSystemUID", SubSystemUID);
	if (!qDeleteSubSystemRight.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GroupRightSubSystemDelete.Delete"), qDeleteSubSystemRight);
	}

	//Если ни одна строка не была затронута, значит такого права нет - ошибка
	if (qDeleteSubSystemRight.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightSubSystemDelete.NotExist");
		return false;
	}

	if (!qDeleteSubSystemRight.First()) //Не удалось переместиться на первую строку
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

	//Получаем мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Добавляем право
	ISQuery qInsertTableRight(ISDatabase::Instance().GetDB(DBConnectionName), QI_GROUP_RIGHT_TABLE);
	qInsertTableRight.BindValue(":GroupID", GroupID);
	qInsertTableRight.BindValue(":TableName", MetaTable->Name);
	qInsertTableRight.BindValue(":AccessUID", AccessUID);
	if (!qInsertTableRight.Execute()) //Вставка не удалась
	{
		//Определяем тип ошибки
		ErrorString = qInsertTableRight.GetErrorNumber() == 23505 ?
			LANG("Carat.Error.Query.GroupRightTableAdd.AlreadyExist") :
			LANG("Carat.Error.Query.GroupRightTableAdd.Insert").arg(qInsertTableRight.GetErrorString());
		return false;
	}

	//Не удалось прочитать
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

	//Получаем мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Удаляем право
	ISQuery qDeleteTableRight(ISDatabase::Instance().GetDB(DBConnectionName), QD_GROUP_RIGHT_TABLE);
	qDeleteTableRight.BindValue(":GroupID", GroupID);
	qDeleteTableRight.BindValue(":TableName", MetaTable->Name);
	qDeleteTableRight.BindValue(":AccessUID", AccessUID);
	if (!qDeleteTableRight.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.GroupRightTableDelete.Delete"), qDeleteTableRight);
	}

	//Если ни одна строка не была затронута, значит такого права нет - ошибка
	if (qDeleteTableRight.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightTableDelete.NotExist");
		return false;
	}

	if (!qDeleteTableRight.First()) //Не удалось переместиться на первую строку
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

	//Добавляем спец. право
	ISQuery qInsertSpecialRight(ISDatabase::Instance().GetDB(DBConnectionName), QI_GROUP_RIGHT_SPECIAL);
	qInsertSpecialRight.BindValue(":GroupID", GroupID);
	qInsertSpecialRight.BindValue(":SpecialRightUID", SpecialRightUID);
	if (!qInsertSpecialRight.Execute()) //Вставка не удалась и ошибка говорит о наружении уникальности
	{
		//Определяем тип ошибки
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

	//Удаляем спец. право
	ISQuery qDeleteSpecialRight(ISDatabase::Instance().GetDB(DBConnectionName), QD_GROUP_RIGHT_SPECIAL);
	qDeleteSpecialRight.BindValue(":GroupID", GroupID);
	qDeleteSpecialRight.BindValue(":SpecialRightUID", SpecialRightUID);
	if (!qDeleteSpecialRight.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GroupRightSpecialDelete.Delete"), qDeleteSpecialRight);
	}

	//Если ни одна строка не была затронута, значит такого права нет - ошибка
	if (qDeleteSpecialRight.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.GroupRightSpecialDelete.NotExist");
		return false;
	}

	if (!qDeleteSpecialRight.First()) //Не удалось переместиться на первую строку
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

	//Получаем мета-таблицу
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
		return ErrorQuery(LANG("Carat.Error.Query.GetRecordValue.Select"), qSelectValue);
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
bool ISTcpWorker::RecordFavoriteAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//Получаем мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Получаем имя записи
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//Добавляем запись в избранное
	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_FAVORITE);
	qInsert.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qInsert.BindValue(":TableName", MetaTable->Name);
	qInsert.BindValue(":ObjectID", ObjectID);
	if (!qInsert.Execute()) //Не удалось добавить - проверяем ошибку
	{
		ErrorString = qInsert.GetErrorNumber() == 23505 ?
			LANG("Carat.Error.Query.RecordFavoriteAdd.AlreadyExist") :
			LANG("Carat.Error.Query.RecordFavoriteAdd.Insert");
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_RECORD_FAVORITE_ADD, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordFavoriteDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant TableName = CheckNullField("TableName", TcpMessage),
		ObjectID = CheckNullField("ObjectID", TcpMessage);
	if (!TableName.isValid() || !ObjectID.isValid())
	{
		return false;
	}

	//Получаем мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Получаем имя записи
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//Удаляем запись
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), QD_FAVORITE);
	qDelete.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qDelete.BindValue(":TableName", MetaTable->Name);
	qDelete.BindValue(":ObjectID", ObjectID);
	if (!qDelete.Execute()) //Не удалось удалить
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordFavoriteDelete.Delete"), qDelete);
	}

	//Если при удалении не была затронута ни одна строка - значит такой записи нет
	if (qDelete.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.RecordFavoriteDelete.NotExist");
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_RECORD_FAVORITE_DELETE, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetFavoriteNames(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Запрашиваем избранные записи
	ISQuery qSelectObjects(ISDatabase::Instance().GetDB(DBConnectionName));
	if (TcpMessage->Parameters.contains("TableName")) //Если таблица указана - удаляем записи по указанной таблице
	{
		//Проверяем наличие такой таблицы в мета-данных
		QString TableName = TcpMessage->Parameters["TableName"].toString();
		if (!GetMetaTable(TableName))
		{
			return false;
		}
		qSelectObjects.Prepare(QS_FAVORITE_NAMES_TABLE);
		qSelectObjects.BindValue(":TableName", TableName);
	}
	else //Таблица не указана - удаляем все записи
	{
		qSelectObjects.Prepare(QS_FAVORITE_NAMES_ALL);
	}
	qSelectObjects.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qSelectObjects.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetFavoriteNames.Select"), qSelectObjects);
	}

	PMetaTable *MetaTable = nullptr;
	QString ObjectName;

	//Обходим
	QVariantList NamesList;
	while (qSelectObjects.Next())
	{
		QString TableName = qSelectObjects.ReadColumn("fvts_tablename").toString();
		if (!MetaTable || MetaTable->Name != TableName) //Если мета-таблица ещё не получена или её имя сменилось - получаем её
		{
			MetaTable = GetMetaTable(TableName);
			if (!MetaTable) //Не удалось получить мета-таблицу - выходим с ошибкой
			{
				return false;
			}
		}
	
		//Получаем имя записи
		unsigned int ObjectID = qSelectObjects.ReadColumn("fvts_objectid").toUInt();
		if (!GetObjectName(MetaTable, ObjectID, ObjectName))
		{
			return false;
		}

		//Добавляем в результирующий список
		NamesList.append(QVariantMap
		{
			{ "TableName", MetaTable->Name },
			{ "TableLocalName", MetaTable->LocalListName },
			{ "ObjectID", ObjectID },
			{ "ObjectName", ObjectName },
		});
	}
	TcpAnswer->Parameters["Names"] = NamesList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FavoritesDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	//Удаляем избранные записи
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName));
	if (TcpMessage->Parameters.contains("TableName")) //Если таблица указана - удаляем записи по указанной таблице
	{
		//Проверяем наличие такой таблицы в мета-данных
		QString TableName = TcpMessage->Parameters["TableName"].toString();
		if (!GetMetaTable(TableName))
		{
			return false;
		}
		qDelete.Prepare(QD_FAVORITES_TABLE);
		qDelete.BindValue(":TableName", TableName);
	}
	else //Таблица не указана - удаляем все записи
	{
		qDelete.Prepare(QD_FAVORITES_ALL);
	}
	qDelete.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qDelete.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.FavoritesDelete.Delete"), qDelete);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::LogGetStructure(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Формируем объект папки с логами и получаем список папок-годов
	QDir DirYear(QCoreApplication::applicationDirPath() + "/Logs");
	QStringList YearDirList = DirYear.entryList(QDir::AllDirs, QDir::Name);

	//Обходим папки с годами
	bool Ok = false;
	QVariantMap VariantYearMap;
	for (const QString &YearDirName : YearDirList)
	{
		(void)YearDirName.toUInt(&Ok);
		if (!Ok) //Имя папки невалидное - идём дальше
		{
			continue;
		}

		//Формируем объект папки с месяцами
		QVariantMap VariantMonthMap;
		QDir DirMonth(DirYear.path() + "/" + YearDirName);
		QStringList MonthDirList = DirMonth.entryList(QDir::AllDirs, QDir::Name);
		for (const QString &MonthDirName : MonthDirList)
		{
			(void)MonthDirName.toUInt(&Ok);
			if (!Ok) //Имя месяца невалидное
			{
				continue;
			}

			//Формируем объект папки с днями
			QVariantList VariantDaysList;
			QDir DirDays(DirMonth.path() + "/" + MonthDirName);
			QStringList DayFileList = DirDays.entryList(QDir::Files, QDir::Name);
			for (QString &FileName : DayFileList)
			{
				//Получаем название файла
				int Pos = FileName.indexOf('_');
				if (Pos == -1 || FileName.left(Pos) != QCoreApplication::applicationName()) //Имя файла невалидное
				{
					continue;
				}
				FileName.remove(0, ++Pos); //Убираем имя приложения из имени файла
				std::reverse(FileName.begin(), FileName.end()); //Реверсируем строку
				FileName.remove(0, FileName.indexOf('.') + 1); //Ищем и удаляем расширение в имени файла
				std::reverse(FileName.begin(), FileName.end()); //Реверсируем обратно
				VariantDaysList.append(FileName);
			}
			VariantMonthMap[MonthDirName] = VariantDaysList;
		}
		VariantYearMap[YearDirName] = VariantMonthMap;
	}
	TcpAnswer->Parameters["Years"] = VariantYearMap;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::LogGetContent(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant Year = CheckNullField("Year", TcpMessage),
		Month = CheckNullField("Month", TcpMessage),
		Day = CheckNullField("Day", TcpMessage);
	if (!Year.isValid() || !Month.isValid() || !Day.isValid())
	{
		return false;
	}

	//Формируем путь к файлу и проверяем его наличие
	QFile File(QString("%1/Logs/%2/%3/%4_%5.%3.%2.log").arg(QCoreApplication::applicationDirPath())
		.arg(Year.toString())
		.arg(Month.toUInt() < 10 ? "0" + Month.toString() : Month.toString())
		.arg(QCoreApplication::applicationName())
		.arg(Day.toUInt() < 10 ? "0" + Day.toString() : Day.toString()));
	if (!File.exists()) //Файл не существует
	{
		ErrorString = LANG("Carat.Error.Query.LogGetContent.NotExist");
		return false;
	}

	if (!File.open(QIODevice::ReadOnly)) //Файл не открывается
	{
		ISLOGGER_E(__CLASS__, QString("Error open file \"%1\": %2").arg(File.fileName()).arg(File.errorString()));
		ErrorString = LANG("Carat.Error.Query.LogGetContent.NotOpen");
		return false;
	}

	//Читаем и закрываем файл
	QString Content = File.readAll();
	File.close();
	TcpAnswer->Parameters["Content"] = Content;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CalendarClose(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant CalendarID = CheckNullField("CalendarID", TcpMessage);
	if (!CalendarID.isValid())
	{
		return false;
	}

	//Закрываем событие
	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), QU_CALENDAR_CLOSE);
	qUpdate.BindValue(":CalendarID", CalendarID);
	if (!qUpdate.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.CalendarClose.Update"), qUpdate);
	}

	//Ни одна запись не была затрону - значит её и нет - ошибка
	if (qUpdate.GetCountAffected() == 0)
	{
		ErrorString = LANG("Carat.Error.Query.CalendarClose.NotExist");
		return false;
	}

	if (!qUpdate.First())
	{
		ErrorString = qUpdate.GetErrorString();
		return false;
	}
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_CALENDAR_CLOSE, QVariant(), QVariant(), QVariant(), qUpdate.ReadColumn("cldr_name"));
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetHistoryList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//Получаем данные
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_HISTORY);
	qSelect.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qSelect.Execute()) //Ошибка запроса
	{
		ErrorString = LANG("Carat.Error.Query.GetHistoryList.Select");
		return false;
	}

	PMetaTable *MetaTable = nullptr;
	QString ObjectName;

	//Обходим выборку
	QVariantList HistoryList;
	while (qSelect.Next())
	{
		QString TableName = qSelect.ReadColumn("prtc_tablename").toString();
		if (!MetaTable || MetaTable->Name != TableName) //Если мета-таблица ещё не получена или её имя сменилось - получаем её
		{
			MetaTable = GetMetaTable(TableName);
			if (!MetaTable) //Не удалось получить мета-таблицу - выходим с ошибкой
			{
				return false;
			}
		}
		
		//Получаем имя записи
		unsigned int ObjectID = qSelect.ReadColumn("prtc_objectid").toUInt();
		if (!GetObjectName(MetaTable, ObjectID, ObjectName))
		{
			return false;
		}

		//Добавляем в результирующий список
		HistoryList.append(QVariantMap
		{
			{ "DateTime", ISTcpWorkerHelper::ConvertDateTimeToString(qSelect.ReadColumn("prtc_datetime").toDateTime(), FORMAT_TIME_V3) },
			{ "TableLocalName", MetaTable->LocalListName },
			{ "TableName", MetaTable->Name },
			{ "ObjectID", ObjectID },
			{ "ObjectName", ObjectName }
		});
	}
	TcpAnswer->Parameters["History"] = HistoryList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::TaskCommentAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant TaskID = CheckNullField("TaskID", TcpMessage),
		Comment = CheckNullField("Comment", TcpMessage);
	if (!TaskID.isValid() || !Comment.isValid())
	{
		return false;
	}

	//Проверяем комментарий на пустоту
	if (Comment.toString().isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.TaskCommentAdd.CommentIsEmpty");
		return false;
	}

	//Идентификатор родительского комментария
	QVariant ParentCommentID = TcpMessage->Parameters.contains("ParentCommentID") ?
		TcpMessage->Parameters["ParentCommentID"] : QVariant();

	//Добавляем комментарий
	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_TASK_COMMENT);
	qInsert.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qInsert.BindValue(":TaskID", TaskID);
	qInsert.BindValue(":ParentCommentID", ParentCommentID);
	qInsert.BindValue(":Comment", Comment);
	if (!qInsert.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.TaskCommentAdd.Insert"), qInsert);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetForeignList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant TableName = CheckNullField("TableName", TcpMessage),
		FieldName = CheckNullField("FieldName", TcpMessage);
	if (!TableName.isValid() || !FieldName.isValid())
	{
		return false;
	}
	
	//Получаем мета-таблицу
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//Получаем мета-поле
	PMetaField *MetaField = MetaTable->GetField(FieldName.toString());
	if (!MetaField)
	{
		ErrorString = LANG("Carat.Error.Query.GetForeignList.FieldNotExist").arg(FieldName.toString()).arg(MetaTable->Name);
		return false;
	}

	//Проверяем наличие внешнего ключа по поле
	if (!MetaField->Foreign)
	{
		ErrorString = LANG("Carat.Error.Query.GetForeignList.FieldNotForeign").arg(FieldName.toString());
		return false;
	}

	//Формируем запрос
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass); //Таблица на которую ссылается внешний ключ
	QStringList FieldList = MetaField->Foreign->ForeignViewNameField.split(';');

	QString SqlText = "SELECT " + MetaTableForeign->Alias + '_' + MetaField->Foreign->ForeignField.toLower() + " AS ID, concat(";
	for (const QString &String : FieldList) //Обход полей (которые должны быть отображены)
	{
		SqlText += MetaTableForeign->Alias + '_' + String.toLower() + ", ' ',";
	}
	SqlText.chop(6);
	SqlText += ") ";
	SqlText += "AS Value\n";
	SqlText += "FROM " + MetaTableForeign->Name + "\n";
	SqlText += "ORDER BY 2 ASC";

	//Выполняем запрос
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	if (!qSelect.Execute()) //Ошибка запроса
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetForeignList.Select"), qSelect);
	}

	//Обходим результаты запроса
	QVariantList VariantList;
	while (qSelect.Next())
	{
		VariantList.append(QVariantMap
		{
			{ "ID", qSelect.ReadColumn("ID") },
			{ "Value", qSelect.ReadColumn("Value") },
		});
	}

	TcpAnswer->Parameters["List"] = VariantList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetServerInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	//Делаем запрос к БД
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_SERVER_INFO);
	if (!qSelect.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetServerInfo.SelectInfo"), qSelect);
	}

	if (!qSelect.First())
	{
		ErrorString = qSelect.GetErrorString();
		return false;
	}

	QString DatabaseSizeFull = qSelect.ReadColumn("database_size_full").toString();
	QString DatabaseSizeIndexes = qSelect.ReadColumn("database_size_indexes").toString();
	QString DatabaseSizeData = qSelect.ReadColumn("database_size_data").toString();
	QString DatabaseOwner = qSelect.ReadColumn("database_owner").toString();
	QString DatabaseEncoding = qSelect.ReadColumn("database_encoding").toString();
	QString DatabaseUptime = qSelect.ReadColumn("database_uptime").toString();
	unsigned int DatabasePID = qSelect.ReadColumn("database_backend_pid").toUInt();
	QString DatabaseVersion = qSelect.ReadColumn("database_version").toString();
	QString DatabaseClusterPath = qSelect.ReadColumn("database_cluster_path").toString();
	QString DatabaseSizeLogs = ISAlgorithm::StringFromSize(ISAlgorithm::DirSize(DatabaseClusterPath + "/pg_log", QStringList() << "*.log"));
	QString DatabaseSizeXLogs = ISAlgorithm::StringFromSize(ISAlgorithm::DirSize(DatabaseClusterPath + "/pg_xlog"));
	QString DatabaseCountTable = ISAlgorithm::FormatNumber(qSelect.ReadColumn("table_count").toLongLong());
	QString DatabaseCountField = ISAlgorithm::FormatNumber(qSelect.ReadColumn("field_count").toLongLong());
	QString DatabaseCountSequence = ISAlgorithm::FormatNumber(qSelect.ReadColumn("sequence_count").toLongLong());
	QString DatabaseCountForeign = ISAlgorithm::FormatNumber(qSelect.ReadColumn("foreign_count").toLongLong());
	QString DatabaseRowsCount = ISAlgorithm::FormatNumber(qSelect.ReadColumn("rows_count").toLongLong());
	QString DatabaseCountProtocol = ISAlgorithm::FormatNumber(qSelect.ReadColumn("protocol_count").toLongLong());
	QString DatabaseUsersCount = ISAlgorithm::FormatNumber(qSelect.ReadColumn("users_count").toLongLong());

	TcpAnswer->Parameters["Carat"] = QVariantMap
	{
		{ "Version", ISVersionInfo::Instance().Info.Version },
		{ "StartedDateTime", ISTcpWorkerHelper::ConvertDateTimeToString(PROPERTY_GET("Uptime").toDateTime(), FORMAT_TIME_V3) },
		{ "Uptime", ISTcpWorkerHelper::GetUptime() },
		{ "SizeLogs", ISAlgorithm::StringFromSize(ISAlgorithm::DirSize(QCoreApplication::applicationDirPath() + "/Logs", QStringList() << "*.log")) },
        { "CountClients", static_cast<int>(ISTcpClients::Instance().GetCount()) }
	};
	TcpAnswer->Parameters["Database"] = QVariantMap
	{
		{ "SizeFull", DatabaseSizeFull },
		{ "SizeIndexes", DatabaseSizeIndexes },
		{ "SizeData", DatabaseSizeData },
		{ "Owner", DatabaseOwner },
		{ "Encoding", DatabaseEncoding },
		{ "Uptime", DatabaseUptime },
		{ "PID", DatabasePID },
		{ "Version", DatabaseVersion },
		{ "ClusterPath", DatabaseClusterPath },
		{ "SizeLogs", DatabaseSizeLogs },
		{ "SizeXLogs", DatabaseSizeXLogs },
		{ "CountTable", DatabaseCountTable },
		{ "CountField", DatabaseCountField },
		{ "CountSequence", DatabaseCountSequence },
		{ "CountForeign", DatabaseCountForeign },
		{ "RowsCount", DatabaseRowsCount },
		{ "ProtocolCount", DatabaseCountProtocol },
		{ "UsersCount", DatabaseUsersCount }
	};
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_SERVER_INFO);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OrganizationFormINN(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant INN = CheckNullField("INN", TcpMessage);
	if (!INN.isValid())
	{
		return false;
	}

	//Формируем запрос
	QNetworkRequest NetworkRequest;
	NetworkRequest.setUrl(QUrl("https://suggestions.dadata.ru/suggestions/api/4_1/rs/suggest/party"));
	NetworkRequest.setRawHeader("Content-Type", "application/json");
	NetworkRequest.setRawHeader("Authorization", QString("token %1").arg(TOKEN_DA_DATA_TOKEN).toUtf8());

	//Отправляем его
	QNetworkAccessManager NetworkAccessManager;
	QEventLoop EventLoop;
	QNetworkReply *NetworkReply = NetworkAccessManager.post(NetworkRequest, ISSystem::VariantMapToJsonString(QVariantMap
	{
		{ "Content-Type", "appliaction/json" },
		{ "Accept", "appliaction/json" },
		{ "Authorization", QString("Token ") + TOKEN_DA_DATA_TOKEN },
		{ "query", INN }
	}));
	connect(NetworkReply, &QNetworkReply::finished, &EventLoop, &QEventLoop::quit);
	EventLoop.exec(); //Ждём ответа

	//Парсим ответ и указатель на ответ удаляем
	QJsonParseError JsonParseError;
	QVariantMap ReplyMap = ISSystem::JsonStringToVariantMap(NetworkReply->readAll(), JsonParseError);
	NetworkReply->close();
	NetworkReply->deleteLater();

	if (JsonParseError.error != QJsonParseError::NoError) //Ответ невалидный
	{
		ISLOGGER_E(__CLASS__, "Not parse reply from DaData: " + JsonParseError.errorString());
		ErrorString = LANG("Carat.Error.Query.OrganizationFormINN.Parse");
		return false;
	}
	
	QVariantList ReplyList = ReplyMap["suggestions"].toList();
	if(ReplyList.isEmpty()) //Ничего не найдено
	{
		ErrorString = LANG("Carat.Error.Query.OrganizationFormINN.NotFound");
		return false;
	}
	TcpAnswer->Parameters["Reply"] = ReplyList.front().toMap();
	return true;
}
//-----------------------------------------------------------------------------
