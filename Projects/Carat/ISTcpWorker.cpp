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
//-----------------------------------------------------------------------------
static QString QS_USERS_HASH = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
											 "FROM _users "
											 "WHERE usrs_hash IS NOT NULL "
											 "AND usrs_salt IS NOT NULL");
//-----------------------------------------------------------------------------
static QString QS_USER_AUTH = PREPARE_QUERY("SELECT usrs_id, usrs_issystem, usrs_group, usrs_fio, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, usgp_fullaccess, "
											"(SELECT sgdb_useraccessdatabase FROM _settingsdatabase WHERE sgdb_uid = :SettingUID) "
											"FROM _users "
											"LEFT JOIN _usergroup ON usgp_id = usrs_group "
											"WHERE usrs_hash = :Hash");
//-----------------------------------------------------------------------------
static QString QI_PROTOCOL = PREPARE_QUERY("INSERT INTO _protocol(prtc_datetime, prtc_user, prtc_tablename, prtc_tablelocalname, prtc_type, prtc_objectid, prtc_information) "
										   "VALUES(:DateTime, :UserID, :TableName, :TableLocalName, (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = :TypeUID), :ObjectID, :Information)");
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
static QString QS_PERIOD = PREPARE_QUERY("SELECT prod_constant "
										 "FROM period "
										 "WHERE CURRENT_DATE BETWEEN prod_datestart AND prod_dateend");
//-----------------------------------------------------------------------------
static QString QS_STOCK = PREPARE_QUERY("SELECT stck_id, stck_name "
										"FROM stock "
										"ORDER BY stck_name");
//-----------------------------------------------------------------------------
static QString QS_STATEMENT = PREPARE_QUERY("SELECT COUNT(*) "
											"FROM gasstationstatement "
											"WHERE gsts_implementationunload = :ImplementationUnload");
//-----------------------------------------------------------------------------
static QString QI_STATEMENT = PREPARE_QUERY("INSERT INTO gasstationstatement(gsts_implementationunload, gsts_stock, gsts_date, gsts_volumeincome) "
											"VALUES(:ImplementationUnload, :StockID, "
											"(SELECT impl_date FROM implementation WHERE impl_id = (SELECT iunl_implementation FROM implementationunload WHERE iunl_id = :ImplementationUnload)), "
											":VolumeIncome)");
//-----------------------------------------------------------------------------
static QString QS_FILL_IN_BASED = PREPARE_QUERY("SELECT "
												"COALESCE(gsts_balanceendchange, 0) AS gsts_balanceendchange, "
												"COALESCE(gsts_cashboxtotalpayment, 0) AS gsts_cashboxtotalpayment, "
												"COALESCE(gsts_cashboxtotalactually, 0) AS gsts_cashboxtotalactually, "
												"COALESCE(gsts_cashboxkkmtotal, 0) AS gsts_cashboxkkmtotal "
												"FROM gasstationstatement "
												"WHERE gsts_id = :StatementID");
//-----------------------------------------------------------------------------
static QString QS_IMPLEMENTATION_UNLOAD = PREPARE_QUERY("SELECT true AS is_load, ilod_implementation AS implementation_id, ilod_id AS id, impl_date AS date, ilod_cost AS cost "
														"FROM implementationload "
														"LEFT JOIN implementation ON ilod_implementation = impl_id "
														"WHERE ilod_counterparty = :CounterpartyID "
														""
														"UNION "
														""
														"SELECT false AS is_load, iunl_implementation AS implementation_id, iunl_id AS id, impl_date AS date, iunl_cost AS cost "
														"FROM implementationunload "
														"LEFT JOIN implementation ON iunl_implementation = impl_id "
														"WHERE iunl_counterparty = :CounterpartyID "
														"ORDER BY is_load, date DESC");
//-----------------------------------------------------------------------------
static QString QS_COUNTERPARTY_DEBT = PREPARE_QUERY("SELECT get_counterparty_unload(:CounterpartyID), get_counterparty_load(:CounterpartyID), get_counterparty_entrollment(:CounterpartyID), get_counterparty_move_wagon(:CounterpartyID)");
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
			ISLOGGER_I(__CLASS__, "Stopping ...");
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
			unsigned long long PerfomanceMsec = 0;
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
					case ISNamespace::AMT_RecordAdd: Result = RecordAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_RecordEdit: Result = RecordEdit(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_RecordDelete: Result = RecordDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_RecordGet: Result = RecordGet(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_RecordGetInfo: Result = RecordGetInfo(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionAdd: Result = DiscussionAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionEdit: Result = DiscussionEdit(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_DiscussionCopy: Result = DiscussionCopy(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetTableData: Result = GetTableData(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetTableQuery: Result = GetTableQuery(tcp_message, TcpAnswer); break;
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
					case ISNamespace::AMT_RecordFavoriteAdd: Result = RecordFavoriteAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_RecordFavoriteDelete: Result = RecordFavoriteDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetFavoriteNames: Result = GetFavoriteNames(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_FavoritesDelete: Result = FavoritesDelete(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_LogGet: Result = LogGet(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_CalendarClose: Result = CalendarClose(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetHistoryList: Result = GetHistoryList(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_TaskCommentAdd: Result = TaskCommentAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetForeignList: Result = GetForeignList(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_PeriodContains: Result = PeriodContains(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetStockList: Result = GetStockList(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_StatementAdd: Result = StatementAdd(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetGasStation: Result = GetGasStation(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetDebtImplementation: Result = GetDebtImplementation(tcp_message, TcpAnswer); break;
					case ISNamespace::AMT_GetDebtCounterparty: Result = GetDebtCounterparty(tcp_message, TcpAnswer); break;
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
		return ErrorQuery(LANG("Carat.Error.CheckExistUserPassword"), qSelectHashIsNull);
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
QString ISTcpWorker::ConvertDateTimeToString(const QDateTime &DateTime, const QString &TimeFormat)
{
	return DateTime.isValid() ? ConvertDateToString(DateTime.date()) + SYMBOL_SPACE +
		LANG("Carat.In") + SYMBOL_SPACE + DateTime.time().toString(TimeFormat) :
		QString();
}
//-----------------------------------------------------------------------------
QString ISTcpWorker::ConvertDateToString(const QDate &Date)
{
	QString Result;
	QDate CurrentDate = QDate::currentDate();
	if (Date == CurrentDate.addDays(-2)) //���������
	{
		Result = LANG("Carat.BeforeYesterday");
	}
	else if (Date == CurrentDate.addDays(-1)) //�����
	{
		Result = LANG("Carat.Yesterday");
	}
	else if (Date == CurrentDate) //�������
	{
		Result = LANG("Carat.Today");
	}
	else if (Date == CurrentDate.addDays(1)) //������
	{
		Result = LANG("Carat.Tomorrow");
	}
	else if (Date == CurrentDate.addDays(2)) //�����������
	{
		Result = LANG("Carat.AfterTomorrow");
	}
	else
	{
		switch (Date.month())
		{
		case 1: Result = LANG("Carat.Month.January"); break;
		case 2: Result = LANG("Carat.Month.February"); break;
		case 3: Result = LANG("Carat.Month.March"); break;
		case 4: Result = LANG("Carat.Month.April"); break;
		case 5: Result = LANG("Carat.Month.May"); break;
		case 6: Result = LANG("Carat.Month.June"); break;
		case 7: Result = LANG("Carat.Month.July"); break;
		case 8: Result = LANG("Carat.Month.August"); break;
		case 9: Result = LANG("Carat.Month.September"); break;
		case 10: Result = LANG("Carat.Month.October"); break;
		case 11: Result = LANG("Carat.Month.November"); break;
		case 12: Result = LANG("Carat.Month.December"); break;
		default:
			break;
		}
		Result = QString::number(Date.day()) + SYMBOL_SPACE +
			Result + SYMBOL_SPACE +
			QString::number(Date.year());
	}
	return Result;
}
//-----------------------------------------------------------------------------
QString ISTcpWorker::GenerateSqlQueryFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName)
{
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaForeign->ForeignClass);
	QString SqlQuery = "SELECT " + MetaTableForeign->Alias + '_' + MetaForeign->ForeignViewNameField + "\n" +
		"FROM " + MetaTableForeign->Name + "\n" +
		"WHERE " + MetaTableForeign->Alias + "_id = " + Alias + '_' + FieldName;
	return SqlQuery;
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
				("(" + GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "), ' ', ") :
				(MetaTable->Alias + '_' + FieldName + ", ' ', ");
		}
		QueryText.chop(7);
		QueryText += ") \n";
	}
	else //���� ������� ������ ���� ����
	{
		PMetaForeign *MetaForeign = MetaTable->GetField(MetaTable->TitleName)->Foreign;
		QueryText += MetaForeign ?
			("(" + GenerateSqlQueryFromTitleName(MetaForeign, MetaTable->Alias, MetaTable->TitleName) + ") \n") :
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
		return ErrorQuery(LANG("Carat.Error.GetObjectName"), qSelectName);
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
QVariant ISTcpWorker::GetSettingDB(const QString &SettingName)
{
	QVariant Value;
	ISQuery qSelectSettingDB(ISDatabase::Instance().GetDB(DBConnectionName),
		"SELECT sgdb_" + SettingName + " FROM _settingsdatabase WHERE sgdb_uid = :SettingUID");
	qSelectSettingDB.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectSettingDB.Execute()) //������ �������� �������
	{
		if (qSelectSettingDB.First()) //��������� �������
		{
			Value = qSelectSettingDB.ReadColumn(0);
		}
		else //����� ��������� � �� ���
		{
			ISLOGGER_E(__CLASS__, "not found setting database. Use default value...");
		}
	}
	else //������ �������
	{
		ISLOGGER_E(__CLASS__, "not getting setting database: " + qSelectSettingDB.GetErrorString());
	}

	//���� �������� ���������� - ���������� ��������� �� ���������
	if (!Value.isValid() || Value.isNull())
	{
		Value = ISMetaData::Instance().GetMetaTable("_SettingsDatabase")->GetField(SettingName)->DefaultValue;
	}
	return Value;
}
//-----------------------------------------------------------------------------
QString ISTcpWorker::GetColorForLogMessage(const QString &Severity) const
{
	if (Severity == LOGGER_SEVERITY_DEBUG)
	{
		return QColor(Qt::blue).name();
	}
	else if (Severity == LOGGER_SEVERITY_INFO)
	{
		return QColor(Qt::darkGreen).name();
	}
	else if (Severity == LOGGER_SEVERITY_WARNING)
	{
		return QColor(255, 106, 0).name();
	}
	else if (Severity == LOGGER_SEVERITY_ERROR)
	{
		return QColor(Qt::red).name();
	}
	else if (Severity == LOGGER_SEVERITY_CRITICAL)
	{
		return QColor(Qt::red).name();
	}
	else if (Severity == LOGGER_SEVERITY_TRACE)
	{
		return QColor(Qt::darkMagenta).name();
	}
	else if (Severity == LOGGER_SEVERITY_ASSERT)
	{
		return QColor(Qt::darkMagenta).name();
	}
	return QColor(Qt::black).name();
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

	//���������, �� ����������� �� ��� ������. ���� ����������� - ������� � �������
	if (TcpMessage->TcpSocket->GetAuthorized())
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AlreadyAuthorized");
		return false;
	}

	//���� ���� ������� ������ ��� ������� - �� ��� ����� � ���������
	if (ISConfigurations::Instance().Get().DateExpired.isValid() &&
		QDate::currentDate() > ISConfigurations::Instance().Get().DateExpired)
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AuthDenied");
		return false;
	}

	//���������, �� ������������ �� �����
	QString IPAddress = TcpMessage->TcpSocket->GetAddress();
	if (ISFail2Ban::Instance().IsLock(IPAddress))
	{
		ErrorString = LANG("Carat.Error.Query.Auth.AddressIsLocked").arg(ISFail2Ban::Instance().GetUnlockDateTime(IPAddress).toString(FORMAT_DATE_TIME_V2));
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
			return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectHash"), qSelectHash);
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

		if (IsFound) //����� ������������ - ������� ����� �� Fail2Ban
		{
			ISFail2Ban::Instance().Remove(IPAddress);
		}
		else //�� ����� ������������ - ��������� ����� � Fail2Ban
		{
			//���� ����� ������������ - �������� �� ����
			//����� - ������������� � ������������ ����� ������ ��� ������
			ErrorString = ISFail2Ban::Instance().Add(IPAddress)
				? LANG("Carat.Error.Query.Auth.Fail2Ban").arg(CARAT_BAN_ATTEMPT_COUNT).arg(ISFail2Ban::Instance().GetUnlockDateTime(IPAddress).toString(FORMAT_DATE_TIME_V2))
				: LANG("Carat.Error.Query.Auth.InvalidLoginOrPassword");
			return false;
		}
	}

	//�������� ������������
	ISQuery qSelectAuth(ISDatabase::Instance().GetDB(DBConnectionName), QS_USER_AUTH);
	qSelectAuth.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	qSelectAuth.BindValue(":Hash", Hash);
	if (!qSelectAuth.ExecuteFirst()) //������ �������� � �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectLogin"), qSelectAuth);
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

	//��������� ������ �������
	bool IsNeedUpdate = false, VersionIsValid = false; //�� ��������� ���� ���������� ������ ���� false (����� ������ �������� ���������� ������)
	unsigned int VersionLast = 0;
	if (TcpMessage->Parameters.contains("Version")) //���� ������ �������
	{
		//��������� ��� ������ �� ����������
		QVariant Version = TcpMessage->Parameters["Version"];
		unsigned int VersionClient = Version.toUInt(&VersionIsValid);
		if (VersionIsValid) //������ ������� - ���� ��������� ����
		{
			//�������� ���������� � ������������
			QString UpdateClientDir = CONFIG_STRING(CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR);
			if (!UpdateClientDir.isEmpty()) //���� ���������� ��������� - ��� ������
			{
				//��������� ������������� ���������� � ������������
				QDir Dir(UpdateClientDir);
				if (!Dir.exists())
				{
					ISLOGGER_W(__CLASS__, "Folder \"" + UpdateClientDir + "\" not exist. Check config parameter \"" + CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR + "\"");
					ErrorString = LANG("Carat.Error.Query.Auth.UpdatePathNotExist");
					return false;
				}

				//�������� ��������������� (�� ����) ������ ������ � ��������� ��� �� �������
				QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
				if (!StringList.isEmpty()) //���� ���������� ���� - ����������� ������ ���������� �����
				{
					QString FilePath = Dir.absolutePath() + '/' + StringList.front();
					VersionLast = ISAlgorithm::ExtractVersionFile(FilePath);
					if (VersionLast > 0) //������ ����� ����� �������
					{
						IsNeedUpdate = VersionLast > VersionClient;
					}
					else //���������� ������ ����� �����
					{
						ISLOGGER_W(__CLASS__, "Invalid format update file name: " + FilePath);
					}
				}
				//���������� ��� - ��� ������
			}
			//���������� �� ��������� - �������� �������������� � ��� ������
			ISLOGGER_W(__CLASS__, "Not setting directory updates");
		}
		else //������ ���������
		{
			ISLOGGER_W(__CLASS__, "Client version invalid: " + Version.toString());
		}
	}

	//������������� ����� ������
	TcpMessage->TcpSocket->SetAuthorized(true);
	TcpMessage->TcpSocket->SetUserID(UserID);
	TcpMessage->TcpSocket->SetUserIsSystem(IsSystem);

	//����� ���������� � ������������ � ������������
	TcpAnswer->Parameters["UserID"] = UserID;
	TcpAnswer->Parameters["UserIsSystem"] = IsSystem;
	TcpAnswer->Parameters["UserFIO"] = UserFIO;
	TcpAnswer->Parameters["UserGroupID"] = GroupID;
	TcpAnswer->Parameters["UserGroupFullAccess"] = GroupFullAccess;
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
	unsigned int TimeoutInt = Timeout.toUInt(&Ok);
	if (!Ok) //�� ������� �������� �������� � �����
	{
		ErrorString = LANG("Carat.Error.Query.Sleep.TimeoutValueInvalid");
		return false;
	}

	if (TimeoutInt == 0) //���� �������� ����� ���� - ������
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
	//������� ������ ��� ��������� �������� ��
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

	//�������� ��������� ��
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

	//�������� ����� �� �������
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

	//�������� ����������� �����
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
			
			//��������� ������� ������ ���� �� ��� ��������� �����-������ ����������
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

	//�������� ��������� �������
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

	//�������� ������� �����
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

	//�������� ���������
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

	//�������� ���������� �����
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

	//������ ����-������
	QVariantList MetaDataList;
	QStringList Filter("*.xsn"); //������
	QFileInfoList FileInfoList = QDir(":Scheme").entryInfoList(Filter, QDir::NoFilter, QDir::Name); //�������� ����-������ ������
	FileInfoList.append(QDir(":_" + ISConfigurations::Instance().Get().Name).entryInfoList(Filter, QDir::NoFilter, QDir::Name)); //�������� ����-������ ������������
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

	//�������� ��������������� (�� ����) ������ ������
	//� ����������� ���������� � ��������� ��� �� �������
	QDir Dir(CONFIG_STRING(CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR));
	QStringList StringList = Dir.entryList(QDir::Files, QDir::Time);
	if (StringList.isEmpty()) //���� ���������� ��� - �������
	{
		TcpAnswer->Parameters["Found"] = false;
		return true;
	}

	//��������� ����
	QString FilePath = Dir.absolutePath() + '/' + StringList.front();
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
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordCreate.UpdateHash"), qUpdateHash);
	}
	qUpdateHash.First(); //�������� �� ���������, ��� ����������� ����

	//��������� �������� ������
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_PASSWORD_CREATE, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID, qUpdateHash.ReadColumn("usrs_fio"));
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
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.SelectHash"), qSelectHash);
	}

	if (!qSelectHash.First()) //������������ � ����� UserID ��� � ��
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordEdit.UserNotExist").arg(UserID.toInt());
		return false;
	}

	//������� ��� � ����
	QString CurrentHash = qSelectHash.ReadColumn("usrs_hash").toString(),
		CurrentSalt = qSelectHash.ReadColumn("usrs_salt").toString();

	//����� ���������� ��� � ���������. ���� ���������� ��� �� ������������� ���� ��� � �� - ������ ������� ������������ ������� ������ (��� �����) - ������
	if (ISAlgorithm::SaltPassword(HashOld.toString(), CurrentSalt) != CurrentHash)
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordEdit.InvalidCurrentLoginOrPassword");
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
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.UpdateHash"), qUpdateHash);
	}

	//��������� ��������� ������
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

	//��������� ������������ �� �����������
	bool IsSystem = true;
	if (!UserIsSystem(UserID, IsSystem))
	{
		return false;
	}

	if (IsSystem) //������������ ��������� - ���������� � ������ ������
	{
		ErrorString = LANG("Carat.Error.Query.UserPasswordReset.UserIsSystem");
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
		ErrorString = LANG("Carat.Error.Query.UserPasswordReset.PasswordIsNull");
		return false;
	}

	//���������� ������
	ISQuery qUpdateHashReset(ISDatabase::Instance().GetDB(DBConnectionName), QU_USER_HASH_RESET);
	qUpdateHashReset.BindValue(":UserID", UserID);
	if (!qUpdateHashReset.Execute()) //�� ������� �������� ������
	{
		return ErrorQuery(LANG("Carat.Error.Query.UserPasswordReset.Reset"), qUpdateHashReset);
	}

	//��������� ����� ������
	Protocol(TcpMessage->TcpSocket->GetUserID(), CONST_UID_PROTOCOL_USER_PASSWORD_RESET, "_Users", ISMetaData::Instance().GetMetaTable("_Users")->LocalListName, UserID, qUpdateHashReset.ReadColumn("usrs_fio"));
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
		return ErrorQuery(LANG("Carat.Error.Query.UserSettingsReset.Update"), qUpdate);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetRecordCall.SelectUniqueID"), qSelectRecord);
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

	//����������� ������ �������������
	QVariantList VariantList, VariantListOffline;
	ISQuery qSelectClients(ISDatabase::Instance().GetDB(DBConnectionName), QS_CLIENTS);
	if (!qSelectClients.Execute())
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetClients.Select"), qSelectClients);
	}

	//�������� ������ ��������������� �������� � ������� ���������� �������
	ISVectorUInt VectorClientsID = ISTcpClients::Instance().GetClientsID();
	while (qSelectClients.Next())
	{
		//�������� ������������� �������� ������������ � ���������, ������ �� ��
		//���� ���������� �� �������
		unsigned int UserID = qSelectClients.ReadColumn("usrs_id").toUInt();
		bool IsOnline = ISAlgorithm::VectorContains(VectorClientsID, UserID);
		ISClientInfo ClientInfo = IsOnline ? ISTcpClients::Instance().GetClient(UserID) : ISClientInfo();
		
		//���� ������ � ���� - ���������, ������� �� ��������� � ����
		unsigned int Days = 0, Hours = 0, Minutes = 0, Seconds = 0;
		if (IsOnline)
		{
			ISAlgorithm::ConvertSecondToTime(ClientInfo.DTConnected.secsTo(QDateTime::currentDateTime()),
				Days, Hours, Minutes, Seconds);
		}

		//���� ������ � ���� - ��������� � ���� ������, ����� - � ������
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

	//�������� ��������� �� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//��������� ������� ������ ��������
	if (!TcpMessage->Parameters.contains("Values"))
	{
		ErrorString = LANG("Carat.Error.Query.RecordAdd.ValuesNotExist");
		return false;
	}

	//��������� ������ �������� �� �������
	QVariantMap Values = TcpMessage->Parameters["Values"].toMap();
	if (Values.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.RecordAdd.EmptyValues");
		return false;
	}

	//������� �������� � ������� ������
	QString InsertText = "INSERT INTO " + MetaTable->Name.toLower() + '(',
		ValuesText = "VALUES(";
	for (const auto &MapItem : Values.toStdMap())
	{
		if (!MetaTable->GetField(MapItem.first)) //������ ���� ��� - ������
		{
			ErrorString = LANG("Carat.Error.Query.RecordAdd.FieldNotFound");
			return false;
		}
		InsertText += MetaTable->Alias + '_' + MapItem.first.toLower() + ',';
		ValuesText += ':' + MapItem.first + ',';
	}
	InsertText.chop(1);
	ValuesText.chop(1);

	//������ ������ ������� � ��������� ��� �����������
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

	//��������� ������
	if (!qInsert.Execute())
	{
		switch (qInsert.GetErrorNumber())
		{
		case 23505: return ErrorQuery(LANG("Carat.Error.Query.RecordAdd.Insert.23505"), qInsert);
		default:
			return ErrorQuery(LANG("Carat.Error.Query.RecordAdd.Insert"), qInsert);
		}
	}

	//��������� �� ������ ������
	if (!qInsert.First())
	{
		ErrorString = LANG("Carat.Error.Query.RecordAdd.First");
		return false;
	}

	//����������� ����� ������������� � �������� ��� ������
	QVariant ObjectID = qInsert.ReadColumn(0);
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//������������� � ���������� �����
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

	//�������� ��������� �� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//��������� ������� ������ ��������
	if (!TcpMessage->Parameters.contains("Values"))
	{
		ErrorString = LANG("Carat.Error.Query.RecordEdit.ValuesNotExist");
		return false;
	}

	//��������� ������ �������� �� �������
	QVariantMap Values = TcpMessage->Parameters["Values"].toMap();
	if (Values.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.RecordEdit.EmptyValues");
		return false;
	}

	//������� �������� � ������� ������
	QString SqlText = "UPDATE " + MetaTable->Name.toLower() + " SET\n";
	for (const auto &MapItem : Values.toStdMap())
	{
		if (!MetaTable->GetField(MapItem.first)) //������ ���� ��� - ������
		{
			ErrorString = LANG("Carat.Error.Query.RecordEdit.FieldNotFound");
			return false;
		}
		SqlText += MetaTable->Alias + '_' + MapItem.first.toLower() + " = :" + MapItem.first + ",\n";
	}
	SqlText.chop(2);
	SqlText += "\nWHERE " + MetaTable->Alias + "_id = :ObjectID";

	//������ ������� ������� � ��������� ��� �����������
	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qUpdate.BindValue(":ObjectID", ObjectID);
	for (const auto &MapItem : Values.toStdMap())
	{
		if (!qUpdate.BindValue(':' + MapItem.first, MapItem.second))
		{
			return ErrorQuery(LANG("Carat.Error.Query.RecordEdit.BindValue"), qUpdate);
		}
	}

	//��������� ������
	if (!qUpdate.Execute())
	{
		switch (qUpdate.GetErrorNumber())
		{
		case 23505: return ErrorQuery(LANG("Carat.Error.Query.RecordEdit.Update.23505"), qUpdate);
		default:
			return ErrorQuery(LANG("Carat.Error.Query.RecordEdit.Update"), qUpdate);
		}
	}

	//�������� ��� �������
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//������������� � ���������� �����
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

	//�������
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), "DELETE FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
	qDelete.SetShowLongQuery(false);
	if (!qDelete.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordDelete.Delete"), qDelete);
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
bool ISTcpWorker::RecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
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

	//��������� SQL-������
	QString SqlText = "SELECT\n";
	for (PMetaField *MetaField : MetaTable->Fields) //������� ���� ����-�������
	{
		//���� ���� �������� ��������������� ��� ��� ������ �� ����� ������� - ����������
		if (MetaField->IsFieldID() || MetaField->HideFromObject)
		{
			continue;
		}

		if (MetaField->Foreign) //���� ���� ��������� �� ����������
		{
			//�������� ����-�������, �� ������� ��������� ������� ����
			PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass);

			//��������� ���������
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
		else //���� �����������
		{
			if (MetaField->QueryText.isEmpty()) //���� �� �������� �����������
			{
				SqlText += MetaTable->Alias + '_' + MetaField->Name.toLower();
			}
			else //���� �������� �����������
			{
				SqlText += '(' + MetaField->QueryText + ')';
			}
			SqlText += " AS \"" + MetaField->Name + "\",\n"; //��������� ���������� ���� � �������
		}
	}
	SqlText.chop(2);
	SqlText += "\nFROM " + MetaTable->Name.toLower() + "\n";
	SqlText += "WHERE " + MetaTable->Alias + "_id = :ID";

	//��������� ������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qSelect.BindValue(":ID", ObjectID);
	if (!qSelect.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordGet.Select"), qSelect);
	}

	//���� ������� �� ������ ������ �� ������ - ����� ������ �� ����������
	if (!qSelect.First())
	{
		ErrorString = LANG("Carat.Error.Query.RecordGet.NotExist").arg(ObjectID.toUInt());
		return false;
	}

	//�������� ������ � ���������, �� ��������� �� ���
	QSqlRecord SqlRecord = qSelect.GetRecord();
	if (SqlRecord.value("IsSystem").toBool())
	{
		ErrorString = LANG("Carat.Error.Query.RecordGet.IsSystem").arg(ObjectID.toUInt());
		return false;
	}

	//������� ���� ������
	QVariantMap Values;
	for (int i = 0, c = SqlRecord.count(); i < c; ++i)
	{
		QSqlField SqlField = SqlRecord.field(i);
		QVariant Value = SqlField.value();

		PMetaField *MetaField = MetaTable->GetField(SqlField.name());
		if (MetaField->Type == ISNamespace::FT_Image) //���� ���� �������� ������������, �������� ��� � base64
		{
			Value = Value.toByteArray().toBase64();
		}
		Values[MetaField->Name] = Value.isNull() ? QVariant() : Value;
	}

	//�������� ��� �������
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

	//�������� ����-�������
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

	//�������� ��� �������
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}
	TcpAnswer->Parameters["ObjectName"] = ObjectName;
	TcpAnswer->Parameters["CreateDate"] = qSelect.ReadColumn("create_date").toDateTime().toString(FORMAT_DATE_TIME_V3);
	TcpAnswer->Parameters["CreateUser"] = qSelect.ReadColumn("create_user");
	TcpAnswer->Parameters["EditDate"] = qSelect.ReadColumn("edit_date").toDateTime().toString(FORMAT_DATE_TIME_V3);
	TcpAnswer->Parameters["EditUser"] = qSelect.ReadColumn("edit_user");
	TcpAnswer->Parameters["CopyCount"] = qSelect.ReadColumn("copy_count");
	TcpAnswer->Parameters["EditCount"] = qSelect.ReadColumn("edit_count");
	TcpAnswer->Parameters["ShowCount"] = qSelect.ReadColumn("show_count");
	TcpAnswer->Parameters["FavoriteCount"] = qSelect.ReadColumn("favorite_count");
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
	if (!qInsert.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.DiscussionAdd.Insert"), qInsert);
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
	if (!qCopy.Execute()) //�� ������� ������� �����
	{
		return ErrorQuery(LANG("Carat.Error.Query.DiscussionCopy.Insert"), qCopy);
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

	//��������� ������� ����� �� �������� ������
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

	//���� ������� �� �������� ������ ��� - ������
	if (!qSelectRight.ReadColumn(0).toBool())
	{
		ErrorString = LANG("Carat.Error.Query.GetTableData.NoRightShow").arg(MetaTable->LocalListName);
		return false;
	}

	//�������� ���������� ��� ���� �������
	QString SortingField;
	Qt::SortOrder SortingOrder;
	ISQuery qSelectSorting(ISDatabase::Instance().GetDB(DBConnectionName), QS_SORTING);
	qSelectSorting.BindValue(":UserID", UserID);
	qSelectSorting.BindValue(":TableName", MetaTable->Name);
	if (!qSelectSorting.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetTableData.SelectSorting"), qSelectSorting);
	}

	if (qSelectSorting.First()) //��������� ���� - �������� �
	{
		SortingField = qSelectSorting.ReadColumn("sgts_fieldname").toString();
		SortingOrder = static_cast<Qt::SortOrder>(qSelectSorting.ReadColumn("sgts_sorting").toUInt());

		//��������� �� ������� �� ���������� � �������
		//���� ������� - ��������� - �� ����� �� �������� � � ��
		QVariantMap SortingFieldQuery = TcpMessage->Parameters["Sorting"].toMap();
		if (!SortingFieldQuery.isEmpty())
		{
			//���� ����� ���������� ���������� �� ������� - ��������� � � ��
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
			return ErrorQuery(LANG("Carat.Error.Query.GetTableData.InsertSorting"), qInsertSorting);
		}
	}

	//��������� ��������� ����������
	QVariantMap ServiceInfoMap =
	{
		{ "SortingField", SortingField },
		{ "SortingOrder", SortingOrder }
	};

	//��������� ������ �� �������
	QString SqlText = "SELECT\n",
		SqlTextJoins;
	size_t Index = 0;
	ISStringMap ForeignFields;

	//������� ���� ����-�������
	for (PMetaField *MetaField : MetaTable->Fields)
	{
		//���� ���� ������ � ������� - ���������� ���
		if (MetaField->HideFromList)
		{
			continue;
		}

		if (MetaField->Foreign) //���� �� ���� ���������� ������� ����
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
		else //���� �����������
		{
			if (MetaField->QueryText.isEmpty()) //���� �� �����������
			{
				SqlText += MetaTable->Alias + SYMBOL_POINT + MetaTable->Alias + '_' + MetaField->Name.toLower();
			}
			else //���� �������� �����������
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

	//���� ���������� ������� - �������������
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

	//���� ������� ��������� ���������
	QVariantList SearchList = TcpMessage->Parameters.contains("Search") ? TcpMessage->Parameters["Search"].toList() : QVariantList();
	bool IsSearch = !SearchList.isEmpty();
	if (IsSearch)
	{
		SqlText += "\nWHERE\n";
		for (const QVariant &Variant : SearchList) //������� ��������� �������
		{
			QVariantMap Map = Variant.toMap();
			QString FieldName = Map["FieldName"].toString();
			//ISNamespace::SearchOperatorType OperatorType = static_cast<ISNamespace::SearchOperatorType>(Map["Operator"].toUInt());
			QVariantList ValueList = Map["Values"].toList();

			SqlText += MetaTable->Alias + '_' + MetaTable->GetField(FieldName)->Name.toLower();			
			if (ValueList.size() > 1) //���� �������� ������ ��������� - ���������� �������� "IN"
			{
				SqlText += " IN(";
				for (const QVariant &Value : ValueList) //������� ������ ��������
				{
					QString UIDLite = GENERATE_UUID_LITE;
					SqlText += ':' + UIDLite + ',';
					FilterMap[UIDLite] = Value;
				}
				SqlText.chop(1);
				SqlText += ')';
			}
			else //���� �������� ��� ������ - ���������� ������� �������� "="
			{
				QString UIDLite = GENERATE_UUID_LITE;
				SqlText += " = :" + UIDLite;
				FilterMap[UIDLite] = ValueList.front();
			}
			SqlText += "\nAND ";
		}
		SqlText.chop(5);
	}

	//����������� ����������
	PMetaField *MetaFieldSorting = MetaTable->GetField(SortingField);
	bool SortingIsVirtual = !MetaFieldSorting->QueryText.isEmpty(),
		SortingIsForeign = MetaFieldSorting->Foreign;
	if (SortingIsVirtual) //���� ���� �������� ����������� - � �������� ���������� ����� ��������� ������
	{
		SortingField = '(' + MetaFieldSorting->QueryText + ')';
	}
	else //���� �� �����������: ���� �� ���� ���������� ������� ���� - ���������� �������������� ��� - ����� ������ ��� ����
	{
		SortingField = SortingIsForeign ? ForeignFields[MetaFieldSorting->Name] : MetaFieldSorting->Name;
	}

	//��������� ���������� � � �����������
	SqlText += "\nORDER BY " + (SortingIsVirtual || SortingIsForeign ? SortingField :
		MetaTable->Alias + '.' + MetaTable->Alias + '_' + SortingField) + ' ' +
		(SortingOrder == Qt::AscendingOrder ? "ASC" : "DESC");

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	qSelect.SetShowLongQuery(false);

	//��������� ��������� �������
	for (const auto &MapItem : FilterMap.toStdMap())
	{
		qSelect.BindValue(":" + MapItem.first, MapItem.second);
	}

	if (!qSelect.Execute()) //������ �� ���������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetTableData.Select"), qSelect);
	}

	QSqlRecord SqlRecord = qSelect.GetRecord();
	int FieldCount = SqlRecord.count();
	QVariantList FieldList, RecordList;
	std::vector<ISNamespace::FieldType> VectorType(FieldCount, ISNamespace::FT_Unknown);
	std::vector<bool> VectorForeign(FieldCount, false);

	//��������� �������� �����
	for (int i = 0; i < FieldCount; ++i)
	{
		PMetaField *MetaField = MetaTable->GetField(SqlRecord.fieldName(i));
		VectorType[i] = MetaField->Type; //��������� ���� ������, ����� ������������ �� ����
		VectorForeign[i] = MetaField->Foreign ? true : false;
		FieldList.append(QVariantMap
		{
			{ "Index", i },
			{ "Name", MetaField->Name },
			{ "LocalName", MetaField->LocalListName },
			{ "Type", MetaField->Type },
			{ "IsForeign", MetaField->Foreign ? true : false },
			{ "IsSysten", MetaField->IsSystem }
		});
	}

	if (qSelect.First()) //������ ����
	{
		//�������� ����������� ��������� ��
		unsigned int Precision = GetSettingDB(CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA).toInt();

		do
		{
			QVariantList Values; //������ ��������
			SqlRecord = qSelect.GetRecord(); //�������� ��������� ������
			for (int i = 0; i < FieldCount; ++i) //������� ���� � ����������� ��������
			{
				//�������� ��� ����
				ISNamespace::FieldType Type = VectorType[i];

				//�������� ������� �������� ����� �� ����
				bool IsForeign = VectorForeign[i];

				//���� �������� �������� NULL - ��������� ������ � ��������� � ����������
				QVariant Value = SqlRecord.value(i);
				if (Value.isNull())
				{
					Values.push_back(QVariant());
					continue;
				}
				
				//�������� �� �������� NULL - �����������
				if (Type == ISNamespace::FT_Int || Type == ISNamespace::FT_BigInt)
				{
					if (!IsForeign)
					{
						Value = ISAlgorithm::FormatNumber(Value.toLongLong());
					}
				}
				else if (Type == ISNamespace::FT_Date)
				{
					Value = ConvertDateToString(Value.toDate());
				}
				else if (Type == ISNamespace::FT_Time)
				{
					Value = Value.toTime().toString(FORMAT_TIME_V3);
				}
				else if (Type == ISNamespace::FT_DateTime)
				{
					Value = ConvertDateTimeToString(Value.toDateTime(), FORMAT_TIME_V1);
				}
				else if (Type == ISNamespace::FT_Birthday)
				{
					Value = Value.toDate().toString(FORMAT_DATE_V1);
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
					Value = ISAlgorithm::FormatNumber(Value.toDouble(), ' ', Precision);
				}
				else if (Type == ISNamespace::FT_Money)
				{
					Value = ISAlgorithm::FormatNumber(Value.toDouble(), ' ', 2);
				}
				else if (Type == ISNamespace::FT_UID)
				{
					Value = ISUuid(Value);
				}
				else if (Type == ISNamespace::FT_ProtocolDT)
				{
					Value = ConvertDateTimeToString(Value.toDateTime(), FORMAT_TIME_V3);
				}
				Values.push_back(Value);
			}
			RecordList.push_back(Values); //��������� ������ �������� � ������ �������
		} while (qSelect.Next()); //������� �������	
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

	//�������� ��� SQL-������� �� ������
	QString SqlText;
	QFileInfoList FileInfoList = QDir(":SQL").entryInfoList(QStringList() << "*.sql");
	for (const QFileInfo &FileInfo : FileInfoList)
	{
		QString s = FileInfo.completeBaseName();
		if (s == QueryName) //����� ������ - ������ ���
		{
			QFile FileSQL(FileInfo.filePath());
			if (!FileSQL.open(QIODevice::ReadOnly)) //�� ������� ������� ���� - ������
			{
				ErrorString = LANG("Carat.Error.Query.GetTableQuery.FileOpen").arg(FileSQL.errorString());
				return false;
			}

			//������, ��������� � ������� �� �����
			SqlText = FileSQL.readAll();
			FileSQL.close();
			break;
		}
	}

	//���� ������ �� ������ - ������
	if (SqlText.isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.GetTableQuery.NotFound");
		return false;
	}

	//��������� ������ �������, ������� ��������� � ���������
	QVariantMap ParametersMap = TcpMessage->Parameters["Parameters"].toMap();
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	for (const auto &MapItem : ParametersMap.toStdMap())
	{
		qSelect.BindValue(':' + MapItem.first, MapItem.second);
	}
	if (!qSelect.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetTableQuery.Select"), qSelect);
	}

	//������� ����
	QStringList FieldList;
	QSqlRecord SqlRecord = qSelect.GetRecord();
	int FieldCount = SqlRecord.count();
	for (int i = 0; i < FieldCount; ++i)
	{
		FieldList.append(SqlRecord.fieldName(i));
	}

	QVariantList RecordList;
	if (qSelect.First()) //������ ���� - ������� ������
	{
		do
		{
			//�������� ��������� ������
			SqlRecord = qSelect.GetRecord();

			//��������� ������ ���������� � ���������� � ������ �������
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
		return ErrorQuery(LANG("Carat.Error.Query.GetNoteRecord.Select"), qSelect);
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
			return ErrorQuery(LANG("Carat.Error.Query.SetNoteRecord.Delete"), qDelete);
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
	int Size = ByteArray.size(), MaxSizeMB = GetSettingDB(CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE).toInt();
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
	qInsert.BindValue(":Size", ISAlgorithm::FileSizeFromString(Size));
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

	//�������� ����
	ISQuery qInsertCopy(ISDatabase::Instance().GetDB(DBConnectionName), QI_FILE_STORAGE_COPY);
	qInsertCopy.SetShowLongQuery(false);
	qInsertCopy.BindValue(":Name", Name);
	qInsertCopy.BindValue(":ObjectID", ID);
	if (!qInsertCopy.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.FileStorageCopy.Insert"), qInsertCopy);
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
		return ErrorQuery(LANG("Carat.Error.Query.FileStorageGet.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.SearchTaskText.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.SearchFullText.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetCalendarEvents.Select"), qSelect);
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
		return ErrorQuery(LANG("Carat.Error.Query.CalendarDelete.Delete"), qDelete);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetInternalLists.Select"), qSelect);
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
						return ErrorQuery(LANG("Carat.Error.Query.SaveMetaData.UpdateColumnSize"), qUpdateColumnSize);
					}
				}
				else //������� �� ������� �� ������ �������
				{
					return ErrorQuery(LANG("Carat.Error.Query.SaveMetaData.InsertColumnSize"), qInsertColumnSize);
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
				return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSubSystems"), qSelectSubSystems);
			}
			SystemsList.append(SystemMap);
		}
	}
	else //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSystems"), qSelectSystems);
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
		return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.RightTableType"), qSelectAccessTablesType);
	}

	//������� �������
	QVariantList TablesList;
	QVariantMap TablesMap; //���� ��������� ����� ��� ���������� ������ ������ �� ��������
	ISQuery qSelectTables(ISDatabase::Instance().GetDB(DBConnectionName), QS_GROUP_RIGHT_TABLE);
	for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
	{
		//���� ������� �������� ��������� - ��� ������
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
				return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSpecial"), qSelectSpecial);
			}
			SpecialList.append(SpecialGroupMap);
		}
	}
	else //������ ������� � ������� ����. ����
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
		return ErrorQuery(LANG("Carat.Error.Query.GroupRightSubSystemDelete.Delete"), qDeleteSubSystemRight);
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
		return ErrorQuery(LANG("Carat.Error.Query.GroupRightTableDelete.Delete"), qDeleteTableRight);
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
		return ErrorQuery(LANG("Carat.Error.Query.GroupRightSpecialDelete.Delete"), qDeleteSpecialRight);
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

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//�������� ��� ������
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//��������� ������ � ���������
	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_FAVORITE);
	qInsert.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qInsert.BindValue(":TableName", MetaTable->Name);
	qInsert.BindValue(":ObjectID", ObjectID);
	if (!qInsert.Execute()) //�� ������� �������� - ��������� ������
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

	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//�������� ��� ������
	QString ObjectName;
	if (!GetObjectName(MetaTable, ObjectID.toUInt(), ObjectName))
	{
		return false;
	}

	//������� ������
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName), QD_FAVORITE);
	qDelete.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qDelete.BindValue(":TableName", MetaTable->Name);
	qDelete.BindValue(":ObjectID", ObjectID);
	if (!qDelete.Execute()) //�� ������� �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.RecordFavoriteDelete.Delete"), qDelete);
	}

	//���� ��� �������� �� ���� ��������� �� ���� ������ - ������ ����� ������ ���
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
	//����������� ��������� ������
	ISQuery qSelectObjects(ISDatabase::Instance().GetDB(DBConnectionName));
	if (TcpMessage->Parameters.contains("TableName")) //���� ������� ������� - ������� ������ �� ��������� �������
	{
		//��������� ������� ����� ������� � ����-������
		QString TableName = TcpMessage->Parameters["TableName"].toString();
		if (!GetMetaTable(TableName))
		{
			return false;
		}
		qSelectObjects.Prepare(QS_FAVORITE_NAMES_TABLE);
		qSelectObjects.BindValue(":TableName", TableName);
	}
	else //������� �� ������� - ������� ��� ������
	{
		qSelectObjects.Prepare(QS_FAVORITE_NAMES_ALL);
	}
	qSelectObjects.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qSelectObjects.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetFavoriteNames.Select"), qSelectObjects);
	}

	PMetaTable *MetaTable = nullptr;
	QString ObjectName;

	//�������
	QVariantList NamesList;
	while (qSelectObjects.Next())
	{
		QString TableName = qSelectObjects.ReadColumn("fvts_tablename").toString();
		if (!MetaTable || MetaTable->Name != TableName) //���� ����-������� ��� �� �������� ��� � ��� ��������� - �������� �
		{
			MetaTable = GetMetaTable(TableName);
			if (!MetaTable) //�� ������� �������� ����-������� - ������� � �������
			{
				return false;
			}
		}
	
		//�������� ��� ������
		unsigned int ObjectID = qSelectObjects.ReadColumn("fvts_objectid").toUInt();
		if (!GetObjectName(MetaTable, ObjectID, ObjectName))
		{
			return false;
		}

		//��������� � �������������� ������
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

	//������� ��������� ������
	ISQuery qDelete(ISDatabase::Instance().GetDB(DBConnectionName));
	if (TcpMessage->Parameters.contains("TableName")) //���� ������� ������� - ������� ������ �� ��������� �������
	{
		//��������� ������� ����� ������� � ����-������
		QString TableName = TcpMessage->Parameters["TableName"].toString();
		if (!GetMetaTable(TableName))
		{
			return false;
		}
		qDelete.Prepare(QD_FAVORITES_TABLE);
		qDelete.BindValue(":TableName", TableName);
	}
	else //������� �� ������� - ������� ��� ������
	{
		qDelete.Prepare(QD_FAVORITES_ALL);
	}
	qDelete.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qDelete.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.FavoritesDelete.Delete"), qDelete);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::LogGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant Date = CheckNullField("Date", TcpMessage);
	if (!Date.isValid())
	{
		return false;
	}

	//��������� ���� � ����� � �������� ��� �������
	QFile File(ISLogger::Instance().GetPathLogsDir() + QCoreApplication::applicationName() + '_' + Date.toString() + ".log");
	if (!File.open(QIODevice::ReadOnly)) //�� ������� ������� ����
	{
		ISLOGGER_W(__CLASS__, "Not open file \"" + File.fileName() + "\": " + File.errorString());
		ErrorString = LANG("Carat.Error.Query.LogGet.Open");
		return false;
	}

	//������, ��������� � ��������� ������
	QString Content = File.readAll();
	File.close();
	QStringList StringList = Content.split('\n');

	//��������� ������ �����
	QStringList FieldList =
	{
		LANG("Carat.Log.Field.Time"),
		LANG("Carat.Log.Field.ThreadID"),
		LANG("Carat.Log.Field.Severity"),
		LANG("Carat.Log.Field.ModuleName"),
		LANG("Carat.Log.Field.Message")
	};

	//������� ������ ���-�����
	QVariantList RecordList;
	for (const QString &Line : StringList)
	{
		//���� ������ ������ ���������
		int Pos = Line.indexOf('\t');
		if (Pos == -1) //�� ����� ������ - ���������� ������
		{
			continue;
		}

		QString String = Line;

		//����������� �����
		QString Time = ISAlgorithm::StringTake(String, 0, ++Pos);
		Pos = Time.indexOf(' ');
		if (Pos == -1)
		{
			continue;
		}
		Time = ISAlgorithm::StringTake(Time, Pos + 1, Time.size() - Pos - 2);

		//����������� ����� ������
		Pos = String.indexOf('\t');
		if (Pos == -1)
		{
			continue;
		}
		QString ThreadID = ISAlgorithm::StringTake(String, 0, ++Pos);
		ThreadID.chop(1);

		//����������� ������� ���������
		Pos = String.indexOf(']');
		if (Pos == -1)
		{
			continue;
		}
		QString Severity = ISAlgorithm::StringTake(String, 0, ++Pos);
		Severity = Severity.mid(1, Severity.size() - 2);

		//����������� ��� ����������
		Pos = String.indexOf(']');
		if (Pos == -1)
		{
			continue;
		}
		QString ModuleName = ISAlgorithm::StringTake(String, 0, ++Pos);
		ModuleName = ModuleName.mid(1, ModuleName.size() - 2);

		//������� ������ � ������ ������ � ������ �������� ������
		String = String.mid(1, String.size() - 2);

		RecordList.append(QVariantMap
		{
			{ "Color", GetColorForLogMessage(Severity) },
			{ "Values", QStringList() << Time << ThreadID << Severity << ModuleName << String }
		});
	}
	TcpAnswer->Parameters["FieldList"] = FieldList;
	TcpAnswer->Parameters["RecordList"] = RecordList;
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

	//��������� �������
	ISQuery qUpdate(ISDatabase::Instance().GetDB(DBConnectionName), QU_CALENDAR_CLOSE);
	qUpdate.BindValue(":CalendarID", CalendarID);
	if (!qUpdate.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.CalendarClose.Update"), qUpdate);
	}

	//�� ���� ������ �� ���� ������� - ������ � � ��� - ������
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
	//�������� ������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_HISTORY);
	qSelect.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	if (!qSelect.Execute()) //������ �������
	{
		ErrorString = LANG("Carat.Error.Query.GetHistoryList.Select");
		return false;
	}

	PMetaTable *MetaTable = nullptr;
	QString ObjectName;

	//������� �������
	QVariantList HistoryList;
	while (qSelect.Next())
	{
		QString TableName = qSelect.ReadColumn("prtc_tablename").toString();
		if (!MetaTable || MetaTable->Name != TableName) //���� ����-������� ��� �� �������� ��� � ��� ��������� - �������� �
		{
			MetaTable = GetMetaTable(TableName);
			if (!MetaTable) //�� ������� �������� ����-������� - ������� � �������
			{
				return false;
			}
		}
		
		//�������� ��� ������
		unsigned int ObjectID = qSelect.ReadColumn("prtc_objectid").toUInt();
		if (!GetObjectName(MetaTable, ObjectID, ObjectName))
		{
			return false;
		}

		//��������� � �������������� ������
		HistoryList.append(QVariantMap
		{
			{ "DateTime", ConvertDateTimeToString(qSelect.ReadColumn("prtc_datetime").toDateTime(), FORMAT_TIME_V3) },
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

	//��������� ����������� �� �������
	if (Comment.toString().isEmpty())
	{
		ErrorString = LANG("Carat.Error.Query.TaskCommentAdd.CommentIsEmpty");
		return false;
	}

	//������������� ������������� �����������
	QVariant ParentCommentID = TcpMessage->Parameters.contains("ParentCommentID") ?
		TcpMessage->Parameters["ParentCommentID"] : QVariant();

	//��������� �����������
	ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_TASK_COMMENT);
	qInsert.BindValue(":UserID", TcpMessage->TcpSocket->GetUserID());
	qInsert.BindValue(":TaskID", TaskID);
	qInsert.BindValue(":ParentCommentID", ParentCommentID);
	qInsert.BindValue(":Comment", Comment);
	if (!qInsert.Execute()) //������ �������
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
	
	//�������� ����-�������
	PMetaTable *MetaTable = GetMetaTable(TableName.toString());
	if (!MetaTable)
	{
		return false;
	}

	//�������� ����-����
	PMetaField *MetaField = MetaTable->GetField(FieldName.toString());
	if (!MetaField)
	{
		ErrorString = LANG("Carat.Error.Query.GetForeignList.FieldNotExist").arg(FieldName.toString()).arg(MetaTable->Name);
		return false;
	}

	//��������� ������� �������� ����� �� ����
	if (!MetaField->Foreign)
	{
		ErrorString = LANG("Carat.Error.Query.GetForeignList.FieldNotForeign").arg(FieldName.toString());
		return false;
	}

	//��������� ������
	PMetaTable *MetaTableForeign = ISMetaData::Instance().GetMetaTable(MetaField->Foreign->ForeignClass); //������� �� ������� ��������� ������� ����
	QStringList FieldList = MetaField->Foreign->ForeignViewNameField.split(';');

	QString SqlText = "SELECT " + MetaTableForeign->Alias + '_' + MetaField->Foreign->ForeignField.toLower() + " AS ID, concat(";
	for (const QString &String : FieldList) //����� ����� (������� ������ ���� ����������)
	{
		SqlText += MetaTableForeign->Alias + '_' + String.toLower() + ", ' ',";
	}
	SqlText.chop(6);
	SqlText += ") ";
	SqlText += "AS Value\n";
	SqlText += "FROM " + MetaTableForeign->Name + "\n";
	SqlText += "ORDER BY 2 ASC";

	//��������� ������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), SqlText);
	if (!qSelect.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetForeignList.Select"), qSelect);
	}

	//������� ���������� �������
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
bool ISTcpWorker::PeriodContains(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_PERIOD);
	if (!qSelect.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.PeriodContains.Select"), qSelect);
	}
	bool IsExist = qSelect.First();
	TcpAnswer->Parameters["Exist"] = IsExist;
	TcpAnswer->Parameters["Value"] = IsExist ? qSelect.ReadColumn("prod_constant") : QVariant();
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetStockList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpMessage);

	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_STOCK);
	if (!qSelect.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetStockList.Select"), qSelect);
	}

	//������� ���������� �������
	QVariantList StockList;
	while (qSelect.Next())
	{
		StockList.append(QVariantMap
		{
			{ "ID", qSelect.ReadColumn("stck_id") },
			{ "Name", qSelect.ReadColumn("stck_name") },
		});
	}
	TcpAnswer->Parameters["List"] = StockList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::StatementAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	Q_UNUSED(TcpAnswer);

	QVariant ImplementationUnload = CheckNullField("ImplementationUnload", TcpMessage),
		UnloadStock = CheckNullField("UnloadStock", TcpMessage),
		ValumeIncome = CheckNullField("ValumeIncome", TcpMessage);
	if (!ImplementationUnload.isValid() || !UnloadStock.isValid() || !ValumeIncome.isValid())
	{
		return false;
	}

	//��������� ������� ����� ������
	ISQuery qSelectUnload(ISDatabase::Instance().GetDB(DBConnectionName), QS_STATEMENT);
	qSelectUnload.BindValue(":ImplementationUnload", ImplementationUnload);
	if (!qSelectUnload.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.StatementAdd.Select"), qSelectUnload);
	}

	if (!qSelectUnload.First()) //�� ������� ������� �� ������ ������
	{
		ErrorString = qSelectUnload.GetErrorString();
		return false;
	}

	//���� �������� � ���������� ��� ��� - ���������
	if (qSelectUnload.ReadColumn("count").toInt() == 0)
	{
		ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_STATEMENT);
		qInsert.BindValue(":ImplementationUnload", ImplementationUnload);
		qInsert.BindValue(":StockID", UnloadStock);
		qInsert.BindValue(":VolumeIncome", ValumeIncome);
		if (!qInsert.Execute()) //�� ������� ��������
		{
			return ErrorQuery(LANG("Carat.Error.Query.StatementAdd.Insert"), qInsert);
		}
	}

	//�������� ��� ���������� - ��� ����
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetGasStation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant StatementID = CheckNullField("StatementID", TcpMessage);
	if (!StatementID.isValid())
	{
		return false;
	}

	//����������� ��������
	ISQuery qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_FILL_IN_BASED);
	qSelect.BindValue(":StatementID", StatementID);
	if (!qSelect.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetGasStation.Select"), qSelect);
	}

	if (!qSelect.First()) //����� ������ �� ����������
	{
		ErrorString = LANG("Carat.Error.Query.GetGasStation.NotExist");
		return false;
	}

	TcpAnswer->Parameters["BalanceEndChange"] = qSelect.ReadColumn("gsts_balanceendchange");
	TcpAnswer->Parameters["CashboxTotalPayment"] = qSelect.ReadColumn("gsts_cashboxtotalpayment");
	TcpAnswer->Parameters["CashboxTotalActually"] = qSelect.ReadColumn("gsts_cashboxtotalactually");
	TcpAnswer->Parameters["CashboxKKMTotal"] = qSelect.ReadColumn("gsts_cashboxkkmtotal");
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetDebtImplementation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant CounterpartyID = CheckNullField("CounterpartyID", TcpMessage);
	if (!CounterpartyID.isValid())
	{
		return false;
	}

	//����������� �������� � ��������
	ISQuery qSelectLoadUnload(ISDatabase::Instance().GetDB(DBConnectionName), QS_IMPLEMENTATION_UNLOAD);
	qSelectLoadUnload.BindValue(":CounterpartyID", CounterpartyID);
	if (!qSelectLoadUnload.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetDebtImplementation.Select"), qSelectLoadUnload);
	}

	//������� ���������� �������
	QVariantList LoadUnloadList;
	while (qSelectLoadUnload.Next())
	{
		LoadUnloadList.append(QVariantMap
		{
			{ "IsLoad", qSelectLoadUnload.ReadColumn("is_load") },
			{ "ImplementationID", qSelectLoadUnload.ReadColumn("implementation_id") },
			{ "LoadUnloadID", qSelectLoadUnload.ReadColumn("id") },
			{ "Date", qSelectLoadUnload.ReadColumn("date").toDate().toString(FORMAT_DATE_V2) },
			{ "Cost", qSelectLoadUnload.ReadColumn("cost") }
		});
	}
	TcpAnswer->Parameters["LoadUnload"] = LoadUnloadList;
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetDebtCounterparty(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	QVariant CounterpartyID = CheckNullField("CounterpartyID", TcpMessage);
	if (!CounterpartyID.isValid())
	{
		return false;
	}

	//����������� �������� �����
	ISQuery qSelectTitle(ISDatabase::Instance().GetDB(DBConnectionName), QS_COUNTERPARTY_DEBT);
	qSelectTitle.BindValue(":CounterpartyID", CounterpartyID);
	if (!qSelectTitle.Execute()) //������ �������
	{
		return ErrorQuery(LANG("Carat.Error.Query.GetDebtCounterparty.Select"), qSelectTitle);
	}

	if (!qSelectTitle.First()) //����� ������ �� ����������
	{
		ErrorString = LANG("Carat.Error.Query.GetDebtCounterparty.NotExist");
		return false;
	}
	TcpAnswer->Parameters["TotalUnload"] = qSelectTitle.ReadColumn("get_counterparty_unload").toDouble();
	TcpAnswer->Parameters["TotalLoad"] = qSelectTitle.ReadColumn("get_counterparty_load").toDouble();
	TcpAnswer->Parameters["TotalEntrollment"] = qSelectTitle.ReadColumn("get_counterparty_entrollment").toDouble();
	TcpAnswer->Parameters["MoveWagonSum"] = qSelectTitle.ReadColumn("get_counterparty_move_wagon").toDouble();
	return true;
}
//-----------------------------------------------------------------------------
