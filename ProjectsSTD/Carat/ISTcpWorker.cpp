#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISTcpQueue.h"
#include "ISAssert.h"
#include "ISLogger.h"
#include "ISConfig.h"
#include "ISDatabase.h"
#include "ISLocalization.h"
#include "ISTcpClients.h"
#include "ISResourcer.h"
#include "ISConfigurations.h"
#include "ISTcpWorkerHelper.h"
#include "ISRevision.h"
#include "ISProperty.h"
#include <curl/curl.h>
#include <rapidjson/error/en.h>
//-----------------------------------------------------------------------------
static std::string Q_TEST = PREPARE_QUERYN("INSERT INTO _usergroup(usgp_name, usgp_fullaccess) VALUES($1, $2)", 2);
//-----------------------------------------------------------------------------
static std::string QS_USERS_HASH = PREPARE_QUERY("SELECT usrs_hash, usrs_salt "
    "FROM _users "
    "WHERE usrs_hash IS NOT NULL "
    "AND usrs_salt IS NOT NULL");
//-----------------------------------------------------------------------------
static std::string QS_USER_AUTH = PREPARE_QUERYN("SELECT usrs_id, usrs_issystem, usrs_fio, usrs_group, usgp_fullaccess, usrs_accessallowed, usrs_accountlifetime, usrs_accountlifetimestart, usrs_accountlifetimeend, "
    "(SELECT sgdb_useraccessdatabase FROM _settingsdatabase WHERE sgdb_uid = $1) "
    "FROM _users "
    "LEFT JOIN _usergroup ON usgp_id = usrs_group "
    "WHERE usrs_hash = $2", 2);
//-----------------------------------------------------------------------------
static std::string QI_PROTOCOL = PREPARE_QUERYN("SELECT protocol_user($1, $2, $3, $4, $5, $6)", 6);
//-----------------------------------------------------------------------------
static std::string QS_SETTINGS_DATABASE = PREPARE_QUERYN("SELECT sgdb_id, sgdb_useraccessdatabase, sgdb_numbersimbolsaftercomma, sgdb_storagefilemaxsize, sgdb_tcpmessageid "
    "FROM _settingsdatabase "
    "WHERE sgdb_uid = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_GROUP_ACCESS_TABLE = PREPARE_QUERYN("SELECT gatb_table, gatt_uid "
    "FROM _groupaccesstable "
    "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_AccessType "
    "WHERE gatb_group = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_GROUP_ACCESS_SPECIAL = PREPARE_QUERYN("SELECT gast_uid "
    "FROM _groupaccessspecial "
    "LEFT JOIN _groupaccessspecialtype ON gast_id = gasp_specialaccess "
    "WHERE gasp_group = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_SYSTEM_SUBSYSTEM = PREPARE_QUERYN("SELECT check_access_user_subsystem($1, $2, sbsm_uid), "
    "stms_uid, stms_issystem, stms_localname, stms_icon, stms_hint, "
    "sbsm_uid, sbsm_localname, sbsm_icon, sbsm_classname, sbsm_tablename, sbsm_hint "
    "FROM _systems "
    "LEFT JOIN _subsystems ON stms_uid = sbsm_system "
    "ORDER BY stms_orderid, sbsm_orderid", 2);
//-----------------------------------------------------------------------------
static std::string QS_FAVORITE = PREPARE_QUERYN("SELECT fvts_tablename, fvts_objectid "
    "FROM _favorites "
    "WHERE fvts_user = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_COLUMN_SIZE = PREPARE_QUERYN("SELECT clsz_tablename, clsz_fieldname, clsz_size "
    "FROM _columnsize "
    "WHERE clsz_user = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_SETTINGS = PREPARE_QUERYN("SELECT stgp_uid, stgp_name, stgp_localname, stgp_iconname, stgp_hint, "
    "stgs_uid, stgs_name, stgs_type, stgs_widgeteditname, stgs_localname, stgs_hint, stgs_defaultvalue, "
    "usst_value, "
    "(SELECT COUNT(*) FROM _usersettings WHERE usst_user = $1 AND usst_setting = stgs_id) "
    "FROM _settingsgroup "
    "LEFT JOIN _settings ON stgs_group = stgp_uid "
    "LEFT JOIN _usersettings ON usst_setting = stgs_id AND usst_user = $1 "
    "ORDER BY stgp_order, stgs_order", 1);
//-----------------------------------------------------------------------------
static std::string QI_USER_SETTING = "INSERT INTO _usersettings(usst_user, usst_setting, usst_value) "
"VALUES($1, (SELECT stgs_id FROM _settings WHERE stgs_uid = $2), $3)";
//-----------------------------------------------------------------------------
static std::string QS_PARAGRAPH = PREPARE_QUERY("SELECT prhs_uid, prhs_name, prhs_localname, prhs_tooltip, prhs_icon, prhs_classname "
    "FROM _paragraphs "
    "ORDER BY prhs_orderid");
//-----------------------------------------------------------------------------
static std::string QS_TASK_PRIORITY = PREPARE_QUERY("SELECT tspr_id, tspr_name, tspr_tooltip, tspr_stylesheet, tspr_icon "
    "FROM _taskpriority "
    "ORDER BY tspr_order");
//-----------------------------------------------------------------------------
static std::string QS_USER_PASSWORD_IS_NULL = PREPARE_QUERYN("SELECT "
    "( "
    "SELECT (COUNT(*) > 0)::BOOLEAN is_exist "
    "FROM _users "
    "WHERE usrs_id = $1 "
    "AND usrs_hash IS NOT NULL "
    "AND usrs_salt IS NOT NULL "
    ") "
    "FROM _users "
    "WHERE usrs_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_USER_PASSWORD = PREPARE_QUERYN("SELECT usrs_hash, usrs_salt "
    "FROM _users "
    "WHERE usrs_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QU_USER_HASH_RESET = PREPARE_QUERYN("UPDATE _users SET "
    "usrs_hash = NULL, "
    "usrs_salt = NULL "
    "WHERE usrs_id = $1 "
    "RETURNING usrs_fio", 1);
//-----------------------------------------------------------------------------
static std::string QS_USER_IS_SYSTEM = PREPARE_QUERYN("SELECT usrs_issystem "
    "FROM _users "
    "WHERE usrs_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QU_USER_HASH = PREPARE_QUERYN("UPDATE _users SET "
    "usrs_hash = $1, "
    "usrs_salt = $2 "
    "WHERE usrs_id = $3 "
    "RETURNING usrs_fio", 3);
//-----------------------------------------------------------------------------
static std::string QU_USER_SETTINGS_RESET = PREPARE_QUERYN("UPDATE _usersettings SET "
    "usst_value = (SELECT stgs_defaultvalue FROM _settings WHERE stgs_id = usst_setting) "
    "WHERE usst_user = $1 "
    "RETURNING (SELECT stgs_uid FROM _settings WHERE stgs_id = usst_setting), usst_value", 1);
//-----------------------------------------------------------------------------
static std::string QS_ASTERISK_RECORD = PREPARE_QUERYN("SELECT ascl_uniqueid "
    "FROM _asteriskcalls "
    "WHERE ascl_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_RIGHT_SHOW_TABLE = PREPARE_QUERYN("SELECT check_access_user_table($1, $2, $3)", 3);
//-----------------------------------------------------------------------------
static std::string QS_SORTING = PREPARE_QUERYN("SELECT sgts_fieldname, sgts_sorting "
    "FROM _sortingtables "
    "WHERE sgts_user = $1 "
    "AND sgts_tablename = $2", 2);
//-----------------------------------------------------------------------------
static std::string QU_SORTING = PREPARE_QUERYN("UPDATE _sortingtables SET "
    "sgts_fieldname = $1, "
    "sgts_sorting = $2 "
    "WHERE sgts_user = $3 "
    "AND sgts_tablename = $4", 4);
//-----------------------------------------------------------------------------
static std::string QI_SORTING = PREPARE_QUERYN("INSERT INTO _sortingtables(sgts_user, sgts_tablename, sgts_fieldname, sgts_sorting)"
    "VALUES($1, $2, $3, $4)", 4);
//-----------------------------------------------------------------------------
static std::string QS_RECORD_INFO = PREPARE_QUERYN("SELECT "
    "( "
    "SELECT prtc_datetime AS create_date "
    "FROM _protocol "
    "LEFT JOIN _users ON usrs_id = prtc_user "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{D1348312-298F-4A7C-B584-9BA8C4952CD3}') "
    "ORDER BY prtc_datetime ASC "
    "LIMIT 1 "
    "), "
    "( "
    "SELECT usrs_fio AS create_user "
    "FROM _protocol "
    "LEFT JOIN _users ON usrs_id = prtc_user "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{D1348312-298F-4A7C-B584-9BA8C4952CD3}') "
    "ORDER BY prtc_datetime ASC "
    "LIMIT 1 "
    "), "
    "( "
    "SELECT prtc_datetime AS edit_date "
    "FROM _protocol "
    "LEFT JOIN _users ON usrs_id = prtc_user "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{0361643D-0A62-4F51-84BD-313F53115EFD}') "
    "ORDER BY prtc_datetime DESC "
    "LIMIT 1 "
    "), "
    "( "
    "SELECT usrs_fio AS edit_user "
    "FROM _protocol "
    "LEFT JOIN _users ON usrs_id = prtc_user "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{0361643D-0A62-4F51-84BD-313F53115EFD}') "
    "ORDER BY prtc_datetime DESC "
    "LIMIT 1 "
    "), "
    "( "
    "SELECT COUNT(*) AS copy_count "
    "FROM _protocol "
    "LEFT JOIN _users ON usrs_id = prtc_user "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{EFA8FE45-1174-4D2E-BBE6-4940380961D4}') "
    "), "
    "( "
    "SELECT COUNT(*) AS edit_count "
    "FROM _protocol "
    "LEFT JOIN _users ON usrs_id = prtc_user "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{0361643D-0A62-4F51-84BD-313F53115EFD}') "
    "), "
    "( "
    "SELECT COUNT(*) AS show_count "
    "FROM _protocol "
    "LEFT JOIN _users ON usrs_id = prtc_user "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "AND prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{117E8972-97DC-4E72-93AC-DC3BB50D11CF}') "
    "), "
    "( "
    "SELECT COUNT(*) favorite_count "
    "FROM _favorites "
    "WHERE fvts_tablename = $1 "
    "AND fvts_objectid = $2 "
    "), "
    "( "
    "SELECT COUNT(*) AS protocol_count "
    "FROM _protocol "
    "WHERE prtc_tablename = $1 "
    "AND prtc_objectid = $2 "
    "), "
    "( "
    "SELECT COUNT(*) AS discussion_count "
    "FROM _discussion "
    "WHERE dson_tablename = $1 "
    "AND dson_objectid = $2 "
    ")", 2);
//-----------------------------------------------------------------------------
static std::string QS_SERVER_INFO = PREPARE_QUERY("SELECT "
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
    "(SELECT COUNT(*) AS users_count FROM _users), "
    "(SELECT COUNT(*) AS files_count FROM _storagefiles), "
    "(SELECT sum(length(sgfs_data)) AS files_size FROM _storagefiles)");
//-----------------------------------------------------------------------------
static std::string QD_FAVORITE = PREPARE_QUERYN("DELETE FROM _favorites "
    "WHERE fvts_user = $1 "
    "AND fvts_tablename = $2 "
    "AND fvts_objectid = $3", 3);
//-----------------------------------------------------------------------------
static std::string QI_FILE_STORAGE = PREPARE_QUERYN("INSERT INTO _storagefiles(sgfs_owner, sgfs_name, sgfs_expansion, sgfs_size, sgfs_data) "
    "VALUES ($1, $2, $3, $4, $5) "
    "RETURNING sgfs_id", 5);
//-----------------------------------------------------------------------------
static std::string QI_FILE_STORAGE_COPY = PREPARE_QUERYN("INSERT INTO _storagefiles(sgfs_owner, sgfs_name, sgfs_expansion, sgfs_size, sgfs_note, sgfs_data) "
    "SELECT sgfs_owner, $1, sgfs_expansion, sgfs_size, sgfs_note, sgfs_data "
    "FROM _storagefiles "
    "WHERE sgfs_id = $2 "
    "RETURNING sgfs_id", 2);
//-----------------------------------------------------------------------------
static std::string QS_FILE_STORAGE = PREPARE_QUERYN("SELECT sgfs_id, sgfs_data "
    "FROM _storagefiles "
    "WHERE sgfs_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QI_DISCUSSION = PREPARE_QUERYN("INSERT INTO _discussion(dson_user, dson_tablename, dson_objectid, dson_message) "
    "VALUES($1, $2, $3, $4) "
    "RETURNING dson_id", 4);
//-----------------------------------------------------------------------------
static std::string QU_DISCUSSION = PREPARE_QUERYN("UPDATE _discussion SET "
    "dson_message = $1 "
    "WHERE dson_id = $2", 2);
//-----------------------------------------------------------------------------
static std::string QI_DISCUSSION_COPY = PREPARE_QUERYN("INSERT INTO _discussion(dson_user, dson_tablename, dson_objectid, dson_message) "
    "SELECT dson_user, dson_tablename, dson_objectid, dson_message "
    "FROM _discussion "
    "WHERE dson_id = $1 "
    "RETURNING dson_id", 1);
//-----------------------------------------------------------------------------
static std::string QS_NOTE_RECORD = PREPARE_QUERYN("SELECT nobj_note "
    "FROM _noteobject "
    "WHERE nobj_tablename = $1 "
    "AND nobj_objectid = $2", 2);
//-----------------------------------------------------------------------------
static std::string QD_NOTE_RECORD = PREPARE_QUERYN("DELETE FROM _noteobject "
    "WHERE nobj_tablename = $1 "
    "AND nobj_objectid = $2", 2);
//-----------------------------------------------------------------------------
static std::string QU_NOTE_RECORD = PREPARE_QUERYN("UPDATE _noteobject SET "
    "nobj_note = $1 "
    "WHERE nobj_tablename = $2 "
    "AND nobj_objectid = $3", 3);
//-----------------------------------------------------------------------------
static std::string QI_NOTE_RECORD = PREPARE_QUERYN("INSERT INTO _noteobject(nobj_note, nobj_tablename, nobj_objectid) "
    "VALUES($1, $2, $3)", 3);
//-----------------------------------------------------------------------------
static std::string QS_INTERNAL_LISTS = PREPARE_QUERY("SELECT intd_tablename "
    "FROM _internaldirectories "
    "ORDER BY intd_order");
//-----------------------------------------------------------------------------
static std::string QS_FAVORITE_NAMES_TABLE = PREPARE_QUERYN("SELECT fvts_tablename, fvts_objectid "
    "FROM _favorites "
    "WHERE fvts_tablename = $1 "
    "AND fvts_user = $2 "
    "ORDER BY fvts_tablename", 2);
//-----------------------------------------------------------------------------
static std::string QS_FAVORITE_NAMES_ALL = PREPARE_QUERYN("SELECT fvts_tablename, fvts_objectid "
    "FROM _favorites "
    "WHERE fvts_user = $1 "
    "ORDER BY fvts_tablename", 1);
//-----------------------------------------------------------------------------
static std::string QI_FAVORITE = PREPARE_QUERYN("INSERT INTO _favorites(fvts_user, fvts_tablename, fvts_objectid) "
    "VALUES($1, $2, $3)", 3);
//-----------------------------------------------------------------------------
static std::string QD_FAVORITES_TABLE = PREPARE_QUERYN("DELETE FROM _favorites "
    "WHERE fvts_tablename = $1 "
    "AND fvts_user = $2", 2);
//-----------------------------------------------------------------------------
static std::string QD_FAVORITES_ALL = PREPARE_QUERYN("DELETE FROM _favorites "
    "WHERE fvts_user = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_HISTORY = PREPARE_QUERYN("SELECT prtc_datetime, prtc_tablename, prtc_objectid "
    "FROM _protocol "
    "WHERE prtc_type = (SELECT prtp_id FROM _protocoltype WHERE prtp_uid = '{117E8972-97DC-4E72-93AC-DC3BB50D11CF}') "
    "AND prtc_user = $1 "
    "ORDER BY prtc_datetime DESC", 1);
//-----------------------------------------------------------------------------
static std::string QS_GROUP_RIGHT_SYSTEM = PREPARE_QUERY("SELECT stms_uid, stms_localname "
    "FROM _systems "
    "ORDER BY stms_orderid");
//-----------------------------------------------------------------------------
static std::string QS_GROUP_RIGHT_SUBSYSTEM = PREPARE_QUERYN("SELECT sbsm_uid, sbsm_localname, "
    "(SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM _groupaccesssubsystem WHERE gass_group = $1 AND gass_subsystem = sbsm_uid) "
    "FROM _subsystems "
    "WHERE sbsm_system = $2 "
    "ORDER BY sbsm_orderid", 2);
//-----------------------------------------------------------------------------
static std::string QS_GROUP_RIGHT_TABLE_TYPE = PREPARE_QUERY("SELECT gatt_uid, gatt_name, gatt_icon "
    "FROM _groupaccesstabletype "
    "ORDER BY gatt_order");
//-----------------------------------------------------------------------------
static std::string QS_GROUP_RIGHT_TABLE = PREPARE_QUERYN("SELECT gatt_uid "
    "FROM _groupaccesstable "
    "LEFT JOIN _groupaccesstabletype ON gatt_id = gatb_accesstype "
    "WHERE gatb_group = $1 "
    "AND gatb_table = $2", 2);
//-----------------------------------------------------------------------------
static std::string QS_GROUP_RIGHT_SPECIAL_PARENT = PREPARE_QUERY("SELECT gast_uid, gast_name "
    "FROM _groupaccessspecialtype "
    "WHERE gast_parent IS NULL "
    "ORDER BY gast_order");
//-----------------------------------------------------------------------------
static std::string QS_GROUP_RIGHT_SPECIAL = PREPARE_QUERYN("SELECT gast_uid, gast_name, gast_hint, "
    "(SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM _groupaccessspecial WHERE gasp_group = $1 AND gasp_specialaccess = gast_id) "
    "FROM _groupaccessspecialtype "
    "WHERE gast_parent = $2 "
    "ORDER BY gast_order", 2);
//-----------------------------------------------------------------------------
static std::string QI_GROUP_RIGHT_SUBSYSTEM = PREPARE_QUERYN("INSERT INTO _groupaccesssubsystem(gass_group, gass_subsystem) "
    "VALUES($1, $2) "
    "RETURNING (SELECT sbsm_localname FROM _subsystems WHERE sbsm_uid = $2)", 2);
//-----------------------------------------------------------------------------
static std::string QD_GROUP_RIGHT_SUBSYSTEM = PREPARE_QUERYN("DELETE FROM _groupaccesssubsystem "
    "WHERE gass_group = $1 "
    "AND gass_subsystem = $2 "
    "RETURNING (SELECT sbsm_localname FROM _subsystems WHERE sbsm_uid = $2)", 2);
//-----------------------------------------------------------------------------
static std::string QI_GROUP_RIGHT_TABLE = PREPARE_QUERYN("INSERT INTO _groupaccesstable(gatb_group, gatb_table, gatb_accesstype) "
    "VALUES($1, $2, (SELECT gatt_id FROM _groupaccesstabletype WHERE gatt_uid = $3)) "
    "RETURNING (SELECT gatt_name FROM _groupaccesstabletype WHERE gatt_uid = $3)", 3);
//-----------------------------------------------------------------------------
static std::string QD_GROUP_RIGHT_TABLE = PREPARE_QUERYN("DELETE FROM _groupaccesstable "
    "WHERE gatb_group = $1 "
    "AND gatb_table = $2 "
    "AND gatb_accesstype = (SELECT gatt_id FROM _groupaccesstabletype WHERE gatt_uid = $3) "
    "RETURNING (SELECT gatt_name FROM _groupaccesstabletype WHERE gatt_uid = $3)", 3);
//-----------------------------------------------------------------------------
static std::string QI_GROUP_RIGHT_SPECIAL = PREPARE_QUERYN("INSERT INTO _groupaccessspecial(gasp_group, gasp_specialaccess) "
    "VALUES($1, (SELECT gast_id FROM _groupaccessspecialtype WHERE gast_uid = $2)) "
    "RETURNING (SELECT gast_name FROM _groupaccessspecialtype WHERE gast_uid = $2)", 2);
//-----------------------------------------------------------------------------
static std::string QD_GROUP_RIGHT_SPECIAL = PREPARE_QUERYN("DELETE FROM _groupaccessspecial "
    "WHERE gasp_group = $1 "
    "AND gasp_specialaccess = (SELECT gast_id FROM _groupaccessspecialtype WHERE gast_uid = $2) "
    "RETURNING (SELECT gast_name FROM _groupaccessspecialtype WHERE gast_uid = $2)", 2);
//-----------------------------------------------------------------------------
static std::string QI_COLUMN_SIZE = PREPARE_QUERYN("INSERT INTO _columnsize(clsz_user, clsz_tablename, clsz_fieldname, clsz_size) "
    "VALUES($1, $2, $3, $4)", 4);
//-----------------------------------------------------------------------------
static std::string QU_COLUMN_SIZE = PREPARE_QUERYN("UPDATE _columnsize SET "
    "clsz_size = $1 "
    "WHERE clsz_user = $2 "
    "AND clsz_tablename = $3 "
    "AND clsz_fieldname = $4", 4);
//-----------------------------------------------------------------------------
static std::string QU_SETTING = PREPARE_QUERYN("UPDATE _usersettings SET "
    "usst_value = $1 "
    "WHERE usst_user = $2 "
    "AND usst_setting = (SELECT stgs_id FROM _settings WHERE stgs_uid = $3)", 3);
//-----------------------------------------------------------------------------
static std::string QS_PERIOD = PREPARE_QUERY("SELECT prod_constant "
    "FROM period "
    "WHERE CURRENT_DATE BETWEEN prod_datestart AND prod_dateend");
//-----------------------------------------------------------------------------
static std::string QS_STOCK = PREPARE_QUERY("SELECT stck_id, stck_name "
    "FROM stock "
    "ORDER BY stck_name");
//-----------------------------------------------------------------------------
static std::string QS_STATEMENT = PREPARE_QUERYN("SELECT COUNT(*) "
    "FROM gasstationstatement "
    "WHERE gsts_implementationunload = $1", 1);
//-----------------------------------------------------------------------------
static std::string QI_STATEMENT = PREPARE_QUERYN("INSERT INTO gasstationstatement(gsts_implementationunload, gsts_stock, gsts_date, gsts_volumeincome) "
    "VALUES($1, $2, "
    "(SELECT impl_date FROM implementation WHERE impl_id = (SELECT iunl_implementation FROM implementationunload WHERE iunl_id = $1)), "
    "(SELECT iunl_weightnet * $3 FROM implementationunload WHERE iunl_id = $1))", 3);
//-----------------------------------------------------------------------------
static std::string QS_FILL_IN_BASED = PREPARE_QUERYN("SELECT "
    "COALESCE(gsts_balanceendchange, 0) AS gsts_balanceendchange, "
    "COALESCE(gsts_cashboxtotalpayment, 0) AS gsts_cashboxtotalpayment, "
    "COALESCE(gsts_cashboxtotalactually, 0) AS gsts_cashboxtotalactually, "
    "COALESCE(gsts_cashboxkkmtotal, 0) AS gsts_cashboxkkmtotal "
    "FROM gasstationstatement "
    "WHERE gsts_id = $1", 1);
//-----------------------------------------------------------------------------
static std::string QS_IMPLEMENTATION_UNLOAD = PREPARE_QUERYN("SELECT true AS is_load, ilod_implementation AS implementation_id, ilod_id AS id, impl_date AS date, ilod_cost AS cost "
    "FROM implementationload "
    "LEFT JOIN implementation ON ilod_implementation = impl_id "
    "WHERE ilod_counterparty = $1 "
    ""
    "UNION "
    ""
    "SELECT false AS is_load, iunl_implementation AS implementation_id, iunl_id AS id, impl_date AS date, iunl_cost AS cost "
    "FROM implementationunload "
    "LEFT JOIN implementation ON iunl_implementation = impl_id "
    "WHERE iunl_counterparty = $1 "
    "ORDER BY is_load, date DESC", 1);
//-----------------------------------------------------------------------------
static std::string QS_COUNTERPARTY_DEBT = PREPARE_QUERYN("SELECT get_counterparty_unload($1), get_counterparty_load($1), get_counterparty_entrollment($1), get_counterparty_move_wagon($1)", 1);
//-----------------------------------------------------------------------------
static std::string QS_USERS_CONSUMPTION = PREPARE_QUERY("SELECT usrs_id, usrs_fio, get_user_balance(usrs_id) "
    "FROM _users "
    "ORDER BY usrs_fio");
//-----------------------------------------------------------------------------
static std::string QS_USER_CONSUMPTION = PREPARE_QUERYN("SELECT cpmp_date, cpmp_sum, cpmp_note "
    "FROM consumption "
    "WHERE cpmp_user = $1 "
    "ORDER BY cpmp_date DESC", 1);
//-----------------------------------------------------------------------------
static std::string QS_USER_RETURN = PREPARE_QUERYN("SELECT rtrn_date, rtrn_sum, rtrn_note "
    "FROM return "
    "WHERE rtrn_user = $1 "
    "ORDER BY rtrn_date DESC", 1);
//-----------------------------------------------------------------------------
static std::string QS_TOTAL_BALANCE = PREPARE_QUERY("SELECT "
    "(SELECT COALESCE(sum(cmng_sum), 0) FROM coming) + "
    "(SELECT COALESCE(sum(cpen_sum), 0) FROM counterpartyenrollment WHERE cpen_sendtocoming) AS total_coming,"
    "(SELECT COALESCE(sum(cpmp_sum), 0) AS total_consumption FROM consumption)");
//-----------------------------------------------------------------------------
static std::string QS_BANK = PREPARE_QUERY("SELECT COUNT(*) "
    "FROM bank "
    "WHERE bank_date = :Date "
    "AND bank_incomingnumber = :IncomingNumber");
//-----------------------------------------------------------------------------
static std::string QI_BANK = PREPARE_QUERY("INSERT INTO bank(bank_date, bank_admission, bank_writeoff, bank_purposepayment, bank_counterparty, bank_checknumber, bank_operationtype, bank_incomingnumber, bank_incomingdate, bank_bankaccount, bank_comment) "
    "VALUES(:Date, :Admission, :WriteOff, :PurposePayment, :Counterparty, :CheckNumber, :OperationType, :IncomingNumber, :IncomingDate, :BankAccount, :Comment)");
//-----------------------------------------------------------------------------
static std::string QS_USER_BALANCE = PREPARE_QUERYN("SELECT get_user_balance($1)", 1);
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker()
    : ErrorString(STRING_NO_ERROR),
    IsBusy(false),
    IsRunning(false),
    IsFinished(false),
    CurrentMessage(nullptr),
    DBConnection(nullptr),
    qProtocol(nullptr)
{
    MapFunction[API_AUTH] = &ISTcpWorker::Auth;
    MapFunction[API_SLEEP] = &ISTcpWorker::Sleep;
    MapFunction[API_GET_META_DATA] = &ISTcpWorker::GetMetaData;
    MapFunction[API_GET_LAST_CLIENT] = &ISTcpWorker::GetLastClient;
    MapFunction[API_USER_PASSWORD_EXIST] = &ISTcpWorker::UserPasswordExist;
    MapFunction[API_USER_PASSWORD_CREATE] = &ISTcpWorker::UserPasswordCreate;
    MapFunction[API_USER_PASSWORD_EDIT] = &ISTcpWorker::UserPasswordEdit;
    MapFunction[API_USER_PASSWORD_RESET] = &ISTcpWorker::UserPasswordReset;
    MapFunction[API_USER_SETTINGS_RESET] = &ISTcpWorker::UserSettingsReset;
    MapFunction[API_GET_TABLE_DATA] = &ISTcpWorker::GetTableData;
    MapFunction[API_RECORD_GET_INFO] = &ISTcpWorker::RecordGetInfo;
    MapFunction[API_GET_SERVER_INFO] = &ISTcpWorker::GetServerInfo;
    MapFunction[API_GET_RECORD_VALUE] = &ISTcpWorker::GetRecordValue;
    MapFunction[API_RECORD_ADD] = &ISTcpWorker::RecordAdd;
    MapFunction[API_RECORD_GET] = &ISTcpWorker::RecordGet;
    MapFunction[API_RECORD_EDIT] = &ISTcpWorker::RecordEdit;
    MapFunction[API_RECORD_DELETE] = &ISTcpWorker::RecordDelete;
    MapFunction[API_FILE_STORAGE_ADD] = &ISTcpWorker::FileStorageAdd;
    MapFunction[API_FILE_STORAGE_COPY] = &ISTcpWorker::FileStorageCopy;
    MapFunction[API_FILE_STORAGE_GET] = &ISTcpWorker::FileStorageGet;
    MapFunction[API_DISCUSSION_ADD] = &ISTcpWorker::DiscussionAdd;
    MapFunction[API_DISCUSSION_EDIT] = &ISTcpWorker::DiscussionEdit;
    MapFunction[API_DISCUSSION_COPY] = &ISTcpWorker::DiscussionCopy;
    MapFunction[API_GET_NOTE_RECORD] = &ISTcpWorker::GetNoteRecord;
    MapFunction[API_SET_NOTE_RECORD] = &ISTcpWorker::SetNoteRecord;
    MapFunction[API_SEARCH_FULL_TEXT] = &ISTcpWorker::SearchFullText;
    MapFunction[API_GET_INTERNAL_LISTS] = &ISTcpWorker::GetInternalLists;
    MapFunction[API_GET_FAVORITE_NAMES] = &ISTcpWorker::GetFavoriteNames;
    MapFunction[API_RECORD_FAVORITE_ADD] = &ISTcpWorker::RecordFavoriteAdd;
    MapFunction[API_RECORD_FAVORITE_DELETE] = &ISTcpWorker::RecordFavoriteDelete;
    MapFunction[API_FAVORITES_DELETE] = &ISTcpWorker::FavoritesDelete;
    MapFunction[API_GET_HISTORY_LIST] = &ISTcpWorker::GetHistoryList;
    MapFunction[API_GET_FOREIGN_LIST] = &ISTcpWorker::GetForeignList;
    MapFunction[API_GET_GROUP_RIGHTS] = &ISTcpWorker::GetGroupRights;
    MapFunction[API_ORGANIZATION_FROM_INN] = &ISTcpWorker::OrganizationFromINN;
    MapFunction[API_GET_TABLE_QUERY] = &ISTcpWorker::GetTableQuery;
    MapFunction[API_GROUP_RIGHT_SUBSYSTEM_ADD] = &ISTcpWorker::GroupRightSubSystemAdd;
    MapFunction[API_GROUP_RIGHT_SUBSYSTEM_DELETE] = &ISTcpWorker::GroupRightSubSystemDelete;
    MapFunction[API_GROUP_RIGHT_TABLE_ADD] = &ISTcpWorker::GroupRightTableAdd;
    MapFunction[API_GROUP_RIGHT_TABLE_DELETE] = &ISTcpWorker::GroupRightTableDelete;
    MapFunction[API_GROUP_RIGHT_SPECIAL_ADD] = &ISTcpWorker::GroupRightSpecialAdd;
    MapFunction[API_GROUP_RIGHT_SPECIAL_DELETE] = &ISTcpWorker::GroupRightSpecialDelete;
    MapFunction[API_SAVE_META_DATA] = &ISTcpWorker::SaveMetaData;
    MapFunction[API_GET_RECORD_CALL] = &ISTcpWorker::GetRecordCall;

    if (ISConfigurations::Instance().Get().Name == "OilSphere")
    {
        MapFunction["OilSphere_PeriodContains"] = &ISTcpWorker::OilSphere_PeriodContains;
        MapFunction["OilSphere_GetStockList"] = &ISTcpWorker::OilSphere_GetStockList;
        MapFunction["OilSphere_StatementAdd"] = &ISTcpWorker::OilSphere_StatementAdd;
        MapFunction["OilSphere_GetGasStation"] = &ISTcpWorker::OilSphere_GetGasStation;
        MapFunction["OilSphere_GetDebtImplementation"] = &ISTcpWorker::OilSphere_GetDebtImplementation;
        MapFunction["OilSphere_GetDebtCounterparty"] = &ISTcpWorker::OilSphere_GetDebtCounterparty;
        MapFunction["OilSphere_GetUserConsumption"] = &ISTcpWorker::OilSphere_GetUserConsumption;
        MapFunction["OilSphere_LoadBanks"] = &ISTcpWorker::OilSphere_LoadBanks;
        MapFunction["OilSphere_GetUserBalance"] = &ISTcpWorker::OilSphere_GetUserBalance;
    }

    CRITICAL_SECTION_INIT(&CriticalSection);
    CRITICAL_SECTION_INIT(&CSRunning);
}
//-----------------------------------------------------------------------------
ISTcpWorker::~ISTcpWorker()
{
    CRITICAL_SECTION_DESTROY(&CriticalSection);
    CRITICAL_SECTION_DESTROY(&CSRunning);
    delete qProtocol;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetBusy()
{
    bool is_busy = false;
    CRITICAL_SECTION_LOCK(&CriticalSection);
    is_busy = IsBusy;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
    return is_busy;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetMessage(ISTcpMessage *TcpMessage)
{
    CRITICAL_SECTION_LOCK(&CriticalSection);
    CurrentMessage = TcpMessage;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Start()
{
    std::thread(&ISTcpWorker::Process, this).detach();
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Shutdown()
{
    //Останавливаем воркер
    SetRunning(false);

    //Ждём пока воркер не остановится
    while (true)
    {
        ISSleep(1);

        CRITICAL_SECTION_INIT(&CriticalSection);
        bool is_finished = IsFinished;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
        if (is_finished) //Воркер остановимся - выходим из цикла
        {
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetRunning(bool is_running)
{
    CRITICAL_SECTION_LOCK(&CSRunning);
    IsRunning = is_running;
    CRITICAL_SECTION_UNLOCK(&CSRunning);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRunning()
{
    bool is_running = false;
    CRITICAL_SECTION_LOCK(&CSRunning);
    is_running = IsRunning;
    CRITICAL_SECTION_UNLOCK(&CSRunning);
    return is_running;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Process()
{
    //Получаем параметры подключения к БД
    std::string DBHost = ISConfig::Instance().GetValueString("Database", "Host"),
        DBName = ISConfig::Instance().GetValueString("Database", "Name"),
        DBUser = ISConfig::Instance().GetValueString("Database", "User"),
        DBPassword = ISConfig::Instance().GetValueString("Database", "Password");
    unsigned short DBPort = ISConfig::Instance().GetValueUShort("Database", "Port");

    //Формируем имя подключения
    std::stringstream StringStream;
    StringStream << CURRENT_THREAD_ID();
    std::string DBConnectionName = StringStream.str();

    //Подключаемся к БД
    if (!ISDatabase::Instance().Connect(DBConnectionName, DBHost, DBPort, DBName, DBUser, DBPassword))
    {
        ISLOGGER_E(__CLASS__, "Not connected to database: %s", ISDatabase::Instance().GetErrorString().c_str());
        return;
    }

    //Получаем указатель на соединение
    DBConnection = ISDatabase::Instance().GetDB(DBConnectionName);

    //Создаём указатель на объект запроса протокола
    qProtocol = new ISQuery(DBConnection, QI_PROTOCOL);

    SetRunning(true);
    while (true)
    {
        ISSleep(1); //Немного поспим

        //Проверяем, не останавливали ли воркер. Если не запущен - выходим из потока
        if (!GetRunning())
        {
            break;
        }

        //Получаем текущее сообщение
        CRITICAL_SECTION_LOCK(&CriticalSection);
        ISTcpMessage *tcp_message = CurrentMessage;
        if (tcp_message)
        {
            IsBusy = true;
            CurrentMessage = nullptr;
        }
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

        //Если сообщения нет - переходим на следующую итерацию
        if (!tcp_message)
        {
            continue;
        }

        bool Result = false;
        uint64_t PerfomanceMsec = 0;
        ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->TcpClient->Socket);

        if (tcp_message->IsValid()) //Если сообщение валидное - переходим к выполнению
        {
            //Если запрос не авторизационный и клиент ещё не авторизовался - ошибка
            if (tcp_message->Type != API_AUTH && !tcp_message->TcpClient->Authorized)
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
        std::string LogText = ISAlgorithm::StringF("%s message \"%s\"\n"
            "Size: %d, Chunk: %d, Parse msec: %llu, Execute msec: %llu",
            (Result ? "Done" : "Failed"), tcp_message->Type.c_str(),
            tcp_message->Size, tcp_message->ChunkCount, tcp_message->MSecParse, PerfomanceMsec);
        if (!Result) //Запрос выполнен с ошибкой - устанавливаем текст ошибки в ответе и лог-сообщении, а так же очищаем потенциально не пустые параметры ответа
        {
            TcpAnswer->SetError(ErrorString);
            LogText.append("\nError string: " + ErrorString);
            ErrorString.clear();
        }

        //Удаляем сообщение, логируемся, добавляем ответ в очередь и освобождаем воркер
        delete tcp_message;
        Result ? ISLOGGER_I(__CLASS__, LogText.c_str()) : ISLOGGER_E(__CLASS__, LogText.c_str());
        ISTcpQueue::Instance().AddAnswer(TcpAnswer);

        CRITICAL_SECTION_LOCK(&CriticalSection);
        IsBusy = false;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);
    }

    //Установим флаг остановки
    CRITICAL_SECTION_LOCK(&CriticalSection);
    IsFinished = true;
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Ищем указатель на функцию и если нашли - выполняем
    auto It = MapFunction.find(TcpMessage->Type);
    if (It != MapFunction.end())
    {
        return (this->*It->second)(TcpMessage, TcpAnswer);
    }
    else //Функция не найдена
    {
        ErrorString = ISAlgorithm::StringF("Not support function \"%s\"", TcpMessage->Type.c_str());
    }
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNull(ISTcpMessage *TcpMessage, const char *ParameterName, rapidjson::Value &JsonValue)
{
    //Получаем константное имя параметра
    if (!TcpMessage->Parameters.HasMember(ParameterName))
    {
        ErrorString = ISAlgorithm::StringF("Parameter '%s' not exist", ParameterName);
        return false;
    }

    //Получаем значение
    JsonValue = TcpMessage->Parameters[ParameterName];

    //Проверяем, не пустое ли значение
    if (JsonValue.IsNull())
    {
        ErrorString = ISAlgorithm::StringF("Parameter '%s' is null", ParameterName);
        return false;
    }

    switch (JsonValue.GetType()) //Проверяем тип
    {
    case rapidjson::Type::kNullType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kObjectType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kArrayType:
        IS_ASSERT(false, "Not support type. Need of develop support");
        break;

    case rapidjson::Type::kStringType: //Строковый тип
        if (JsonValue.GetStringLength() == 0)
        {
            ErrorString = ISAlgorithm::StringF("Parameter '%s' is empty", ParameterName);
            return false;
        }
        break;

    default:
        break;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNullString(ISTcpMessage *TcpMessage, const char *ParameterName, std::string &String)
{
    rapidjson::Value JsonValue;
    if (!CheckIsNull(TcpMessage, ParameterName, JsonValue))
    {
        return false;
    }

    if (!JsonValue.IsString())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.ParameterNotString"), ParameterName);
        return false;
    }
    String = JsonValue.GetString();
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNullBool(ISTcpMessage *TcpMessage, const char *ParameterName, bool &Bool)
{
    rapidjson::Value JsonValue;
    if (!CheckIsNull(TcpMessage, ParameterName, JsonValue))
    {
        return false;
    }

    if (!JsonValue.IsBool())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.ParameterNotBool"), ParameterName);
        return false;
    }
    Bool = JsonValue.GetBool();
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNullUInt(ISTcpMessage *TcpMessage, const char *ParameterName, unsigned int &UInt)
{
    rapidjson::Value JsonValue;
    if (!CheckIsNull(TcpMessage, ParameterName, JsonValue))
    {
        return false;
    }

    if (!JsonValue.IsUint()) //Значение не является числовым
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.ParameterNotInt"), ParameterName);
        return false;
    }
    UInt = JsonValue.GetUint();
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNullDouble(ISTcpMessage *TcpMessage, const char *ParameterName, double &Double)
{
    rapidjson::Value JsonValue;
    if (!CheckIsNull(TcpMessage, ParameterName, JsonValue))
    {
        return false;
    }

    if (JsonValue.IsDouble())
    {
        Double = JsonValue.GetDouble();
    }
    else if (JsonValue.IsInt64())
    {
        Double = (double)JsonValue.GetInt64();
    }
    else
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.ParameterNotDouble"), ParameterName);
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::ErrorQuery(const std::string &LocalError, ISQuery &SqlQuery)
{
    ErrorString = LocalError;
    ISLOGGER_E(__CLASS__, "Sql query error:\n%s\n%s", SqlQuery.GetSqlText().c_str(), SqlQuery.GetErrorString().c_str());
    return false;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Protocol(unsigned int UserID, const char *ActionUID, const std::string &TableName, const std::string &TableLocalName, unsigned int ObjectID, const std::string &Information)
{
    qProtocol->BindUInt(UserID);
    TableName.empty() ? qProtocol->BindNull() : qProtocol->BindString(TableName);
    TableLocalName.empty() ? qProtocol->BindNull() : qProtocol->BindString(TableLocalName);
    qProtocol->BindUID(ActionUID);
    ObjectID == 0 ? qProtocol->BindNull() : qProtocol->BindUInt(ObjectID);
    Information.empty() ? qProtocol->BindNull() : qProtocol->BindString(Information);
    if (!qProtocol->Execute())
    {
        ISLOGGER_E(__CLASS__, "Not insert protocol: %s", qProtocol->GetErrorString().c_str());
    }
}
//-----------------------------------------------------------------------------
unsigned int ISTcpWorker::ExtractVersionFile(const std::string &FileName)
{
    unsigned int Version = 0;
    ISVectorString VectorString = ISAlgorithm::StringSplit(FileName, '_');
    if (VectorString.size() == 4) //Формат вроде валиден
    {
        std::string Temp = VectorString.back();
        size_t Pos = Temp.find('.');
        if (Pos != NPOS) //Нашли точку (разделитель расширения файла)
        {
            Temp.erase(Pos);
            if (!Temp.empty()) //На всякий случай
            {
                Version = std::stoi(Temp);
            }
        }
    }
    return Version;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetObjectName(PMetaTable *MetaTable, unsigned int ObjectID, std::string &ObjectName)
{
    //Если TitleName у мета-таблицы не заполнен - возвращаем идентификатор объекта
    if (MetaTable->TitleName.empty())
    {
        ObjectName = std::to_string(ObjectID);
        return true;
    }

    //Формируем запрос
    std::string QueryText = "SELECT ";
    ISVectorString VectorString = ISAlgorithm::StringSplit(MetaTable->TitleName, ';');
    for (const std::string &FieldName : VectorString) //Обходим список полей
    {
        PMetaForeign *MetaForeign = MetaTable->GetField(FieldName)->Foreign;
        QueryText += MetaForeign ?
            ("(" + ISTcpWorkerHelper::CreateSqlFromTitleName(MetaForeign, MetaTable->Alias, FieldName) + "),") :
            (MetaTable->Alias + '_' + FieldName + ',');
    }
    ISAlgorithm::StringChop(QueryText, 1);
    QueryText += "\nFROM " + MetaTable->Name;
    QueryText += "\nWHERE " + MetaTable->Alias + "_id = $1";

    //Запрашиваем имя
    ISQuery qSelectName(DBConnection, QueryText);
    qSelectName.SetShowLongQuery(false);
    qSelectName.BindUInt(ObjectID);
    if (!qSelectName.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.GetObjectName"), qSelectName);
    }

    if (!qSelectName.First()) //Запись не найдена
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.GetObjectName.NotExist"), ObjectID, MetaTable->Name.c_str());
        return false;
    }

    //Получаем запись и анализируем её
    for (int i = 0, c = qSelectName.GetResultColumnCount(); i < c; ++i)
    {
        switch (qSelectName.GetResultColumnType(i))
        {
        case DATEOID: ObjectName = qSelectName.ReadColumn_Date(i).ToString(); break;
        case TIMEOID: ObjectName = qSelectName.ReadColumn_Time(i).ToString(); break;
        case TIMESTAMPOID: ObjectName = qSelectName.ReadColumn_DateTime(i).ToString(); break;
        default:
            ObjectName = qSelectName.ReadColumn(i);
        }

        //На последней итерации пробел не добавляем
        if (i != c - 1)
        {
            ObjectName += ' ';
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
PMetaTable* ISTcpWorker::GetMetaTable(const std::string &TableName)
{
    PMetaTable *MetaTable = ISMetaData::Instance().GetTable(TableName);
    if (!MetaTable)
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.GetMetaTable"), TableName.c_str());
    }
    return MetaTable;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(unsigned int UserID, bool &Exist)
{
    ISQuery qSelectHashIsNull(DBConnection, QS_USER_PASSWORD_IS_NULL);
    qSelectHashIsNull.BindUInt(UserID);
    if (!qSelectHashIsNull.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.CheckExistUserPassword"), qSelectHashIsNull);
    }

    if (!qSelectHashIsNull.First()) //Не удалось перейти на первую строку, т.к. пользователя с таким UserID не существует
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.UserNotExist"), UserID);
        return false;
    }
    Exist = qSelectHashIsNull.ReadColumn_Bool(0);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserIsSystem(unsigned int UserID, bool &IsSystem)
{
    ISQuery qSelectIsSystem(DBConnection, QS_USER_IS_SYSTEM);
    qSelectIsSystem.BindUInt(UserID);
    if (!qSelectIsSystem.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.CheckUserIsSystem"), qSelectIsSystem);
    }

    if (!qSelectIsSystem.First())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.UserNotExist"), UserID);
        return false;
    }
    IsSystem = qSelectIsSystem.ReadColumn_Bool(0);
    return true;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::RegisterOilSphere()
{

}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Проверяем, не авторизаван ли уже клиент. Если авторизован - выходим с ошибкой
    if (TcpMessage->TcpClient->Authorized)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.AlreadyAuthorized");
        return false;
    }

    //Проверяем и получаем хэш
    std::string Hash;
    if (!CheckIsNullString(TcpMessage, "Hash", Hash))
    {
        return false;
    }

    //Проверяем размер хэша
    size_t HashSize = Hash.size();
    if (HashSize != CARAT_HASH_SIZE)
    {
        ErrorString = ISAlgorithm::StringF("Invalid hash size %d", HashSize);
        return false;
    }

    //Проверяем валидность хэша
    for (const char Char : Hash)
    {
        int ASCII = (int)Char; //Преобразовываем текущий символ в ASCII-код
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
        ISQuery qSelectHash(DBConnection, QS_USERS_HASH);
        if (!qSelectHash.Execute()) //Ошибка запроса
        {
            return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectHash"), qSelectHash);
        }

        //Если запрос ничего не вернул, значит в БД нет ни одного пользователя
        if (qSelectHash.GetResultRowCount() == 0)
        {
            ErrorString = LANG("Carat.Error.Query.Auth.NoUsers");
            return false;
        }

        //Ищем пользователя
        bool IsFound = false;
        while (qSelectHash.Next())
        {
            //Получаем хэш и соль
            std::string CurrentHash = qSelectHash.ReadColumn_String(0),
                CurrentSalt = qSelectHash.ReadColumn_String(1);

            //Солим присланный хэш текущей солью
            std::string HashResult = ISAlgorithm::SaltPassword(Hash, CurrentSalt);
            IsFound = HashResult == CurrentHash;
            if (IsFound) //Нашли
            {
                Hash = HashResult;
                break;
            }
        }

        if (!IsFound)
        {
            ErrorString = LANG("Carat.Error.Query.Auth.InvalidLoginOrPassword");
            return false;
        }
    }

    //Проверка пользователя
    ISQuery qSelectAuth(DBConnection, QS_USER_AUTH);
    qSelectAuth.BindUID(CONST_UID_SETTINGS_DATABASE);
    qSelectAuth.BindString(Hash);
    if (!qSelectAuth.ExecuteFirst()) //Запрос выполнен с ошибкой
    {
        return ErrorQuery(LANG("Carat.Error.Query.Auth.SelectLogin"), qSelectAuth);
    }

    unsigned int UserID = qSelectAuth.ReadColumn_UInt(0);
    bool UserSystem = qSelectAuth.ReadColumn_Bool(1);
    const char *UserFIO = qSelectAuth.ReadColumn(2);
    unsigned int GroupID = qSelectAuth.ReadColumn_UInt(3);
    bool GroupFullAccess = qSelectAuth.ReadColumn_Bool(4);

    //Доступ к БД запрещен
    if (!qSelectAuth.ReadColumn_Bool(9) && !UserSystem)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.ConnectionBan");
        return false;
    }

    //Если у пользователя нет права доступа
    if (!qSelectAuth.ReadColumn_Bool(5))
    {
        ErrorString = LANG("Carat.Error.Query.Auth.LoginNoAccess");
        return false;
    }

    //Проверка наличия привязки не системного пользователя к группе
    if (!UserSystem && GroupID == 0)
    {
        ErrorString = LANG("Carat.Error.Query.Auth.LoginLinkGroup");
        return false;
    }

    //Если для пользователя настроено ограничение срока действия учётной записи
    if (qSelectAuth.ReadColumn_Bool(6))
    {
        ISDate DateStart = qSelectAuth.ReadColumn_Date(7),
            DateEnd = qSelectAuth.ReadColumn_Date(8);
        if (!DateStart.IsNull() && !DateEnd.IsNull()) //Если дата начала и окончания срока действия установлена
        {
            ISDate CurrentDate = ISDate::CurrentDate();
            if (CurrentDate < DateStart) //Срок действия учётной записи ещё не начался
            {
                ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.Auth.LoginLifetimeNotStarted"), DateStart.ToString().c_str());
                return false;
            }
            else if (CurrentDate > DateEnd) //Срок действия учётной записи закончился
            {
                ErrorString = LANG("Carat.Error.Query.Auth.LoginLifetimeEnded");
                return false;
            }
        }
    }

    //Проверяем версию клиента
    bool IsNeedUpdate = false; //По умолчанию флаг обновления должен быть false (вдруг клиент отправил невалидную версию)
    unsigned int VersionLast = 0;
    if (TcpMessage->Parameters.HasMember("Version")) //Если версия указана
    {
        //Получаем версию клиента
        unsigned int VersionClient = 0;
        if (!CheckIsNullUInt(TcpMessage, "Version", VersionClient))
        {
            return false;
        }

        //Получаем директорию с обновлениями
        std::string UpdateClientDir = ISConfig::Instance().GetValueString("Other", "UpdateClientDir");
        if (!UpdateClientDir.empty()) //Если директория настроена - идём дальше
        {
            //Проверяем существование директории с обновлениями
            if (!ISAlgorithm::DirExist(UpdateClientDir))
            {
                ISLOGGER_W(__CLASS__, "Folder \"%s\" not exist. Check config parameter \"Other\\UpdateClientDir\"", UpdateClientDir.c_str());
                ErrorString = LANG("Carat.Error.Query.Auth.UpdatePathNotExist");
                return false;
            }

            //Получаем список файлов и проверяем его на пустоту
            std::vector<ISFileInfo> VectorFiles = ISAlgorithm::DirFiles(false, UpdateClientDir, ErrorString,
                ISNamespace::DirFileSorting::EditDate, ISNamespace::SortingOrder::Descending);
            if (!VectorFiles.empty()) //Если обновления есть - ищем последнюю версию
            {
                std::string FilePath = VectorFiles.front().Path;
                VersionLast = ExtractVersionFile(FilePath);
                if (VersionLast > 0)
                {
                    IsNeedUpdate = VersionLast > VersionClient;
                }
                else
                {
                    ISLOGGER_W(__CLASS__, "Invalid format update file name: %s", +FilePath.c_str());
                }
            }
            //Обновлений нет - идём дальше
        }
        else //Директория не настроена - логируем предупреждение и идём дальше
        {
            ISLOGGER_W(__CLASS__, "Not setting directory updates");
        }
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Проверяем, не подключен ли уже это пользователь
    //Если пользователь уже подключен - вытаскиваем информацию о подключении
    if (ISTcpClients::Instance().IsExistUserID(UserID))
    {
        ISTcpClientInfo TcpClientInfo = ISTcpClients::Instance().GetInfo(UserID);
        const char *DTConnected = TcpClientInfo.DTConnected.ToString().c_str(),
            *IPAddress = TcpClientInfo.IPAddress.c_str();

        rapidjson::Value AlreadyConnectedObject(rapidjson::Type::kObjectType);
        AlreadyConnectedObject.AddMember("DateTime", JSON_STRINGA(DTConnected, Allocator), Allocator);
        AlreadyConnectedObject.AddMember("IPAddress", JSON_STRINGA(IPAddress, Allocator), Allocator);
        TcpAnswer->Parameters.AddMember("AlreadyConnected", rapidjson::Value(AlreadyConnectedObject, Allocator), Allocator);
    }

    //Устанавливаем флаги клиенту
    TcpMessage->TcpClient->Authorized = true;
    TcpMessage->TcpClient->UserID = UserID;
    TcpMessage->TcpClient->UserGroupID = GroupID;
    TcpMessage->TcpClient->UserSystem = UserSystem;

    //Отдаём информацию о пользователе и конфигурации
    TcpAnswer->Parameters.AddMember("UserID", UserID, Allocator);
    TcpAnswer->Parameters.AddMember("UserIsSystem", UserSystem, Allocator);
    TcpAnswer->Parameters.AddMember("UserFIO", JSON_STRING(UserFIO), Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupID", GroupID, Allocator);
    TcpAnswer->Parameters.AddMember("UserGroupFullAccess", GroupFullAccess, Allocator);

    //Информация о сервере
    rapidjson::Value ServerObject(rapidjson::Type::kObjectType);
    ServerObject.AddMember("Version", JSON_NULL, Allocator);
    TcpAnswer->Parameters.AddMember("Server", rapidjson::Value(ServerObject, Allocator), Allocator);

    //Информация об обновлении клиента
    rapidjson::Value UpdateClientObject(rapidjson::Type::kObjectType);
    UpdateClientObject.AddMember("IsNeed", IsNeedUpdate, Allocator);
    UpdateClientObject.AddMember("NewVersion", VersionLast > 0 ? rapidjson::Value(VersionLast) : JSON_NULL, Allocator);
    TcpAnswer->Parameters.AddMember("UpdateClient", rapidjson::Value(UpdateClientObject, Allocator), Allocator);

    ISDate DateExpired = ISConfigurations::Instance().Get().DateExpired;

    //Информация о конфигурации
    rapidjson::Value ConfigurationObject(rapidjson::Type::kObjectType);
    ConfigurationObject.AddMember("Name", JSON_STRING(ISConfigurations::Instance().Get().Name.c_str()), Allocator);
    ConfigurationObject.AddMember("UID", JSON_STRING(ISConfigurations::Instance().Get().UID.c_str()), Allocator);
    ConfigurationObject.AddMember("LocalName", JSON_STRING(ISConfigurations::Instance().Get().LocalName.c_str()), Allocator);
    ConfigurationObject.AddMember("DesktopForm", JSON_STRING(ISConfigurations::Instance().Get().DesktopForm.c_str()), Allocator);
    ConfigurationObject.AddMember("DateExpired", DateExpired.IsNull() ? JSON_NULL : JSON_STRINGA(DateExpired.ToString().c_str(), Allocator), Allocator);
    ConfigurationObject.AddMember("LogoName", JSON_STRING(ISConfigurations::Instance().Get().LogoName.c_str()), Allocator);
    TcpAnswer->Parameters.AddMember("Configuration", rapidjson::Value(ConfigurationObject, Allocator), Allocator);

    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_ENTER_APPLICATION);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int Timeout = 0;
    if (!CheckIsNullUInt(TcpMessage, "Timeout", Timeout))
    {
        return false;
    }

    if (Timeout == 0) //Значение равно нулю
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.Sleep.TimeoutValueIsNull"));
        return false;
    }
    ISSleep(Timeout);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Получаем настройки БД
    rapidjson::Value SettingsDBObject(rapidjson::Type::kObjectType);
    ISQuery qSelectSettingsDB(DBConnection, QS_SETTINGS_DATABASE);
    qSelectSettingsDB.BindUID(CONST_UID_SETTINGS_DATABASE);
    if (qSelectSettingsDB.ExecuteFirst())
    {
        if (qSelectSettingsDB.GetResultRowCount() == 0) //Если настроек в БД нет - ошибка
        {
            ErrorString = LANG("Carat.Error.Query.GetMetaData.SettingsDBNotActive");
            return false;
        }
        SettingsDBObject.AddMember("ID", qSelectSettingsDB.ReadColumn_UInt(0), Allocator);
        SettingsDBObject.AddMember("UserAccessDatabase", qSelectSettingsDB.ReadColumn_Bool(1), Allocator);
        SettingsDBObject.AddMember("NumberSimbolsAfterComma", qSelectSettingsDB.ReadColumn_Int(2), Allocator);
        SettingsDBObject.AddMember("StorageFileMaxSize", qSelectSettingsDB.ReadColumn_Int(3), Allocator);
        SettingsDBObject.AddMember("TCPMessageID", (uint64_t)qSelectSettingsDB.ReadColumn_UInt64(4), Allocator);
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsDB"), qSelectSettingsDB);
    }

    //Получаем права на таблицы
    rapidjson::Value AccessTablesObject(rapidjson::Type::kObjectType);
    ISQuery qSelectAccessTables(DBConnection, QS_GROUP_ACCESS_TABLE);
    qSelectAccessTables.BindUInt(TcpMessage->TcpClient->UserGroupID);
    if (qSelectAccessTables.Execute())
    {
        while (qSelectAccessTables.Next())
        {
            const char *TableName = qSelectAccessTables.ReadColumn(0),
                *AccessUID = qSelectAccessTables.ReadColumn(1);
            if (AccessTablesObject.HasMember(TableName))
            {
                AccessTablesObject[JSON_STRING(TableName)].PushBack(JSON_STRING(AccessUID), Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(JSON_STRING(AccessUID), Allocator);
                AccessTablesObject.AddMember(JSON_STRING(TableName), TempArray, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessTable"), qSelectAccessTables);
    }

    //Получаем специальные права
    rapidjson::Value AccessSpecialObject(rapidjson::Type::kArrayType);
    ISQuery qSelectAccessSpecial(DBConnection, QS_GROUP_ACCESS_SPECIAL);
    qSelectAccessSpecial.BindUInt(TcpMessage->TcpClient->UserGroupID);
    if (qSelectAccessSpecial.Execute())
    {
        while (qSelectAccessSpecial.Next())
        {
            const char *UID = qSelectAccessSpecial.ReadColumn(0);
            AccessSpecialObject.PushBack(JSON_STRING(UID), Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.UserGroupAccessSpecial"), qSelectAccessSpecial);
    }

    //Получаем системы и подсистемы
    rapidjson::Value SystemSubSystemArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSystems(DBConnection, QS_SYSTEM_SUBSYSTEM);
    qSelectSystems.BindUInt(TcpMessage->TcpClient->UserID);
    qSelectSystems.BindBool(TcpMessage->TcpClient->UserSystem);
    if (qSelectSystems.Execute())
    {
        const char *CurrentSystemUID = nullptr;
        while (qSelectSystems.Next())
        {
            //Если доступа к подсистеме нет, переходим к следующей
            if (!qSelectSystems.ReadColumn_Bool(0))
            {
                continue;
            }

            const char *SystemUID = qSelectSystems.ReadColumn(1);
            if (!CurrentSystemUID || strcmp(SystemUID, CurrentSystemUID) != 0)
            {
                CurrentSystemUID = SystemUID;
                const char *SystemLocalName = qSelectSystems.ReadColumn(3),
                    *SystemIcon = qSelectSystems.ReadColumn(4),
                    *SystemHint = qSelectSystems.ReadColumn(5);

                rapidjson::Value SystemObject(rapidjson::Type::kObjectType);
                SystemObject.AddMember("IsSystem", qSelectSystems.ReadColumn_Bool(2), Allocator);
                SystemObject.AddMember("UID", JSON_STRINGA(SystemUID, Allocator), Allocator);
                SystemObject.AddMember("Local", JSON_STRINGA(SystemLocalName, Allocator), Allocator);
                SystemObject.AddMember("Icon", strlen(SystemIcon) > 0 ? JSON_STRINGA(SystemIcon, Allocator) : JSON_NULL, Allocator);
                SystemObject.AddMember("Hint", strlen(SystemHint) > 0 ? JSON_STRINGA(SystemHint, Allocator) : JSON_NULL, Allocator);
                SystemObject.AddMember("SubSystems", rapidjson::Value(rapidjson::Type::kArrayType), Allocator);
                SystemSubSystemArray.PushBack(SystemObject, Allocator);
            }

            const char *SubSystemUID = qSelectSystems.ReadColumn(6),
                *SubSystemLocalName = qSelectSystems.ReadColumn(7),
                *SubSystemIcon = qSelectSystems.ReadColumn(8),
                *SubSystemClassName = qSelectSystems.ReadColumn(9),
                *SubSystemTableName = qSelectSystems.ReadColumn(10),
                *SubSystemHint = qSelectSystems.ReadColumn(11);

            rapidjson::Value SubSystemObject(rapidjson::Type::kObjectType);
            SubSystemObject.AddMember("UID", JSON_STRINGA(SubSystemUID, Allocator), Allocator);
            SubSystemObject.AddMember("Local", JSON_STRINGA(SubSystemLocalName, Allocator), Allocator);
            SubSystemObject.AddMember("Icon", strlen(SubSystemIcon) > 0 ? JSON_STRINGA(SubSystemIcon, Allocator) : JSON_NULL, Allocator);
            SubSystemObject.AddMember("Class", strlen(SubSystemClassName) > 0 ? JSON_STRINGA(SubSystemClassName, Allocator) : JSON_NULL, Allocator);
            SubSystemObject.AddMember("Table", strlen(SubSystemTableName) > 0 ? JSON_STRINGA(SubSystemTableName, Allocator) : JSON_NULL, Allocator);
            SubSystemObject.AddMember("Hint", strlen(SubSystemHint) > 0 ? JSON_STRINGA(SubSystemHint, Allocator) : JSON_NULL, Allocator);
            SystemSubSystemArray[SystemSubSystemArray.Size() - 1][JSON_STRING("SubSystems")].PushBack(SubSystemObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Systems"), qSelectSystems);
    }

    //Получаем избранные объекты
    rapidjson::Value FavoriteObject(rapidjson::Type::kObjectType);
    ISQuery qSelectFavorite(DBConnection, QS_FAVORITE);
    qSelectFavorite.BindUInt(TcpMessage->TcpClient->UserID);
    if (qSelectFavorite.Execute())
    {
        while (qSelectFavorite.Next())
        {
            const char *TableName = qSelectFavorite.ReadColumn(0);
            unsigned int ObjectID = qSelectFavorite.ReadColumn_UInt(1);

            if (FavoriteObject.HasMember(TableName))
            {
                FavoriteObject[rapidjson::Value(TableName, (rapidjson::SizeType)strlen(TableName), Allocator)].PushBack(ObjectID, Allocator);
            }
            else
            {
                rapidjson::Value TempArray(rapidjson::Type::kArrayType);
                TempArray.PushBack(ObjectID, Allocator);
                FavoriteObject.AddMember(rapidjson::Value(TableName, (rapidjson::SizeType)strlen(TableName), Allocator), TempArray, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Favorite"), qSelectFavorite);
    }

    //Получаем размеры полей
    rapidjson::Value ColumnSizeObject(rapidjson::Type::kObjectType);
    ISQuery qSelectColumnSize(DBConnection, QS_COLUMN_SIZE);
    qSelectColumnSize.BindUInt(TcpMessage->TcpClient->UserID);
    if (qSelectColumnSize.Execute())
    {
        while (qSelectColumnSize.Next())
        {
            const char *TableName = qSelectColumnSize.ReadColumn(0),
                *FieldName = qSelectColumnSize.ReadColumn(1);
            unsigned int Size = qSelectColumnSize.ReadColumn_UInt(2);

            rapidjson::Value VTableName(TableName, (rapidjson::SizeType)strlen(TableName), Allocator),
                VFieldName(FieldName, (rapidjson::SizeType)strlen(FieldName), Allocator);

            if (ColumnSizeObject.HasMember(TableName))
            {
                ColumnSizeObject[VTableName].AddMember(VFieldName, Size, Allocator);
            }
            else
            {
                rapidjson::Value TempObject(rapidjson::Type::kObjectType);
                TempObject.AddMember(VFieldName, Size, Allocator);
                ColumnSizeObject.AddMember(VTableName, TempObject, Allocator);
            }
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.ColumnSize"), qSelectColumnSize);
    }

    //Получаем пользовательские настройки
    rapidjson::Value SettingGroupsArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSettings(DBConnection, QS_SETTINGS);
    qSelectSettings.BindUInt(TcpMessage->TcpClient->UserID);
    if (qSelectSettings.Execute())
    {
        const char *CurrentGroupUID = nullptr;
        while (qSelectSettings.Next())
        {
            const char *GroupUID = qSelectSettings.ReadColumn(0),
                *SettingUID = qSelectSettings.ReadColumn(5);
            if (strlen(SettingUID) == 0) //Если идентификатор настройки пустой - значит по этой группе настроек не числится
            {
                continue;
            }

            if (!CurrentGroupUID || strcmp(CurrentGroupUID, GroupUID) != 0)
            {
                CurrentGroupUID = GroupUID;
                const char *GroupName = qSelectSettings.ReadColumn(1),
                    *GroupLocalName = qSelectSettings.ReadColumn(2),
                    *GroupIconName = qSelectSettings.ReadColumn(3),
                    *GroupHint = qSelectSettings.ReadColumn(4);

                rapidjson::Value SettingGroupObject(rapidjson::Type::kObjectType);
                SettingGroupObject.AddMember("UID", JSON_STRINGA(GroupUID, Allocator), Allocator);
                SettingGroupObject.AddMember("Name", JSON_STRINGA(GroupName, Allocator), Allocator);
                SettingGroupObject.AddMember("Local", JSON_STRINGA(GroupLocalName, Allocator), Allocator);
                SettingGroupObject.AddMember("Icon", JSON_STRINGA(GroupIconName, Allocator), Allocator);
                SettingGroupObject.AddMember("Hint", strlen(GroupHint) > 0 ? JSON_STRINGA(GroupHint, Allocator) : JSON_NULL, Allocator);
                SettingGroupObject.AddMember("Settings", rapidjson::Value(rapidjson::Type::kArrayType), Allocator);
                SettingGroupsArray.PushBack(SettingGroupObject, Allocator);
            }

            const char *SettingName = qSelectSettings.ReadColumn(6),
                *SettingType = qSelectSettings.ReadColumn(7),
                *SettingWidget = qSelectSettings.ReadColumn(8),
                *SettingLocalName = qSelectSettings.ReadColumn(9),
                *SettingHint = qSelectSettings.ReadColumn(10),
                *SettingDefault = qSelectSettings.ReadColumn(11),
                *SettingValue = nullptr;

            rapidjson::Value SettingObject(rapidjson::Type::kObjectType);
            SettingObject.AddMember("UID", JSON_STRINGA(SettingUID, Allocator), Allocator);
            SettingObject.AddMember("Name", JSON_STRINGA(SettingName, Allocator), Allocator);
            SettingObject.AddMember("Type", JSON_STRINGA(SettingType, Allocator), Allocator);
            SettingObject.AddMember("Widget", strlen(SettingWidget) > 0 ? JSON_STRINGA(SettingWidget, Allocator) : JSON_NULL, Allocator);
            SettingObject.AddMember("Local", JSON_STRINGA(SettingLocalName, Allocator), Allocator);
            SettingObject.AddMember("Hint", strlen(SettingHint) > 0 ? JSON_STRINGA(SettingHint, Allocator) : JSON_NULL, Allocator);
            SettingObject.AddMember("Default", strlen(SettingDefault) > 0 ? JSON_STRINGA(SettingDefault, Allocator) : JSON_NULL, Allocator);

            if (qSelectSettings.ReadColumn_Int(13) != 0)  //Если такая настройка у пользователя уже есть - получаем её значение
            {
                SettingValue = qSelectSettings.ReadColumn(12);
            }
            else //Такой настройки у пользователя нет - добавляем со значением по умолчанию
            {
                ISQuery qInsertSetting(DBConnection, QI_USER_SETTING);
                qInsertSetting.BindUInt(TcpMessage->TcpClient->UserID);
                qInsertSetting.BindUID(SettingUID);
                qInsertSetting.BindString(SettingDefault);
                if (!qInsertSetting.Execute())
                {
                    ErrorString = "Error inserting new user setting: " + qInsertSetting.GetErrorString();
                    return false;
                }
                SettingValue = SettingDefault;
            }
            SettingObject.AddMember("Value", JSON_STRINGA(SettingValue, Allocator), Allocator);
            SettingGroupsArray[SettingGroupsArray.Size() - 1][JSON_STRING("Settings")].PushBack(SettingObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.SettingsGroup"), qSelectSettings);
    }

    //Получаем параграфы
    rapidjson::Value ParagraphArray(rapidjson::Type::kArrayType);
    ISQuery qSelectParagraph(DBConnection, QS_PARAGRAPH);
    if (qSelectParagraph.Execute())
    {
        while (qSelectParagraph.Next())
        {
            const char *UID = qSelectParagraph.ReadColumn(0),
                *Name = qSelectParagraph.ReadColumn(1),
                *LocalName = qSelectParagraph.ReadColumn(2),
                *ToolTip = qSelectParagraph.ReadColumn(3),
                *IconName = qSelectParagraph.ReadColumn(4),
                *ClassName = qSelectParagraph.ReadColumn(5);

            rapidjson::Value ParagraphObject(rapidjson::Type::kObjectType);
            ParagraphObject.AddMember("UID", JSON_STRING(UID), Allocator);
            ParagraphObject.AddMember("Name", JSON_STRING(Name), Allocator);
            ParagraphObject.AddMember("Local", JSON_STRING(LocalName), Allocator);
            ParagraphObject.AddMember("ToolTip", JSON_STRING(ToolTip), Allocator);
            ParagraphObject.AddMember("Icon", JSON_STRING(IconName), Allocator);
            ParagraphObject.AddMember("Class", JSON_STRING(ClassName), Allocator);
            ParagraphArray.PushBack(ParagraphObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.Paragraph"), qSelectParagraph);
    }

    //Получаем приоритеты задач
    rapidjson::Value TaskPriorityArray(rapidjson::Type::kArrayType);
    ISQuery qSelectTaskPriority(DBConnection, QS_TASK_PRIORITY);
    if (qSelectTaskPriority.Execute())
    {
        while (qSelectTaskPriority.Next())
        {
            unsigned int ID = qSelectTaskPriority.ReadColumn_UInt(0);
            const char *LocalName = qSelectTaskPriority.ReadColumn(1),
                *ToolTip = qSelectTaskPriority.ReadColumn(2),
                *StyleSheet = qSelectTaskPriority.ReadColumn(3),
                *IconName = qSelectTaskPriority.ReadColumn(4);
            rapidjson::Value TaskPriorityObject(rapidjson::Type::kObjectType);
            TaskPriorityObject.AddMember("ID", ID, Allocator);
            TaskPriorityObject.AddMember("Local", JSON_STRING(LocalName), Allocator);
            TaskPriorityObject.AddMember("ToolTip", JSON_STRING(ToolTip), Allocator);
            TaskPriorityObject.AddMember("Style", JSON_STRING(StyleSheet), Allocator);
            TaskPriorityObject.AddMember("Icon", JSON_STRING(IconName), Allocator);
            TaskPriorityArray.PushBack(TaskPriorityObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetMetaData.TaskPriority"), qSelectTaskPriority);
    }

    //Читаем мета-данные
    rapidjson::Value MetaDataArray(rapidjson::Type::kArrayType);
    const ISVectorString &VectorString = ISMetaData::Instance().GetVectorXSN();
    for (const std::string &FileName : VectorString)
    {
        std::string ContentBase64 = ISAlgorithm::Base64Encode(ISResourcer::Instance().GetFile(FileName), ErrorString);
        if (ContentBase64.empty())
        {
            return false;
        }
        MetaDataArray.PushBack(rapidjson::Value(ContentBase64.c_str(), (rapidjson::SizeType)ContentBase64.size(), Allocator), Allocator);
    }

    TcpAnswer->Parameters.AddMember("SettingsDB", SettingsDBObject, Allocator);
    TcpAnswer->Parameters.AddMember("AccessTables", AccessTablesObject, Allocator);
    TcpAnswer->Parameters.AddMember("AccessSpecial", AccessSpecialObject, Allocator);
    TcpAnswer->Parameters.AddMember("Systems", SystemSubSystemArray, Allocator);
    TcpAnswer->Parameters.AddMember("Favorite", FavoriteObject, Allocator);
    TcpAnswer->Parameters.AddMember("ColumnSize", ColumnSizeObject, Allocator);
    TcpAnswer->Parameters.AddMember("SettingGroups", SettingGroupsArray, Allocator);
    TcpAnswer->Parameters.AddMember("Paragraphs", ParagraphArray, Allocator);
    TcpAnswer->Parameters.AddMember("TaskPriority", TaskPriorityArray, Allocator);
    TcpAnswer->Parameters.AddMember("MetaData", MetaDataArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Получаем отсортированный (по дате) список файлов
    //в настроенной директории и проверяем его на пустоту
    std::vector<ISFileInfo> VectorFiles = ISAlgorithm::DirFiles(false, ISConfig::Instance().GetValueString("Other", "UpdateClientDir"),
        ISNamespace::DirFileSorting::EditDate, ISNamespace::SortingOrder::Descending);
    if (VectorFiles.empty()) //Если обновлений нет - выходим
    {
        TcpAnswer->Parameters.AddMember("Found", false, Allocator);
        return true;
    }
    const ISFileInfo &FileInfo = VectorFiles.front();

    //Открываем файл
    const char *FilePath = FileInfo.Path.c_str();
    long FileSize = 0;
    unsigned char *FileData = ISAlgorithm::ReadFile(FilePath, "rb", FileSize, ErrorString);
    if (!FileData)
    {
        return false;
    }

    //Кодируем в base64, очищаем память и проверяем результат кодирования
    std::string Base64 = ISAlgorithm::Base64Encode(FileData, (size_t)FileSize, ErrorString);
    free(FileData);
    if (Base64.empty())
    {
        return false;
    }

    //Протоколируем, конвертируем в base64 и отдаём
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_GET_UPDATE_CLIENT);
    TcpAnswer->Parameters.AddMember("FileName", JSON_STRINGA(FileInfo.Name.c_str(), Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("Data", JSON_STRINGA(Base64.c_str(), Allocator), Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Получаем идентификатор пользователя
    unsigned int UserID = 0;
    if (!CheckIsNullUInt(TcpMessage, "UserID", UserID))
    {
        return false;
    }

    //Проверяем наличие пароля
    bool Exist = true;
    if (!UserPasswordExist(UserID, Exist)) //Не удалось проверить наличие пароля
    {
        return false;
    }
    TcpAnswer->Parameters.AddMember("IsExist", Exist, TcpAnswer->Parameters.GetAllocator());
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    std::string Hash;
    unsigned int UserID = 0;
    if (!CheckIsNullString(TcpMessage, "Hash", Hash) ||
        !CheckIsNullUInt(TcpMessage, "UserID", UserID))
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
    std::string Salt;
    if (!ISAlgorithm::GenerateSalt(Salt, ErrorString)) //Ошибка генерации
    {
        ErrorString = LANG("Carat.Error.Query.UserPasswordCreate.GenerateSalt");
        return false;
    }
    std::string HashResult = ISAlgorithm::SaltPassword(Hash, Salt);

    //Устанавливаем пароль
    ISQuery qUpdateHash(DBConnection, QU_USER_HASH);
    qUpdateHash.BindString(HashResult);
    qUpdateHash.BindString(Salt);
    qUpdateHash.BindUInt(UserID);
    if (!qUpdateHash.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordCreate.UpdateHash"), qUpdateHash);
    }
    qUpdateHash.First(); //Проверка не требуется

    //Фиксируем создание пароля
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_PASSWORD_CREATE, "_Users", ISMetaData::Instance().GetTable("_Users")->LocalListName, UserID, qUpdateHash.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int UserID = 0;
    std::string HashOld, Hash;
    if (!CheckIsNullUInt(TcpMessage, "UserID", UserID) ||
        !CheckIsNullString(TcpMessage, "HashOld", HashOld) ||
        !CheckIsNullString(TcpMessage, "Hash", Hash))
    {
        return false;
    }

    //Получаем текущий хэш и соль пользователя
    ISQuery qSelectHash(DBConnection, QS_USER_PASSWORD);
    qSelectHash.BindUInt(UserID);
    if (!qSelectHash.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.SelectHash"), qSelectHash);
    }

    if (!qSelectHash.First()) //Пользователя с таким UserID нет в БД
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.UserPasswordEdit.UserNotExist"), UserID);
        return false;
    }

    //Текущие хэш и соль
    std::string CurrentHash = qSelectHash.ReadColumn_String(0),
        CurrentSalt = qSelectHash.ReadColumn_String(1);

    //Солим присланный хэш и проверяем. Если подсоленый хэш не соответствует тому что в БД - значит вводили неправильный текущий пароль (или логин) - ошибка
    if (ISAlgorithm::SaltPassword(HashOld, CurrentSalt) != CurrentHash)
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
    std::string HashResult = ISAlgorithm::SaltPassword(Hash, CurrentSalt);

    //Обновляем пароль
    ISQuery qUpdateHash(DBConnection, QU_USER_HASH);
    qUpdateHash.BindString(HashResult);
    qUpdateHash.BindString(CurrentSalt);
    qUpdateHash.BindUInt(UserID);
    if (!qUpdateHash.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordEdit.UpdateHash"), qUpdateHash);
    }
    qUpdateHash.First(); //Проверка не требуется

    //Фиксируем изменение пароля
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE, "_Users", ISMetaData::Instance().GetTable("_Users")->LocalListName, UserID, qUpdateHash.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int UserID = 0;
    if (!CheckIsNullUInt(TcpMessage, "UserID", UserID))
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
    ISQuery qUpdateHashReset(DBConnection, QU_USER_HASH_RESET);
    qUpdateHashReset.BindUInt(UserID);
    if (!qUpdateHashReset.Execute()) //Не удалось сбросить пароль
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserPasswordReset.Reset"), qUpdateHashReset);
    }
    qUpdateHashReset.First(); //Проверка не требуется

    //Фиксируем сброс пароля
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_PASSWORD_RESET, "_Users", ISMetaData::Instance().GetTable("_Users")->LocalListName, UserID, qUpdateHashReset.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::UserSettingsReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Сбрасываем значения настроек в дефолтные
    ISQuery qUpdate(DBConnection, QU_USER_SETTINGS_RESET);
    qUpdate.BindUInt(TcpMessage->TcpClient->UserID);
    if (!qUpdate.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.UserSettingsReset.Update"), qUpdate);
    }

    //Обходим результат
    rapidjson::Value SettingsObject(rapidjson::Type::kObjectType);
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    while (qUpdate.Next())
    {
        const char *SettingUID = qUpdate.ReadColumn(0), *SettingValue = qUpdate.ReadColumn(1);
        SettingsObject.AddMember(JSON_STRING(SettingUID), JSON_STRINGA(SettingValue, Allocator), Allocator);
    }
    TcpAnswer->Parameters.AddMember("Result", SettingsObject, Allocator);
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_USER_SETTINGS_RESET);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Получаем директорию с записями разговоров
    std::string RecordDir = ISConfig::Instance().GetValueString("AMI", "RecordDir");
    if (RecordDir.empty()) //Директория не настроена - ошибка
    {
        ErrorString = LANG("Carat.Error.Query.GetRecordCall.RecordDirIsEmpty");
        return false;
    }

    //По необходимости дополняем путь разделителем
    if (RecordDir.back() != PATH_SEPARATOR)
    {
        RecordDir.push_back(PATH_SEPARATOR);
    }

    unsigned int RecordID;
    if (!CheckIsNullUInt(TcpMessage, "RecordID", RecordID))
    {
        return false;
    }

    //Получаем идентификатор записи по её коду
    ISQuery qSelectRecord(DBConnection, QS_ASTERISK_RECORD);
    qSelectRecord.BindUInt(RecordID);
    if (!qSelectRecord.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetRecordCall.SelectUniqueID"), qSelectRecord);
    }

    if (!qSelectRecord.First())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetRecordCall.NotFoundUniqueID"), RecordID);
        return false;
    }
    const char *UniqueID = qSelectRecord.ReadColumn(0);

    std::string FilePath = RecordDir + UniqueID + ".wav";
    if (!ISAlgorithm::FileExist(FilePath)) //Файл не существует
    {
        ErrorString = LANG("Carat.Error.Query.GetRecordCall.NotFoundRecordCall");
        return false;
    }

    long FileSize = 0;
    unsigned char *FileData = ISAlgorithm::ReadFile(FilePath.c_str(), "rb", FileSize, ErrorString);
    if (!FileData)
    {
        return false;
    }

    //Кодируем в base64, очищаем память и проверяем результат кодирования
    std::string Base64 = ISAlgorithm::Base64Encode(FileData, (size_t)FileSize, ErrorString);
    free(FileData);
    if (Base64.empty())
    {
        return false;
    }

    //Читаем файл, закрываем и выходим
    TcpAnswer->Parameters.AddMember("Data", JSON_STRINGA(Base64.c_str(), TcpAnswer->Parameters.GetAllocator()), TcpAnswer->Parameters.GetAllocator());
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName;
    bool IsCopy = false;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullBool(TcpMessage, "IsCopy", IsCopy))
    {
        return false;
    }

    //Получаем указатель на мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Проверяем наличие списка значений
    if (!TcpMessage->Parameters.HasMember("Values"))
    {
        ErrorString = LANG("Carat.Error.Query.RecordAdd.ValuesNotExist");
        return false;
    }

    //Проверяем список значений на пустоту
    rapidjson::Value &Values = TcpMessage->Parameters["Values"];
    if (Values.GetType() != rapidjson::Type::kObjectType)
    {
        ErrorString = LANG("Carat.Error.Query.RecordAdd.ValuesIsNotObject");
        return false;
    }

    if (Values.ObjectEmpty())
    {
        ErrorString = LANG("Carat.Error.Query.RecordAdd.EmptyValues");
        return false;
    }

    //Обходим значения и готовим запрос
    std::string InsertText = "INSERT INTO " + MetaTable->Name + '(',
        ValuesText = "VALUES(";
    size_t ParameterPos = 0;
    for (const auto &MapItem : Values.GetObject())
    {
        std::string FieldName = MapItem.name.GetString();
        PMetaField *MetaField = MetaTable->GetField(FieldName);
        if (!MetaField) //Такого поля нет - ошибка
        {
            ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.RecordAdd.FieldNotFound"), FieldName.c_str());
            return false;
        }
        InsertText += MetaTable->Alias + '_' + FieldName + ',';
        ValuesText += '$' + std::to_string(++ParameterPos) + "::" + ISMetaData::Instance().GetTypeDB(MetaField->Type) + ',';
    }
    ISAlgorithm::StringChop(InsertText, 1);
    ISAlgorithm::StringChop(ValuesText, 1);

    //Создаём объект запроса и заполняем его параметрами
    ISQuery qInsert(DBConnection,
        InsertText + ")\n" + ValuesText + ")\nRETURNING " + MetaTable->Alias + "_id");
    qInsert.SetShowLongQuery(false);
    for (const auto &MapItem : Values.GetObject())
    {
        switch (MapItem.value.GetType())
        {
        case rapidjson::Type::kStringType:
            qInsert.BindString(MapItem.value.GetString());
            break;

        case rapidjson::Type::kTrueType:
        case rapidjson::Type::kFalseType:
            qInsert.BindBool(MapItem.value.GetBool());
            break;

        case rapidjson::Type::kNumberType:
            qInsert.BindInt64(MapItem.value.GetInt64());
            break;

        case rapidjson::Type::kNullType:
            qInsert.BindNull();
            break;

        case rapidjson::Type::kArrayType:
        case rapidjson::Type::kObjectType:
            ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.RecordAdd.TypeNotSupport"), MapItem.value.GetType());
            return false;
            break;
        }
    }

    //Выполняем запрос
    if (!qInsert.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.RecordAdd.Insert"), qInsert);
    }

    //Переходим на первую запись
    if (!qInsert.First())
    {
        ErrorString = LANG("Carat.Error.Query.RecordAdd.First");
        return false;
    }

    //Вытаскиваем новый идентификатор и получаем имя записи
    unsigned int ObjectID = qInsert.ReadColumn_UInt(0);
    std::string ObjectName;
    if (!GetObjectName(MetaTable, ObjectID, ObjectName))
    {
        return false;
    }

    //Протоколируем и записываем ответ
    Protocol(TcpMessage->TcpClient->UserID,
        IsCopy ? CONST_UID_PROTOCOL_CREATE_COPY_OBJECT : CONST_UID_PROTOCOL_CREATE_OBJECT,
        MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    TcpAnswer->Parameters.AddMember("ObjectID", ObjectID, Allocator);
    TcpAnswer->Parameters.AddMember("ObjectName", JSON_STRINGA(ObjectName.c_str(), Allocator), Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем указатель на мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Проверяем наличие списка значений
    if (!TcpMessage->Parameters.HasMember("Values"))
    {
        ErrorString = LANG("Carat.Error.Query.RecordEdit.ValuesNotExist");
        return false;
    }

    //Проверяем список значений на пустоту
    rapidjson::Value &Values = TcpMessage->Parameters["Values"];
    if (Values.GetType() != rapidjson::Type::kObjectType)
    {
        ErrorString = LANG("Carat.Error.Query.RecordAdd.ValuesIsNotObject");
        return false;
    }

    if (Values.ObjectEmpty())
    {
        ErrorString = LANG("Carat.Error.Query.RecordEdit.EmptyValues");
        return false;
    }

    //Обходим значения и готовим запрос
    std::string SqlText = "UPDATE " + MetaTable->Name + " SET\n";
    size_t ParameterPos = 0;
    for (const auto &MapItem : Values.GetObject())
    {
        std::string FieldName = MapItem.name.GetString();
        PMetaField *MetaField = MetaTable->GetField(FieldName);
        if (!MetaField) //Такого поля нет - ошибка
        {
            ErrorString = LANG("Carat.Error.Query.RecordEdit.FieldNotFound");
            return false;
        }
        SqlText += MetaTable->Alias + '_' + FieldName + " = $" + std::to_string(++ParameterPos) + "::" +
            ISMetaData::Instance().GetTypeDB(MetaField->Type) + ",\n";
    }
    ISAlgorithm::StringChop(SqlText, 2);
    SqlText += "\nWHERE " + MetaTable->Alias + "_id = $" + std::to_string(++ParameterPos);

    //Создаём объекта запроса и заполняем его параметрами
    ISQuery qUpdate(DBConnection, SqlText);
    for (const auto &MapItem : Values.GetObject())
    {
        switch (MapItem.value.GetType())
        {
        case rapidjson::Type::kStringType:
            qUpdate.BindString(MapItem.value.GetString());
            break;

        case rapidjson::Type::kTrueType:
        case rapidjson::Type::kFalseType:
            qUpdate.BindBool(MapItem.value.GetBool());
            break;

        case rapidjson::Type::kNumberType:
            qUpdate.BindInt64(MapItem.value.GetInt64());
            break;
        
        case rapidjson::Type::kNullType:
            qUpdate.BindNull();
            break;

        case rapidjson::Type::kArrayType:
        case rapidjson::Type::kObjectType:
            ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.RecordEdit.TypeNotSupport"), MapItem.value.GetType());
            return false;
            break;
        }
    }
    qUpdate.BindUInt(ObjectID);

    //Выполняем запрос
    if (!qUpdate.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.RecordEdit.Update"), qUpdate);
    }

    //Получаем имя объекта
    std::string ObjectName;
    if (!GetObjectName(MetaTable, ObjectID, ObjectName))
    {
        return false;
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    TcpAnswer->Parameters.AddMember("ObjectName", JSON_STRINGA(ObjectName.c_str(), Allocator), Allocator);
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_EDIT_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    std::string TableName;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName))
    {
        return false;
    }

    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Проверяем наличие идентификаторов
    if (!TcpMessage->Parameters.HasMember("Objects"))
    {
        ErrorString = LANG("Carat.Error.Query.RecordDelete.NotSpecifiedID");
        return false;
    }

    rapidjson::Value &Objects = TcpMessage->Parameters["Objects"];
    if (Objects.Empty())
    {
        ErrorString = LANG("Carat.Error.Query.RecordDelete.EmptyIDs");
        return false;
    }

    //Формируем конструкцию IN для условия WHERE
    std::string SqlIN;
    for (auto &ID : Objects.GetArray())
    {
        SqlIN += std::to_string(ID.GetUint()) + ',';
    }
    ISAlgorithm::StringChop(SqlIN, 1);

    //Получаем имена удаляемых записей для протокола
    std::unordered_map<unsigned int, std::string> ObjectNameMap;
    std::string ObjectName;
    for (auto &ID : Objects.GetArray())
    {
        if (!GetObjectName(MetaTable, ID.GetUint(), ObjectName))
        {
            return false;
        }
        ObjectNameMap.emplace(ID.GetUint(), ObjectName);
    }

    //Проверяем, нет ли системных записей
    ISQuery qSqlQuery(DBConnection, "SELECT (COUNT(*) > 0)::BOOLEAN AS is_exist FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_issystem AND " + MetaTable->Alias + "_id IN(" + SqlIN + ')');
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

    if (qSqlQuery.ReadColumn_Bool(0))
    {
        ErrorString = Objects.Size() == 1 ?
            LANG("Carat.Error.Query.RecordDelete.RecordIsSystem") :
            LANG("Carat.Error.Query.RecordDelete.RecordsIsSystem");
        return false;
    }

    //Удаляем записи
    ISQuery qDelete(DBConnection, "DELETE FROM " + MetaTable->Name + " WHERE " + MetaTable->Alias + "_id IN(" + SqlIN + ")");
    qDelete.SetShowLongQuery(false);
    if (!qDelete.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.RecordDelete.Delete"), qDelete);
    }

    //Удаляем записи из избранного
    ISQuery qDeleteFavorite(DBConnection, QD_FAVORITE);
    for (const auto &ID : Objects.GetArray())
    {
        qDeleteFavorite.BindUInt(TcpMessage->TcpClient->UserID);
        qDeleteFavorite.BindString(MetaTable->Name);
        qDeleteFavorite.BindUInt(ID.GetUint());
        if (!qDeleteFavorite.Execute())
        {
            ISLOGGER_W(__CLASS__, "Error remove object with favorites: %s", qDeleteFavorite.GetErrorString().c_str());
        }
    }

    //Протоколируем
    for (const auto &ObjectID : Objects.GetArray())
    {
        Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_DELETE_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID.GetUint(), ObjectNameMap[ObjectID.GetUint()]);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Формируем SQL-Запрос
    std::string SqlText = "SELECT\n";
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
            PMetaTable *MetaTableForeign = ISMetaData::Instance().GetTable(MetaField->Foreign->ForeignClass);

            //Формируем подзапрос
            SqlText += "(SELECT " + MetaTableForeign->Alias + "_id || ',' || concat(";
            ISVectorString VectorString = ISAlgorithm::StringSplit(MetaField->Foreign->ForeignViewNameField, ';');
            for (const std::string &FieldName : VectorString)
            {
                SqlText += MetaTableForeign->Alias + '_' + FieldName + ',';
            }
            ISAlgorithm::StringChop(SqlText, 1);
            SqlText += ") FROM " + MetaTableForeign->Name +
                " WHERE " + MetaTableForeign->Alias + "_id = " + MetaTable->Alias + '_' + MetaField->Name + ") AS \"" + MetaField->Name + "\",\n";
        }
        else //Поле стандартное
        {
            if (MetaField->QueryText.empty()) //Поле не является виртуальным
            {
                SqlText += MetaTable->Alias + '_' + MetaField->Name;
            }
            else //Поле является виртуальным
            {
                SqlText += '(' + MetaField->QueryText + ')';
            }
            SqlText += " AS \"" + MetaField->Name + "\",\n"; //Дополняем именование поля в выборке
        }
    }
    ISAlgorithm::StringChop(SqlText, 2);
    SqlText += "\nFROM " + MetaTable->Name + "\n";
    SqlText += "WHERE " + MetaTable->Alias + "_id = $1";

    //Выполняем запрос
    ISQuery qSelect(DBConnection, SqlText);
    qSelect.BindUInt(ObjectID);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.RecordGet.Select"), qSelect);
    }

    //Если переход на первую строку не удался - такая запись не существует
    if (!qSelect.First())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.RecordGet.NotExist"), ObjectID);
        return false;
    }

    //Получаем запись и проверяем, не системная ли она
    if (qSelect.ReadColumn_Bool(0))
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.RecordGet.IsSystem"), ObjectID);
        return false;
    }

    //Обходим поля записи
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    rapidjson::Value ValuesObject(rapidjson::Type::kObjectType);
    for (int i = 0, c = qSelect.GetResultColumnCount(); i < c; ++i)
    {
        const char *FieldName = qSelect.GetResultFieldName(i);

        /*PMetaField *MetaField = MetaTable->GetField(FieldName);
        if (MetaField->Type == ISNamespace::FieldType::Image) //Если поле является изображением, приводим его к base64
        {
            //Value = Value.toByteArray().toBase64();
        }*/

        if (qSelect.IsNull(i))
        {
            ValuesObject.AddMember(JSON_STRINGA(FieldName, Allocator), JSON_NULL, Allocator);
            continue;
        }

        Oid FieldTypeDB = qSelect.GetResultColumnType(i);
        switch (FieldTypeDB)
        {
        case BOOLOID:
            ValuesObject.AddMember(JSON_STRINGA(FieldName, Allocator), rapidjson::Value(qSelect.ReadColumn_Bool(i)), Allocator);
            break;

        case INT2OID:
        case INT4OID:
        case INT8OID:
            ValuesObject.AddMember(JSON_STRINGA(FieldName, Allocator), rapidjson::Value(qSelect.ReadColumn_Int64(i)), Allocator);
            break;

        case NUMERICOID:
            ValuesObject.AddMember(JSON_STRINGA(FieldName, Allocator), rapidjson::Value(qSelect.ReadColumn_Double(i)), Allocator);
            break;

        case DATEOID:
        case TIMEOID:
        case TIMESTAMPOID:
        {
            std::string String;
            switch (FieldTypeDB)
            {
            case DATEOID: String = qSelect.ReadColumn_Date(i).ToString(); break;
            case TIMEOID: String = qSelect.ReadColumn_Time(i).ToString(); break;
            case TIMESTAMPOID: String = qSelect.ReadColumn_DateTime(i).ToString(); break;
            }
            ValuesObject.AddMember(JSON_STRINGA(FieldName, Allocator), JSON_STRINGA(String.c_str(), Allocator), Allocator);
        }
        break;

        case TEXTOID:
        case VARCHAROID:
        case UUIDOID:
        {
            const char *String = qSelect.ReadColumn(i);
            ValuesObject.AddMember(JSON_STRINGA(FieldName, Allocator), JSON_STRINGA(String, Allocator), Allocator);
        }
        break;

        default:
            ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.RecordGet.UnsupportType"), FieldTypeDB);
            return false;
            break;
        }
    }

    //Получаем имя объекта
    std::string ObjectName;
    if (!GetObjectName(MetaTable, ObjectID, ObjectName))
    {
        return false;
    }
    TcpAnswer->Parameters.AddMember("Values", ValuesObject, Allocator);
    TcpAnswer->Parameters.AddMember("ObjectName", JSON_STRINGA(ObjectName.c_str(), Allocator), Allocator);
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_SHOW_OBJECT, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordGetInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    ISQuery qSelect(DBConnection, QS_RECORD_INFO);
    qSelect.BindString(TableName);
    qSelect.BindUInt(ObjectID);
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
    std::string ObjectName;
    if (!GetObjectName(MetaTable, ObjectID, ObjectName))
    {
        return false;
    }

    ISDateTime CreateDate = qSelect.ReadColumn_DateTime(0);
    std::string CreateDateString = CreateDate.ToString();
    const char *CreateDateCString = CreateDateString.c_str();
    const char *CreateUser = qSelect.ReadColumn(1);
    ISDateTime EditDate = qSelect.ReadColumn_DateTime(2);
    std::string EditDateString = EditDate.ToString();
    const char *EditDateCString = EditDateString.c_str();
    const char *EditUser = qSelect.ReadColumn(3);

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    TcpAnswer->Parameters.AddMember("TableName", JSON_STRINGA(MetaTable->LocalListName.c_str(), Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("ObjectName", JSON_STRINGA(ObjectName.c_str(), Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("CreateDate", JSON_STRINGA(CreateDate.IsNull() ? LANG("Carat.NoData") : CreateDateCString, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("CreateUser", JSON_STRINGA(strlen(CreateUser) > 0 ? CreateUser : LANG("Carat.NoData"), Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("EditDate", JSON_STRINGA(EditDate.IsNull() ? LANG("Carat.NoData") : EditDateCString, Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("EditUser", JSON_STRINGA(strlen(EditUser) > 0 ? EditUser : LANG("Carat.NoData"), Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("CopyCount", qSelect.ReadColumn_UInt(4), Allocator);
    TcpAnswer->Parameters.AddMember("EditCount", qSelect.ReadColumn_UInt(5), Allocator);
    TcpAnswer->Parameters.AddMember("ShowCount", qSelect.ReadColumn_UInt(6), Allocator);
    TcpAnswer->Parameters.AddMember("FavoriteCount", qSelect.ReadColumn_UInt(7), Allocator);
    TcpAnswer->Parameters.AddMember("ProtocolCount", qSelect.ReadColumn_UInt(8), Allocator);
    TcpAnswer->Parameters.AddMember("DiscussionCount", qSelect.ReadColumn_UInt(9), Allocator);
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_RECORD_INFO, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName, Message;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID) ||
        !CheckIsNullString(TcpMessage, "Message", Message))
    {
        return false;
    }

    ISQuery qInsert(DBConnection, QI_DISCUSSION);
    qInsert.BindUInt(TcpMessage->TcpClient->UserID);
    qInsert.BindString(TableName);
    qInsert.BindUInt(ObjectID);
    qInsert.BindString(Message);
    if (!qInsert.Execute()) //Ошибка вставки
    {
        return ErrorQuery(LANG("Carat.Error.Query.DiscussionAdd.Insert"), qInsert);
    }

    if (!qInsert.First()) //Ошибка перехода к возвращаемому значению
    {
        ErrorString = qInsert.GetErrorString();
        return false;
    }
    TcpAnswer->Parameters.AddMember("ID", qInsert.ReadColumn_UInt(0), TcpAnswer->Parameters.GetAllocator());
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int ID = 0;
    std::string Message;
    if (!CheckIsNullUInt(TcpMessage, "ID", ID) ||
        !CheckIsNullString(TcpMessage, "Message", Message))
    {
        return false;
    }

    ISQuery qUpdate(DBConnection, QU_DISCUSSION);
    qUpdate.BindString(Message);
    qUpdate.BindUInt(ID);
    if (!qUpdate.Execute()) //Не удалось обновить запись
    {
        return ErrorQuery(LANG("Carat.Error.Query.DiscussionEdit.Update"), qUpdate);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    unsigned int ID = 0;
    if (!CheckIsNullUInt(TcpMessage, "ID", ID))
    {
        return false;
    }

    ISQuery qCopy(DBConnection, QI_DISCUSSION_COPY);
    qCopy.BindUInt(ID);
    if (!qCopy.Execute()) //Не удалось создать копию
    {
        return ErrorQuery(LANG("Carat.Error.Query.DiscussionCopy.Insert"), qCopy);
    }

    if (!qCopy.First()) //Ошибка перехода к возвращаемому значению
    {
        ErrorString = qCopy.GetErrorString();
        return false;
    }
    TcpAnswer->Parameters.AddMember("ID", qCopy.ReadColumn_UInt(0), TcpAnswer->Parameters.GetAllocator());
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetTableData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }
    unsigned int UserID = TcpMessage->TcpClient->UserID;

    //Проверяем наличие права на просмотр данных
    ISQuery qSelectRight(DBConnection, QS_RIGHT_SHOW_TABLE);
    qSelectRight.BindUInt(UserID);
    qSelectRight.BindString(MetaTable->Name);
    qSelectRight.BindUID(CONST_UID_GROUP_ACCESS_TYPE_SHOW);
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
    if (!qSelectRight.ReadColumn_Bool(0))
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetTableData.NoRightShow"), MetaTable->LocalListName.c_str());
        return false;
    }

    //Получаем сортировку для этой таблицы
    std::string SortingField;
    ISNamespace::SortingOrder SortingOrder = ISNamespace::SortingOrder::Ascending;
    ISQuery qSelectSorting(DBConnection, QS_SORTING);
    qSelectSorting.BindUInt(UserID);
    qSelectSorting.BindString(MetaTable->Name);
    if (!qSelectSorting.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetTableData.SelectSorting"), qSelectSorting);
    }

    if (qSelectSorting.First()) //Сортирока есть - получаем её
    {
        SortingField = qSelectSorting.ReadColumn(0);
        SortingOrder = static_cast<ISNamespace::SortingOrder>(qSelectSorting.ReadColumn_Int(1));

        //Проверяем не указана ли сортировка в запросе
        //Если указана - проверяем - не нужно ли обновить её в БД
        if (TcpMessage->Parameters.HasMember("Sorting"))
        {
            rapidjson::Value &SortingObject = TcpMessage->Parameters["Sorting"];
            std::string SortingFieldNew = SortingObject["Field"].GetString();
            ISNamespace::SortingOrder SortingOrderNew = static_cast<ISNamespace::SortingOrder>(SortingObject["Order"].GetInt());

            //Если новая сортировка отличается от текущей - сохраняем её в БД
            if (SortingField != SortingFieldNew || SortingOrder != SortingOrderNew)
            {
                SortingField = SortingFieldNew;
                SortingOrder = SortingOrderNew;

                ISQuery qUpdateSorting(DBConnection, QU_SORTING);
                qUpdateSorting.BindString(SortingField);
                qUpdateSorting.BindInt((int)SortingOrder);
                qUpdateSorting.BindUInt(UserID);
                qUpdateSorting.BindString(MetaTable->Name);
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

        ISQuery qInsertSorting(DBConnection, QI_SORTING);
        qInsertSorting.BindUInt(UserID);
        qInsertSorting.BindString(MetaTable->Name);
        qInsertSorting.BindString(SortingField);
        qInsertSorting.BindInt((int)SortingOrder);
        if (!qInsertSorting.Execute())
        {
            return ErrorQuery(LANG("Carat.Error.Query.GetTableData.InsertSorting"), qInsertSorting);
        }
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Заполняем служебную информацию
    rapidjson::Value ServiceInfoObject(rapidjson::Type::kObjectType);
    ServiceInfoObject.AddMember("SortingField", JSON_STRINGA(SortingField.c_str(), Allocator), Allocator);
    ServiceInfoObject.AddMember("SortingOrder", (int)SortingOrder, Allocator);
    ServiceInfoObject.AddMember("TableLocalName", JSON_STRINGA(MetaTable->LocalListName.c_str(), Allocator), Allocator);
    ServiceInfoObject.AddMember("TableShowOnly", MetaTable->ShowOnly, Allocator);
    ServiceInfoObject.AddMember("TableTitleName", JSON_STRINGA(MetaTable->TitleName.c_str(), Allocator), Allocator);

    //Получаем фильтры
    rapidjson::Value FilterObject(rapidjson::Type::kObjectType);
    if (TcpMessage->Parameters.FindMember("Filter") != TcpMessage->Parameters.MemberEnd())
    {
        FilterObject = TcpMessage->Parameters["Filter"];
    }

    //Получаем поисковые параметры
    rapidjson::Value SearchArray(rapidjson::Type::kArrayType);
    if (TcpMessage->Parameters.FindMember("Search") != TcpMessage->Parameters.MemberEnd())
    {
        SearchArray = TcpMessage->Parameters["Search"];
    }
    bool IsSearch = !SearchArray.Empty();

    //Формируем запрос на выборку
    std::string SqlText = ISTcpWorkerHelper::CreateSqlFromTable(MetaTable, FilterObject, SearchArray, SortingField, SortingOrder);

    ISQuery qSelect(DBConnection, SqlText);
    qSelect.SetShowLongQuery(false);

    //Заполняем параметры запроса
    if (FilterObject.MemberCount() > 0)
    {
        for (const auto &MapItem : FilterObject.GetObject())
        {
            switch (MapItem.value.GetType())
            {
            case rapidjson::Type::kNumberType:
                qSelect.BindUInt64(MapItem.value.GetUint64());
                break;

            case rapidjson::Type::kStringType:
                qSelect.BindString(MapItem.value.GetString());
                break;

            default:
                ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetTableData.ClassFilterValueType"), MapItem.value.GetType(), MapItem.name.GetString());
                return false;
                break;
            }
        }
    }

    if (!qSelect.Execute()) //Запрос не отработал
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetTableData.Select"), qSelect);
    }

    int FieldCount = qSelect.GetResultColumnCount();
    rapidjson::Value FieldArray(rapidjson::Type::kArrayType),
        RecordArray(rapidjson::Type::kArrayType);
    std::vector<ISNamespace::FieldType> VectorType(FieldCount, ISNamespace::FieldType::Unknown);
    std::vector<bool> VectorForeign(FieldCount, false);

    //Заполняем описание полей
    for (int i = 0; i < FieldCount; ++i)
    {
        PMetaField *MetaField = MetaTable->GetField(qSelect.GetResultFieldName(i));
        VectorType[i] = MetaField->Type; //Заполняем типы сейчас, чтобы использовать их ниже
        VectorForeign[i] = MetaField->Foreign ? true : false;

        rapidjson::Value FieldObject(rapidjson::Type::kObjectType);
        FieldObject.AddMember("Index", i, Allocator);
        FieldObject.AddMember("Name", JSON_STRING(MetaField->Name.c_str()), Allocator);
        FieldObject.AddMember("LocalName", JSON_STRING(MetaField->LocalListName.c_str()), Allocator);
        FieldObject.AddMember("Type", static_cast<int>(MetaField->Type), Allocator);
        FieldObject.AddMember("IsForeign", MetaField->Foreign ? true : false, Allocator);
        FieldObject.AddMember("IsSystem", MetaField->IsSystem, Allocator);
        FieldArray.PushBack(FieldObject, Allocator);
    }

    if (qSelect.First()) //Данные есть
    {
        //Получаем необходимые настройки БД
        //???
        unsigned int Precision = std::atoi(ISTcpWorkerHelper::GetSettingDB(DBConnection, CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA));
        IS_UNUSED(Precision);

        do
        {
            rapidjson::Value Values(rapidjson::Type::kArrayType); //Список значений
            for (int i = 0; i < FieldCount; ++i) //Обходим поля и вытаскиваем значения
            {
                //Получаем тип поля и наличие внешнего ключа
                ISNamespace::FieldType Type = VectorType[i];
                bool IsForeign = VectorForeign[i];

                //Если значение содержит NULL - добавляем пустое и переходим к следующему
                if (qSelect.IsNull(i))
                {
                    Values.PushBack(JSON_NULL, Allocator);
                    continue;
                }
                //Значение не содержит NULL - анализируем

                if (Type == ISNamespace::FieldType::Int ||
                    Type == ISNamespace::FieldType::BigInt ||
                    Type == ISNamespace::FieldType::Year ||
                    Type == ISNamespace::FieldType::Volume ||
                    Type == ISNamespace::FieldType::Month)
                {
                    if (IsForeign)
                    {
                        Values.PushBack(JSON_STRINGA(qSelect.ReadColumn(i), Allocator), Allocator);
                    }
                    else
                    {
                        std::string String = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(i));
                        const char *CString = String.c_str();
                        Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                    }
                }
                else if (Type == ISNamespace::FieldType::ID)
                {
                    Values.PushBack(qSelect.ReadColumn_UInt64(i), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Bool)
                {
                    Values.PushBack(qSelect.ReadColumn_Bool(i), Allocator);
                }
                else if (Type == ISNamespace::FieldType::String ||
                    Type == ISNamespace::FieldType::Text ||
                    Type == ISNamespace::FieldType::Inn ||
                    Type == ISNamespace::FieldType::Kpp ||
                    Type == ISNamespace::FieldType::Ogrn ||
                    Type == ISNamespace::FieldType::Okpo ||
                    Type == ISNamespace::FieldType::Bik ||
                    Type == ISNamespace::FieldType::Vin ||
                    Type == ISNamespace::FieldType::Color ||
                    Type == ISNamespace::FieldType::EMail ||
                    Type == ISNamespace::FieldType::Url)
                {
                    Values.PushBack(JSON_STRINGA(qSelect.ReadColumn(i), Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Date || Type == ISNamespace::FieldType::Birthday)
                {
                    std::string String = ISTcpWorkerHelper::ConvertDateToString(qSelect.ReadColumn_Date(i));
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Time)
                {
                    std::string String = qSelect.ReadColumn_Time(i).ToString();
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::DateTime || Type == ISNamespace::FieldType::ProtocolDT)
                {
                    std::string String = ISTcpWorkerHelper::ConvertDateTimeToString(qSelect.ReadColumn_DateTime(i));
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Phone)
                {
                    const char *String = qSelect.ReadColumn(i);
                    char Temp[19] = { 0 };
                    Temp[0] = '+';
                    Temp[1] = '7';
                    Temp[2] = ' ';
                    Temp[3] = '(';
                    Temp[4] = String[0];
                    Temp[5] = String[1];
                    Temp[6] = String[2];
                    Temp[7] = ')';
                    Temp[8] = ' ';
                    Temp[9] = String[3];
                    Temp[10] = String[4];
                    Temp[11] = String[5];
                    Temp[12] = '-';
                    Temp[13] = String[6];
                    Temp[14] = String[7];
                    Temp[15] = '-';
                    Temp[16] = String[8];
                    Temp[17] = String[9];
                    Temp[18] = CHAR_NULL_TERM;
                    Values.PushBack(JSON_STRINGA(Temp, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Seconds)
                {
                    std::string String = ISTime::FromSecond(qSelect.ReadColumn_UInt(i)).ToString();
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Double)
                {
                    std::string String = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Double(i), ' ', Precision);
                    const char *CString = String.c_str();
                    Values.PushBack(JSON_STRINGA(CString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::Money)
                {
                    std::string String = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Double(i), ' ', 2);
                    const char *SString = String.c_str();
                    Values.PushBack(JSON_STRINGA(SString, Allocator), Allocator);
                }
                else if (Type == ISNamespace::FieldType::UID)
                {
                    const char *String = qSelect.ReadColumn(i);
                    Values.PushBack(JSON_STRINGA(String, Allocator), Allocator);
                }
                else //Тип не определён
                {
                    ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetTableData.UnknownType"), Type);
                    return false;
                }
            }
            RecordArray.PushBack(Values, Allocator); //Добавляем список значений в список записей
        } while (qSelect.Next()); //Обходим выборку	
    }
    TcpAnswer->Parameters.AddMember("ServiceInfo", ServiceInfoObject, Allocator);
    TcpAnswer->Parameters.AddMember("FieldList", FieldArray, Allocator);
    TcpAnswer->Parameters.AddMember("RecordList", RecordArray, Allocator);
    Protocol(TcpMessage->TcpClient->UserID, IsSearch ? CONST_UID_PROTOCOL_SEARCH : CONST_UID_PROTOCOL_GET_TABLE_DATA, MetaTable->Name, MetaTable->LocalListName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetTableQuery(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string QueryName;
    if (!CheckIsNullString(TcpMessage, "QueryName", QueryName))
    {
        return false;
    }

    const char *SqlText = ISResourcer::Instance().GetFile("SQL/" + QueryName + ".sql");
    if (!SqlText) //Запрос не найден - ошибка
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetTableQuery.NotFound"), QueryName.c_str());
        return false;
    }

    ISQuery qSelect(DBConnection, SqlText);
    if (TcpMessage->Parameters.HasMember("Parameters"))
    {
        rapidjson::Value &ParametersObject = TcpMessage->Parameters["Parameters"];
        for (const auto &MapItem : ParametersObject.GetObject())
        {
            switch (MapItem.value.GetType())
            {
            case rapidjson::Type::kNumberType:
                qSelect.BindUInt64(MapItem.value.GetUint64());
                break;

            case rapidjson::Type::kStringType:
                qSelect.BindString(MapItem.value.GetString());
                break;

            default:
                ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetTableQuery.ClassFilterValueType"), MapItem.value.GetType(), MapItem.name.GetString());
                return false;
                break;
            }
        }
    }

    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetTableQuery.Select"), qSelect);
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Обходим поля
    rapidjson::Value FieldArray(rapidjson::Type::kArrayType);
    int ColumnCount = qSelect.GetResultColumnCount();
    for (int i = 0; i < ColumnCount; ++i)
    {
        const char *FieldName = qSelect.GetResultFieldName(i);
        FieldArray.PushBack(JSON_STRINGA(FieldName, Allocator), Allocator);
    }

    rapidjson::Value RecordArray(rapidjson::Type::kArrayType);
    if (qSelect.First()) //Данные есть - обходим записи
    {
        do
        {
            rapidjson::Value ValuesArray(rapidjson::Type::kArrayType);
            for (int i = 0; i < ColumnCount; ++i)
            {
                char *Value = qSelect.ReadColumn(i);
                ValuesArray.PushBack(JSON_STRINGA(Value, Allocator), Allocator);
            }
            RecordArray.PushBack(ValuesArray, Allocator);
        } while (qSelect.Next());
    }
    TcpAnswer->Parameters.AddMember("FieldList", FieldArray, Allocator);
    TcpAnswer->Parameters.AddMember("RecordList", RecordArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetNoteRecord(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Получаем примечание
    ISQuery qSelect(DBConnection, QS_NOTE_RECORD);
    qSelect.BindString(MetaTable->Name);
    qSelect.BindUInt(ObjectID);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetNoteRecord.Select"), qSelect);
    }

    std::string Note;
    if (qSelect.First()) //Если запись есть - вытаскиваем её
    {
        Note = qSelect.ReadColumn_String(0);
    }
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    TcpAnswer->Parameters.AddMember("Note", JSON_STRINGA(Note.c_str(), Allocator), Allocator);
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_NOTE_RECORD_SHOW, MetaTable->Name, MetaTable->LocalListName, ObjectID);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SetNoteRecord(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    std::string TableName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Проверяем наличие записи
    std::string NoteDB, Note = TcpMessage->Parameters["Note"].GetString();
    ISQuery qSelect(DBConnection, QS_NOTE_RECORD);
    qSelect.BindString(MetaTable->Name);
    qSelect.BindUInt(ObjectID);
    bool Exist = qSelect.ExecuteFirst();
    if (Exist)
    {
        NoteDB = qSelect.ReadColumn_String(0);
    }

    //Если примечание такое же как и в БД - ничего не делаем
    if (Note == NoteDB)
    {
        return true;
    }

    //Примечания отличаются - проверяем что делать дальше
    //Если примечание в БД есть, а новое пустое - удаляем
    if (Exist && Note.empty())
    {
        ISQuery qDelete(DBConnection, QD_NOTE_RECORD);
        qDelete.BindString(MetaTable->Name);
        qDelete.BindUInt(ObjectID);
        if (!qDelete.Execute())
        {
            return ErrorQuery(LANG("Carat.Error.Query.SetNoteRecord.Delete"), qDelete);
        }
        Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_NOTE_RECORD_DELETE, MetaTable->Name, MetaTable->LocalListName, ObjectID);
    }
    else //Добавляем/обновляем
    {
        ISQuery qUpsert(DBConnection, Exist ? QU_NOTE_RECORD : QI_NOTE_RECORD);
        qUpsert.BindString(Note);
        qUpsert.BindString(MetaTable->Name);
        qUpsert.BindUInt(ObjectID);
        if (!qUpsert.Execute()) //Ошибка запроса
        {
            return ErrorQuery(Exist ?
                LANG("Carat.Error.Query.SetNoteRecord.Update") :
                LANG("Carat.Error.Query.SetNoteRecord.Insert"), qUpsert);
        }
        Protocol(TcpMessage->TcpClient->UserID, Exist ? CONST_UID_PROTOCOL_NOTE_RECORD_EDIT : CONST_UID_PROTOCOL_NOTE_RECORD_ADD,
            MetaTable->Name, MetaTable->LocalListName, ObjectID, Note);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    std::string FileName, FileData;
    if (!CheckIsNullString(TcpMessage, "FileName", FileName) ||
        !CheckIsNullString(TcpMessage, "Data", FileData))
    {
        return false;
    }

    //Вытаскиваем расширение файла
    std::string Extension;
    size_t Pos = FileName.rfind('.');
    if (Pos != NPOS)
    {
        Extension = FileName.substr(Pos + 1, FileName.size() - Pos - 1);
    }

    //Декодируем данные файла
    unsigned long DataSize = 0;
    unsigned char *Data = ISAlgorithm::Base64Decode(FileData, DataSize, ErrorString);
    FileData.clear();

    if (!Data)
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.FileStorageAdd.Decode"), ErrorString.c_str());
        return false;
    }

    //Проверяем размер
    size_t MaxSizeMB = std::atoi(ISTcpWorkerHelper::GetSettingDB(DBConnection, CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE));
    if (DataSize > (((1000 * 1024) * MaxSizeMB)))
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.FileStorageAdd.Size"), FileName.c_str(), MaxSizeMB);
        return false;
    }

    ISQuery qInsert(DBConnection, QI_FILE_STORAGE);
    qInsert.SetShowLongQuery(false);
    qInsert.BindUInt(TcpMessage->TcpClient->UserID);
    qInsert.BindString(FileName);
    Extension.empty() ? qInsert.BindNull() : qInsert.BindString(Extension);
    qInsert.BindString(ISAlgorithm::StringFromSize(DataSize));
    qInsert.BindBinary(Data, DataSize);
    bool Result = qInsert.ExecuteFirst();
    free(Data);
    if (!Result)
    {
        return ErrorQuery(ISAlgorithm::StringF(LANG("Carat.Error.Query.FileStorageAdd.Insert"), FileName.c_str(), qInsert.GetErrorString().c_str()), qInsert);
    }
    unsigned int ID = qInsert.ReadColumn_UInt(0);
    TcpAnswer->Parameters.AddMember("ID", ID, TcpAnswer->Parameters.GetAllocator());
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_FILE_STORAGE_ADD, std::string(), std::string(), ID, FileName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    unsigned int ID = 0;
    std::string Name;
    if (!CheckIsNullUInt(TcpMessage, "ID", ID) ||
        !CheckIsNullString(TcpMessage, "Name", Name))
    {
        return false;
    }

    //Копируем файл
    ISQuery qInsertCopy(DBConnection, QI_FILE_STORAGE_COPY);
    qInsertCopy.SetShowLongQuery(false);
    qInsertCopy.BindString(Name);
    qInsertCopy.BindUInt(ID);
    if (!qInsertCopy.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.FileStorageCopy.Insert"), qInsertCopy);
    }

    //Если файл не был скопирован, значит его не существует - ошибка
    if (qInsertCopy.GetResultAffected() == 0)
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.FileStorageCopy.NotExist"), ID);
        return false;
    }

    //Если удалось перейти на первую строку - получаем новый идентификатор записи,
    //иначе - идентификатор исходной записи
    ID = qInsertCopy.First() ? qInsertCopy.ReadColumn_UInt(0) : ID;
    TcpAnswer->Parameters.AddMember("NewID", ID, TcpAnswer->Parameters.GetAllocator());
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_FILE_STORAGE_COPY, std::string(), std::string(), ID, Name);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FileStorageGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    unsigned int ID = 0;
    if (!CheckIsNullUInt(TcpMessage, "ID", ID))
    {
        return false;
    }

    //Вытаскиваем файл
    ISQuery qSelect(DBConnection, QS_FILE_STORAGE);
    qSelect.SetShowLongQuery(false);
    qSelect.BindUInt(ID);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.FileStorageGet.Select"), qSelect);
    }

    if (!qSelect.First()) //Такой файл не существует
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.FileStorageGet.NotExist"), ID);
        return false;
    }

    size_t DataSize = 0;
    unsigned char *Data = qSelect.ReadColumn_Binary(1, DataSize);
    if (!Data)
    {
        ErrorString = LANG("Carat.Error.Query.FileStorageGet.Binary");
        return false;
    }

    std::string String = ISAlgorithm::Base64Encode((unsigned char *)Data, DataSize, ErrorString);
    if (String.empty())
    {
        ErrorString = LANG("Carat.Error.Query.FileStorageGet.Encode");
        return false;
    }

    TcpAnswer->Parameters.AddMember("Data", JSON_STRINGA(String.c_str(), TcpAnswer->Parameters.GetAllocator()), TcpAnswer->Parameters.GetAllocator());
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_FILE_STORAGE_SAVE, std::string(), std::string(), qSelect.ReadColumn_UInt(0));
    PQfreemem(Data);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SearchFullText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string Value;
    if (!CheckIsNullString(TcpMessage, "Value", Value))
    {
        return false;
    }

    //Формируем запрос
    std::string SqlText = "WITH r AS \n(\n";
    for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables()) //Обходим все таблицы
    {
        SqlText += "SELECT " + MetaTable->Alias + "_id AS id, '" + MetaTable->Name + "' AS table_name \n";
        SqlText += "FROM " + MetaTable->Name + " \n";
        SqlText += "WHERE lower(concat(";
        for (PMetaField *MetaField : MetaTable->Fields) //Обходим поля конкретной таблицы
        {
            if (MetaField->IsSystem || !MetaField->QueryText.empty()) //Если поле является системным - пропускаем его
            {
                continue;
            }
            SqlText += MetaTable->Alias + '_' + MetaField->Name + ",";
        }
        ISAlgorithm::StringChop(SqlText, 1);
        SqlText += ")) LIKE '%' || lower($1) || '%'\n";
        SqlText += "UNION \n";
    }
    ISAlgorithm::StringChop(SqlText, 8);
    SqlText += " \n) \n";
    SqlText += "SELECT * \n";
    SqlText += "FROM r \n";
    SqlText += "ORDER BY table_name, id";

    //Выполняем запрос
    ISQuery qSelect(DBConnection, SqlText);
    qSelect.BindString(Value);
    if (!qSelect.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.SearchFullText.Select"), qSelect);
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Анализируем ответ
    rapidjson::Value JsonArray(rapidjson::Type::kArrayType);
    while (qSelect.Next())
    {
        unsigned int ID = qSelect.ReadColumn_UInt(0);
        std::string TableName = qSelect.ReadColumn_String(1);

        //Получаем имя объекта
        std::string ObjectName;
        if (!GetObjectName(ISMetaData::Instance().GetTable(TableName), ID, ObjectName))
        {
            return false;
        }

        rapidjson::Value JsonObject(rapidjson::Type::kObjectType);
        JsonObject.AddMember("ID", ID, Allocator);
        JsonObject.AddMember("TableName", JSON_STRINGA(TableName.c_str(), Allocator), Allocator);
        JsonObject.AddMember("ObjectName", JSON_STRINGA(ObjectName.c_str(), Allocator), Allocator);
        JsonArray.PushBack(JsonObject, Allocator);
    }
    TcpAnswer->Parameters.AddMember("Result", JsonArray, Allocator);
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_SEARCH_FULL_TEXT, std::string(), std::string(), 0, Value);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetCalendarEvents(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CalendarDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetInternalLists(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);

    //Запрашиваем справочники
    ISQuery qSelect(DBConnection, QS_INTERNAL_LISTS);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetInternalLists.Select"), qSelect);
    }

    //Обходим результаты запроса
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    rapidjson::Value JsonArray(rapidjson::Type::kArrayType);
    while (qSelect.Next())
    {
        const char *Value = qSelect.ReadColumn(0);
        JsonArray.PushBack(JSON_STRINGA(Value, Allocator), Allocator);
    }
    TcpAnswer->Parameters.AddMember("Lists", JsonArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::SaveMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    //Протоколируем в начале, вдруг где-то дальше будет ошибка
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_EXIT_APPLICATION);

    //Готовим запросы для размеров полей
    ISQuery qInsertColumnSize(DBConnection, QI_COLUMN_SIZE),
        qUpdateColumnSize(DBConnection, QU_COLUMN_SIZE);

    //Размеры полей указаны
    if (TcpMessage->Parameters.HasMember("ColumnSize"))
    {
        //Получаем размеры полей и обходим их
        rapidjson::Value &ColumnSizeObject = TcpMessage->Parameters["ColumnSize"];
        for (const auto &TableItem : ColumnSizeObject.GetObject())
        {
            for (const auto &FieldItem : TableItem.value.GetObject())
            {
                //Пытаем добавить размер поля
                qInsertColumnSize.BindUInt(TcpMessage->TcpClient->UserID);
                qInsertColumnSize.BindString(TableItem.name.GetString());
                qInsertColumnSize.BindString(FieldItem.name.GetString());
                qInsertColumnSize.BindUInt(FieldItem.value.GetUint());
                if (!qInsertColumnSize.Execute()) //Если вставка не удалась - проверяем причину
                {
                    if (qInsertColumnSize.GetErrorNumber() == 23505) //Причина - наружение уникальности - обновляем
                    {
                        qUpdateColumnSize.BindUInt(FieldItem.value.GetUint());
                        qUpdateColumnSize.BindUInt(TcpMessage->TcpClient->UserID);
                        qUpdateColumnSize.BindString(TableItem.name.GetString());
                        qUpdateColumnSize.BindString(FieldItem.name.GetString());
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
    }

    //Пользовательские настройки указаны
    if (TcpMessage->Parameters.HasMember("Settings"))
    {
        //Получаем изменные настройки
        rapidjson::Value &SettingsObject = TcpMessage->Parameters["Settings"];
        if (SettingsObject.MemberCount() > 0)
        {
            //Готовим запрос для обновления настроек
            ISQuery qUpdateSetting(DBConnection, QU_SETTING);
            for (const auto &MapItem : SettingsObject.GetObject())
            {
                qUpdateSetting.BindString(MapItem.value.GetString());
                qUpdateSetting.BindUInt(TcpMessage->TcpClient->UserID);
                qUpdateSetting.BindUID(MapItem.name.GetString());
                if (!qUpdateSetting.Execute())
                {
                    return ErrorQuery(LANG("Carat.Error.Query.SaveMetaData.UpdateSetting"), qUpdateSetting);
                }
            }
        }
    }

    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetGroupRights(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    unsigned int GroupID = 0;
    if (!CheckIsNullUInt(TcpMessage, "GroupID", GroupID))
    {
        return false;
    }
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Получаем системы и подсистемы
    rapidjson::Value SystemsArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSystems(DBConnection, QS_GROUP_RIGHT_SYSTEM),
        qSelectSubSystems(DBConnection, QS_GROUP_RIGHT_SUBSYSTEM);
    if (qSelectSystems.Execute()) //Запрашиваем список систем
    {
        while (qSelectSystems.Next())
        {
            const char *SystemLocalName = qSelectSystems.ReadColumn(1);

            rapidjson::Value SystemObject(rapidjson::Type::kObjectType);
            SystemObject.AddMember("LocalName", JSON_STRINGA(SystemLocalName, Allocator), Allocator);

            qSelectSubSystems.BindUInt(GroupID);
            qSelectSubSystems.BindUID(qSelectSystems.ReadColumn(0));
            if (qSelectSubSystems.Execute())
            {
                while (qSelectSubSystems.Next())
                {
                    const char *SubSystemUID = qSelectSubSystems.ReadColumn(0),
                        *SubSystemLocalName = qSelectSubSystems.ReadColumn(1);

                    rapidjson::Value SubSystemObject(rapidjson::Type::kObjectType);
                    SubSystemObject.AddMember("UID", JSON_STRINGA(SubSystemUID, Allocator), Allocator);
                    SubSystemObject.AddMember("LocalName", JSON_STRINGA(SubSystemLocalName, Allocator), Allocator);
                    SubSystemObject.AddMember("IsExist", qSelectSubSystems.ReadColumn_Bool(2), Allocator);

                    if (SystemObject.HasMember("SubSystems"))
                    {
                        SystemObject["SubSystems"].PushBack(SubSystemObject, Allocator);
                    }
                    else
                    {
                        rapidjson::Value SubSystemsArray(rapidjson::Type::kArrayType);
                        SubSystemsArray.PushBack(SubSystemObject, Allocator);
                        SystemObject.AddMember("SubSystems", SubSystemsArray, Allocator);
                    }
                }
            }
            else
            {
                return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSubSystems"), qSelectSubSystems);
            }
            SystemsArray.PushBack(SystemObject, Allocator);
        }
    }
    else //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSystems"), qSelectSystems);
    }

    //Получаем типы прав доступа на таблицы
    rapidjson::Value RightsTableTypeArray(rapidjson::Type::kArrayType);
    ISQuery qSelectAccessTablesType(DBConnection, QS_GROUP_RIGHT_TABLE_TYPE);
    if (qSelectAccessTablesType.Execute())
    {
        while (qSelectAccessTablesType.Next())
        {
            const char *UID = qSelectAccessTablesType.ReadColumn(0),
                *LocalName = qSelectAccessTablesType.ReadColumn(1),
                *Icon = qSelectAccessTablesType.ReadColumn(2);
            rapidjson::Value JsonObject(rapidjson::Type::kObjectType);
            JsonObject.AddMember("UID", JSON_STRINGA(UID, Allocator), Allocator);
            JsonObject.AddMember("LocalName", JSON_STRINGA(LocalName, Allocator), Allocator);
            JsonObject.AddMember("Icon", JSON_STRINGA(Icon, Allocator), Allocator);
            RightsTableTypeArray.PushBack(JsonObject, Allocator);
        }
    }
    else
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.RightTableType"), qSelectAccessTablesType);
    }

    //Обходим таблицы
    rapidjson::Value TablesArray(rapidjson::Type::kArrayType);
    //QVariantMap TablesMap; //Этот контейнер нужен для сортировки списка таблиц по алфавиту
    ISQuery qSelectTables(DBConnection, QS_GROUP_RIGHT_TABLE);
    for (PMetaTable *MetaTable : ISMetaData::Instance().GetTables())
    {
        //Если таблица является системной - идём дальше
        if (MetaTable->IsSystem)
        {
            continue;
        }

        rapidjson::Value TableObject(rapidjson::Type::kObjectType);
        TableObject.AddMember("TableName", JSON_STRINGA(MetaTable->Name.c_str(), Allocator), Allocator);
        TableObject.AddMember("LocalName", JSON_STRINGA(MetaTable->LocalListName.c_str(), Allocator), Allocator);

        qSelectTables.BindUInt(GroupID);
        qSelectTables.BindString(MetaTable->Name);
        if (qSelectTables.Execute()) //Запрашиваем права на текущую таблицу
        {
            while (qSelectTables.Next()) //Обходим права на текущую таблицу
            {
                const char *UID = qSelectTables.ReadColumn(0);
                if (TableObject.HasMember("Rights"))
                {
                    TableObject["Rights"].PushBack(JSON_STRINGA(UID, Allocator), Allocator);
                }
                else
                {
                    rapidjson::Value RightsArray(rapidjson::Type::kArrayType);
                    RightsArray.PushBack(JSON_STRINGA(UID, Allocator), Allocator);
                    TableObject.AddMember("Rights", RightsArray, Allocator);
                }
            }
        }
        else //Ошибка запроса
        {
            return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectTables"), qSelectTables);
        }
        TablesArray.PushBack(TableObject, Allocator);
    }

    /*for (const auto &MapItem : TablesMap.toStdMap())
    {
        QVariantMap TempMap = MapItem.second.toMap();
        TempMap["LocalName"] = MapItem.first;
        TablesList.append(TempMap);
    }*/

    //Получаем спец. права
    rapidjson::Value SpecialArray(rapidjson::Type::kArrayType);
    ISQuery qSelectSpecialParent(DBConnection, QS_GROUP_RIGHT_SPECIAL_PARENT),
        qSelectSpecial(DBConnection, QS_GROUP_RIGHT_SPECIAL);
    if (qSelectSpecialParent.Execute()) //Запрашиваем группы спец. прав
    {
        while (qSelectSpecialParent.Next()) //Обходим группы спец. прав
        {
            const char *LocalNameParent = qSelectSpecialParent.ReadColumn(1);
            rapidjson::Value SpecialGroupObject(rapidjson::Type::kObjectType);
            SpecialGroupObject.AddMember("LocalName", JSON_STRINGA(LocalNameParent, Allocator), Allocator);

            qSelectSpecial.BindUInt(GroupID);
            qSelectSpecial.BindUID(qSelectSpecialParent.ReadColumn(0));
            if (qSelectSpecial.Execute()) //Запрашиваем спец. права
            {
                while (qSelectSpecial.Next()) //Обходим спец. права
                {
                    const char *UID = qSelectSpecial.ReadColumn(0),
                        *LocalName = qSelectSpecial.ReadColumn(1),
                        *Hint = qSelectSpecial.ReadColumn(2);

                    rapidjson::Value JsonObject(rapidjson::Type::kObjectType);
                    JsonObject.AddMember("UID", JSON_STRINGA(UID, Allocator), Allocator);
                    JsonObject.AddMember("LocalName", JSON_STRINGA(LocalName, Allocator), Allocator);
                    JsonObject.AddMember("Hint", JSON_STRINGA(Hint, Allocator), Allocator);
                    JsonObject.AddMember("IsExist", qSelectSpecial.ReadColumn_Bool(3), Allocator);

                    if (SpecialGroupObject.HasMember("Rights"))
                    {
                        SpecialGroupObject["Rights"].PushBack(JsonObject, Allocator);
                    }
                    else
                    {
                        rapidjson::Value JsonArray(rapidjson::Type::kArrayType);
                        JsonArray.PushBack(JsonObject, Allocator);
                        SpecialGroupObject.AddMember("Rights", JsonArray, Allocator);
                    }
                }
            }
            else //Ошибка запроса к спец. правам
            {
                return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSpecial"), qSelectSpecial);
            }
            SpecialArray.PushBack(SpecialGroupObject, Allocator);
        }
    }
    else //Ошибка запроса к группам спец. прав
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetGroupRights.SelectSpecialGroup"), qSelectSpecialParent);
    }

    TcpAnswer->Parameters.AddMember("Systems", SystemsArray, Allocator);
    TcpAnswer->Parameters.AddMember("RightsTableType", RightsTableTypeArray, Allocator);
    TcpAnswer->Parameters.AddMember("Tables", TablesArray, Allocator);
    TcpAnswer->Parameters.AddMember("Special", SpecialArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSubSystemAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int GroupID = 0;
    std::string SubSystemUID;
    if (!CheckIsNullUInt(TcpMessage, "GroupID", GroupID) ||
        !CheckIsNullString(TcpMessage, "UID", SubSystemUID))
    {
        return false;
    }

    //Добавляем право
    ISQuery qInsertSubSystemRight(DBConnection, QI_GROUP_RIGHT_SUBSYSTEM);
    qInsertSubSystemRight.BindUInt(GroupID);
    qInsertSubSystemRight.BindUID(SubSystemUID);
    if (!qInsertSubSystemRight.Execute()) //Ошибка запроса
    {
        if (qInsertSubSystemRight.GetErrorNumber() == 23505)
        {
            ErrorString = LANG("Carat.Error.Query.GroupRightSubSystemAdd.AlreadyExist");
            return false;
        }
        else
        {
            return ErrorQuery(LANG("Carat.Error.Query.GroupRightSubSystemAdd.Insert"), qInsertSubSystemRight);
        }
    }

    //Не удалось прочитать имя подсистемы
    if (!qInsertSubSystemRight.First())
    {
        ErrorString = qInsertSubSystemRight.GetErrorString();
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM, std::string(), std::string(), 0, qInsertSubSystemRight.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSubSystemDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int GroupID = 0;
    std::string SubSystemUID;
    if (!CheckIsNullUInt(TcpMessage, "GroupID", GroupID) ||
        !CheckIsNullString(TcpMessage, "UID", SubSystemUID))
    {
        return false;
    }

    //Удаляем право
    ISQuery qDeleteSubSystemRight(DBConnection, QD_GROUP_RIGHT_SUBSYSTEM);
    qDeleteSubSystemRight.BindUInt(GroupID);
    qDeleteSubSystemRight.BindUID(SubSystemUID);
    if (!qDeleteSubSystemRight.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GroupRightSubSystemDelete.Delete"), qDeleteSubSystemRight);
    }

    //Если ни одна строка не была затронута, значит такого права нет - ошибка
    if (qDeleteSubSystemRight.GetResultAffected() == 0)
    {
        ErrorString = LANG("Carat.Error.Query.GroupRightSubSystemDelete.NotExist");
        return false;
    }

    if (!qDeleteSubSystemRight.First()) //Не удалось переместиться на первую строку
    {
        ErrorString = qDeleteSubSystemRight.GetErrorString();
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM, std::string(), std::string(), 0, qDeleteSubSystemRight.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightTableAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int GroupID = 0;
    std::string TableName, AccessUID;
    if (!CheckIsNullUInt(TcpMessage, "GroupID", GroupID) ||
        !CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullString(TcpMessage, "AccessUID", AccessUID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Добавляем право
    ISQuery qInsertTableRight(DBConnection, QI_GROUP_RIGHT_TABLE);
    qInsertTableRight.BindUInt(GroupID);
    qInsertTableRight.BindString(MetaTable->Name);
    qInsertTableRight.BindUID(AccessUID);
    if (!qInsertTableRight.Execute()) //Вставка не удалась
    {
        if (qInsertTableRight.GetErrorNumber() == 23505)
        {
            ErrorString = LANG("Carat.Error.Query.GroupRightTableAdd.AlreadyExist");
            return false;
        }
        else
        {
            return ErrorQuery(LANG("Carat.Error.Query.GroupRightTableAdd.Insert"), qInsertTableRight);
        }
    }

    //Не удалось прочитать
    if (!qInsertTableRight.First())
    {
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE, MetaTable->Name, MetaTable->LocalListName, 0, qInsertTableRight.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightTableDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int GroupID = 0;
    std::string TableName, AccessUID;
    if (!CheckIsNullUInt(TcpMessage, "GroupID", GroupID) ||
        !CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullString(TcpMessage, "AccessUID", AccessUID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Удаляем право
    ISQuery qDeleteTableRight(DBConnection, QD_GROUP_RIGHT_TABLE);
    qDeleteTableRight.BindUInt(GroupID);
    qDeleteTableRight.BindString(MetaTable->Name);
    qDeleteTableRight.BindUID(AccessUID);
    if (!qDeleteTableRight.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.GroupRightTableDelete.Delete"), qDeleteTableRight);
    }

    //Если ни одна строка не была затронута, значит такого права нет - ошибка
    if (qDeleteTableRight.GetResultAffected() == 0)
    {
        ErrorString = LANG("Carat.Error.Query.GroupRightTableDelete.NotExist");
        return false;
    }

    if (!qDeleteTableRight.First()) //Не удалось переместиться на первую строку
    {
        ErrorString = qDeleteTableRight.GetErrorString();
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE, MetaTable->Name, MetaTable->LocalListName, 0, qDeleteTableRight.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSpecialAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int GroupID = 0;
    std::string SpecialRightUID;
    if (!CheckIsNullUInt(TcpMessage, "GroupID", GroupID) ||
        !CheckIsNullString(TcpMessage, "SpecialRightUID", SpecialRightUID))
    {
        return false;
    }

    //Добавляем спец. право
    ISQuery qInsertSpecialRight(DBConnection, QI_GROUP_RIGHT_SPECIAL);
    qInsertSpecialRight.BindUInt(GroupID);
    qInsertSpecialRight.BindUID(SpecialRightUID);
    if (!qInsertSpecialRight.Execute()) //Вставка не удалась и ошибка говорит о наружении уникальности
    {
        if (qInsertSpecialRight.GetErrorNumber() == 23505)
        {
            ErrorString = LANG("Carat.Error.Query.GroupRightSpecialAdd.AlreadyExist");
            return false;
        }
        else
        {
            return ErrorQuery(LANG("Carat.Error.Query.GroupRightSpecialAdd.Insert"), qInsertSpecialRight);
        }
    }

    if (!qInsertSpecialRight.First())
    {
        ErrorString = qInsertSpecialRight.GetErrorString();
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL, std::string(), std::string(), 0, qInsertSpecialRight.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GroupRightSpecialDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int GroupID = 0;
    std::string SpecialRightUID;
    if (!CheckIsNullUInt(TcpMessage, "GroupID", GroupID) ||
        !CheckIsNullString(TcpMessage, "SpecialRightUID", SpecialRightUID))
    {
        return false;
    }

    //Удаляем спец. право
    ISQuery qDeleteSpecialRight(DBConnection, QD_GROUP_RIGHT_SPECIAL);
    qDeleteSpecialRight.BindUInt(GroupID);
    qDeleteSpecialRight.BindUID(SpecialRightUID);
    if (!qDeleteSpecialRight.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GroupRightSpecialDelete.Delete"), qDeleteSpecialRight);
    }

    //Если ни одна строка не была затронута, значит такого права нет - ошибка
    if (qDeleteSpecialRight.GetResultAffected() == 0)
    {
        ErrorString = LANG("Carat.Error.Query.GroupRightSpecialDelete.NotExist");
        return false;
    }

    if (!qDeleteSpecialRight.First()) //Не удалось переместиться на первую строку
    {
        ErrorString = qDeleteSpecialRight.GetErrorString();
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL, std::string(), std::string(), 0, qDeleteSpecialRight.ReadColumn(0));
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRecordValue(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName, FieldName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullString(TcpMessage, "FieldName", FieldName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Получаем мета-поле
    PMetaField *MetaField = MetaTable->GetField(FieldName);
    if (!MetaField)
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetRecordValue.InvalidFieldName"), FieldName.c_str());
        return false;
    }

    //Формируем запрос и выполняем его
    ISQuery qSelectValue(DBConnection, ISAlgorithm::StringF("SELECT %s_%s FROM %s WHERE %s_id = $1",
            MetaTable->Alias.c_str(), MetaField->Name.c_str(), MetaTable->Name.c_str(), MetaTable->Alias.c_str()));
    qSelectValue.BindUInt(ObjectID);
    if (!qSelectValue.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetRecordValue.Select"), qSelectValue);
    }

    //Переходим к первой записи
    if (!qSelectValue.First())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetRecordValue.RecordNotFound"), ObjectID);
        return false;
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    if (qSelectValue.IsNull(0)) //Значение NULL
    {
        TcpAnswer->Parameters.AddMember("Value", JSON_NULL, Allocator);
    }
    else //Значение не NULL
    {
        char *Value = qSelectValue.ReadColumn(0);
        TcpAnswer->Parameters.AddMember("Value", JSON_STRINGA(Value, Allocator), Allocator);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordFavoriteAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    std::string TableName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Получаем имя записи
    std::string ObjectName;
    if (!GetObjectName(MetaTable, ObjectID, ObjectName))
    {
        return false;
    }

    //Добавляем запись в избранное
    ISQuery qInsert(DBConnection, QI_FAVORITE);
    qInsert.BindUInt(TcpMessage->TcpClient->UserID);
    qInsert.BindString(MetaTable->Name);
    qInsert.BindUInt(ObjectID);
    if (!qInsert.Execute()) //Не удалось добавить - проверяем ошибку
    {
        ErrorString = qInsert.GetErrorNumber() == 23505 ?
            LANG("Carat.Error.Query.RecordFavoriteAdd.AlreadyExist") :
            LANG("Carat.Error.Query.RecordFavoriteAdd.Insert");
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_RECORD_FAVORITE_ADD, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::RecordFavoriteDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    std::string TableName;
    unsigned int ObjectID = 0;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullUInt(TcpMessage, "ObjectID", ObjectID))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Получаем имя записи
    std::string ObjectName;
    if (!GetObjectName(MetaTable, ObjectID, ObjectName))
    {
        return false;
    }

    //Удаляем запись
    ISQuery qDelete(DBConnection, QD_FAVORITE);
    qDelete.BindUInt(TcpMessage->TcpClient->UserID);
    qDelete.BindString(MetaTable->Name);
    qDelete.BindUInt(ObjectID);
    if (!qDelete.Execute()) //Не удалось удалить
    {
        return ErrorQuery(LANG("Carat.Error.Query.RecordFavoriteDelete.Delete"), qDelete);
    }

    //Если при удалении не была затронута ни одна строка - значит такой записи нет
    if (qDelete.GetResultAffected() == 0)
    {
        ErrorString = LANG("Carat.Error.Query.RecordFavoriteDelete.NotExist");
        return false;
    }
    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_RECORD_FAVORITE_DELETE, MetaTable->Name, MetaTable->LocalListName, ObjectID, ObjectName);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetFavoriteNames(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Запрашиваем избранные записи
    ISQuery qSelectObjects(DBConnection);
    if (TcpMessage->Parameters.HasMember("TableName")) //Если таблица указана - удаляем записи по указанной таблице
    {
        //Проверяем наличие такой таблицы в мета-данных
        std::string TableName = TcpMessage->Parameters["TableName"].GetString();
        if (!GetMetaTable(TableName))
        {
            return false;
        }
        qSelectObjects.SetSqlQuery(QS_FAVORITE_NAMES_TABLE);
        qSelectObjects.BindString(TableName);
    }
    else //Таблица не указана - удаляем все записи
    {
        qSelectObjects.SetSqlQuery(QS_FAVORITE_NAMES_ALL);
    }
    qSelectObjects.BindUInt(TcpMessage->TcpClient->UserID);
    if (!qSelectObjects.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetFavoriteNames.Select"), qSelectObjects);
    }

    PMetaTable *MetaTable = nullptr;
    std::string ObjectName;

    //Обходим
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    rapidjson::Value JsonArray(rapidjson::Type::kArrayType);
    while (qSelectObjects.Next())
    {
        std::string TableName = qSelectObjects.ReadColumn_String(0);
        if (!MetaTable || MetaTable->Name != TableName) //Если мета-таблица ещё не получена или её имя сменилось - получаем её
        {
            MetaTable = GetMetaTable(TableName);
            if (!MetaTable) //Не удалось получить мета-таблицу - выходим с ошибкой
            {
                return false;
            }
        }

        //Получаем имя записи
        unsigned int ObjectID = qSelectObjects.ReadColumn_UInt(1);
        if (!GetObjectName(MetaTable, ObjectID, ObjectName))
        {
            return false;
        }

        //Добавляем в результирующий список
        rapidjson::Value JsonObject(rapidjson::Type::kObjectType);
        JsonObject.AddMember("TableName", JSON_STRINGA(TableName.c_str(), Allocator), Allocator);
        JsonObject.AddMember("TableLocalName", JSON_STRINGA(MetaTable->LocalListName.c_str(), Allocator), Allocator);
        JsonObject.AddMember("ObjectID", ObjectID, Allocator);
        JsonObject.AddMember("ObjectName", JSON_STRINGA(ObjectName.c_str(), Allocator), Allocator);
        JsonArray.PushBack(JsonObject, Allocator);
    }
    TcpAnswer->Parameters.AddMember("Names", JsonArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::FavoritesDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    //Удаляем избранные записи
    ISQuery qDelete(DBConnection);
    if (TcpMessage->Parameters.HasMember("TableName")) //Если таблица указана - удаляем записи по указанной таблице
    {
        //Проверяем наличие такой таблицы в мета-данных
        std::string TableName = TcpMessage->Parameters["TableName"].GetString();
        if (!GetMetaTable(TableName))
        {
            return false;
        }
        qDelete.SetSqlQuery(QD_FAVORITES_TABLE);
        qDelete.BindString(TableName);
    }
    else //Таблица не указана - удаляем все записи
    {
        qDelete.SetSqlQuery(QD_FAVORITES_ALL);
    }
    qDelete.BindUInt(TcpMessage->TcpClient->UserID);
    if (!qDelete.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.FavoritesDelete.Delete"), qDelete);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CalendarClose(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);
    return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetHistoryList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    //Получаем данные
    ISQuery qSelect(DBConnection, QS_HISTORY);
    qSelect.BindUInt(TcpMessage->TcpClient->UserID);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        ErrorString = LANG("Carat.Error.Query.GetHistoryList.Select");
        return false;
    }

    PMetaTable *MetaTable = nullptr;
    std::string ObjectName;

    //Обходим выборку
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    rapidjson::Value HistoryArray(rapidjson::Type::kArrayType);
    while (qSelect.Next())
    {
        std::string TableName = qSelect.ReadColumn_String(1);
        if (!MetaTable || MetaTable->Name != TableName) //Если мета-таблица ещё не получена или её имя сменилось - получаем её
        {
            MetaTable = GetMetaTable(TableName);
            if (!MetaTable) //Не удалось получить мета-таблицу - выходим с ошибкой
            {
                return false;
            }
        }

        //Получаем имя записи
        unsigned int ObjectID = qSelect.ReadColumn_UInt(2);
        if (!GetObjectName(MetaTable, ObjectID, ObjectName))
        {
            return false;
        }

        std::string StringDT = ISTcpWorkerHelper::ConvertDateTimeToString(qSelect.ReadColumn_DateTime(0));

        //Добавляем в результирующий список
        rapidjson::Value JsonObject(rapidjson::Type::kObjectType);
        JsonObject.AddMember("DateTime", JSON_STRINGA(StringDT.c_str(), Allocator), Allocator);
        JsonObject.AddMember("TableLocalName", JSON_STRINGA(MetaTable->LocalListName.c_str(), Allocator), Allocator);
        JsonObject.AddMember("TableName", JSON_STRINGA(MetaTable->Name.c_str(), Allocator), Allocator);
        JsonObject.AddMember("ObjectID", ObjectID, Allocator);
        JsonObject.AddMember("ObjectName", JSON_STRINGA(ObjectName.c_str(), Allocator), Allocator);
        HistoryArray.PushBack(JsonObject, Allocator);
    }
    TcpAnswer->Parameters.AddMember("History", HistoryArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetForeignList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string TableName, FieldName;
    if (!CheckIsNullString(TcpMessage, "TableName", TableName) ||
        !CheckIsNullString(TcpMessage, "FieldName", FieldName))
    {
        return false;
    }

    //Получаем мета-таблицу
    PMetaTable *MetaTable = GetMetaTable(TableName);
    if (!MetaTable)
    {
        return false;
    }

    //Получаем мета-поле
    PMetaField *MetaField = MetaTable->GetField(FieldName);
    if (!MetaField)
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetForeignList.FieldNotExist"), FieldName.c_str(), MetaTable->Name.c_str());
        return false;
    }

    //Проверяем наличие внешнего ключа по поле
    if (!MetaField->Foreign)
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.GetForeignList.FieldNotForeign"), FieldName.c_str());
        return false;
    }

    //Формируем запрос
    PMetaTable *MetaTableForeign = ISMetaData::Instance().GetTable(MetaField->Foreign->ForeignClass); //Таблица на которую ссылается внешний ключ
    ISVectorString VectorString = ISAlgorithm::StringSplit(MetaField->Foreign->ForeignViewNameField, ';');

    std::string SqlText = "SELECT " + MetaTableForeign->Alias + '_' + MetaField->Foreign->ForeignField + " AS ID, concat(";
    for (const std::string &String : VectorString) //Обход полей (которые должны быть отображены)
    {
        SqlText += MetaTableForeign->Alias + '_' + String + ", ' ',";
    }
    ISAlgorithm::StringChop(SqlText, 6);
    SqlText += ") ";
    SqlText += "AS Value\n";
    SqlText += "FROM " + MetaTableForeign->Name + "\n";
    SqlText += "ORDER BY 2 ASC";

    //Выполняем запрос
    ISQuery qSelect(DBConnection, SqlText);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetForeignList.Select"), qSelect);
    }

    //Обходим результаты запроса
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    rapidjson::Value JsonArray(rapidjson::Type::kArrayType);
    while (qSelect.Next())
    {
        const char *Value = qSelect.ReadColumn(1);

        rapidjson::Value JsonObject(rapidjson::Type::kObjectType);
        JsonObject.AddMember("ID", qSelect.ReadColumn_UInt(0), Allocator);
        JsonObject.AddMember("Value", JSON_STRINGA(Value, Allocator), Allocator);
        JsonArray.PushBack(JsonObject, Allocator);
    }
    TcpAnswer->Parameters.AddMember("List", JsonArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetServerInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);

    //Делаем запрос к БД
    ISQuery qSelect(DBConnection, QS_SERVER_INFO);
    if (!qSelect.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetServerInfo.SelectInfo"), qSelect);
    }

    if (!qSelect.First())
    {
        ErrorString = qSelect.GetErrorString();
        return false;
    }

    const char *DatabaseSizeFull = qSelect.ReadColumn(0);
    const char *DatabaseSizeIndexes = qSelect.ReadColumn(1);
    const char *DatabaseSizeData = qSelect.ReadColumn(2);
    const char *DatabaseOwner = qSelect.ReadColumn(3);
    const char *DatabaseEncoding = qSelect.ReadColumn(4);
    const char *DatabaseUptime = qSelect.ReadColumn(5);
    unsigned int DatabasePID = qSelect.ReadColumn_UInt(6);
    const char *DatabaseVersion = qSelect.ReadColumn(7);
    std::string DatabaseClusterPath = qSelect.ReadColumn(8);
    std::string DatabaseSizeLogs = ISAlgorithm::StringFromSize(ISAlgorithm::DirSize(DatabaseClusterPath + PATH_SEPARATOR + "log"));
    std::string DatabaseCountTable = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(9));
    std::string DatabaseCountField = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(10));
    std::string DatabaseCountSequence = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(11));
    std::string DatabaseCountIndexes = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(12));
    std::string DatabaseCountForeign = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(13));
    std::string DatabaseRowsCount = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(14));
    std::string DatabaseCountProtocol = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(15));
    std::string DatabaseUsersCount = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(16));
    std::string DatabaseFilesCount = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Int64(17));
    std::string DatabaseFilesSize = ISAlgorithm::StringFromSize(qSelect.ReadColumn_UInt64(18));

    std::string StartedDateTime = ISDateTime::FromUnixTime(ISProperty::Instance().GetUptime()).ToString();
    std::string Uptime = ISTcpWorkerHelper::GetUptime();
    std::string SizeLogs = ISAlgorithm::StringFromSize(ISAlgorithm::DirSize(ISAlgorithm::GetApplicationDir() + PATH_SEPARATOR + "Logs"));

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    rapidjson::Value CaratObject(rapidjson::Type::kObjectType);
    CaratObject.AddMember("Version", CARAT_VERSION_N, Allocator);
    CaratObject.AddMember("StartedDateTime", JSON_STRINGA(StartedDateTime.c_str(), Allocator), Allocator);
    CaratObject.AddMember("Uptime", JSON_STRINGA(Uptime.c_str(), Allocator), Allocator);
    CaratObject.AddMember("SizeLogs", JSON_STRINGA(SizeLogs.c_str(), Allocator), Allocator);
    CaratObject.AddMember("CountClients", ISTcpClients::Instance().GetCount(), Allocator);
    TcpAnswer->Parameters.AddMember("Carat", CaratObject, Allocator);

    rapidjson::Value DatabaseObject(rapidjson::Type::kObjectType);
    DatabaseObject.AddMember("SizeFull", JSON_STRINGA(DatabaseSizeFull, Allocator), Allocator);
    DatabaseObject.AddMember("SizeIndexes", JSON_STRINGA(DatabaseSizeIndexes, Allocator), Allocator);
    DatabaseObject.AddMember("SizeData", JSON_STRINGA(DatabaseSizeData, Allocator), Allocator);
    DatabaseObject.AddMember("Owner", JSON_STRINGA(DatabaseOwner, Allocator), Allocator);
    DatabaseObject.AddMember("Encoding", JSON_STRINGA(DatabaseEncoding, Allocator), Allocator);
    DatabaseObject.AddMember("Uptime", JSON_STRINGA(DatabaseUptime, Allocator), Allocator);
    DatabaseObject.AddMember("PID", DatabasePID, Allocator);
    DatabaseObject.AddMember("Version", JSON_STRINGA(DatabaseVersion, Allocator), Allocator);
    DatabaseObject.AddMember("ClusterPath", JSON_STRINGA(DatabaseClusterPath.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("SizeLogs", JSON_STRINGA(DatabaseSizeLogs.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("CountTable", JSON_STRINGA(DatabaseCountTable.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("CountField", JSON_STRINGA(DatabaseCountField.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("CountSequence", JSON_STRINGA(DatabaseCountSequence.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("CountIndexes", JSON_STRINGA(DatabaseCountIndexes.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("CountForeign", JSON_STRINGA(DatabaseCountForeign.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("RowsCount", JSON_STRINGA(DatabaseRowsCount.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("ProtocolCount", JSON_STRINGA(DatabaseCountProtocol.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("UsersCount", JSON_STRINGA(DatabaseUsersCount.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("FilesCount", JSON_STRINGA(DatabaseFilesCount.c_str(), Allocator), Allocator);
    DatabaseObject.AddMember("FilesSize", JSON_STRINGA(DatabaseFilesSize.c_str(), Allocator), Allocator);
    TcpAnswer->Parameters.AddMember("Database", DatabaseObject, Allocator);

    Protocol(TcpMessage->TcpClient->UserID, CONST_UID_PROTOCOL_SERVER_INFO);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OrganizationFromINN(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    std::string INN;
    if (!CheckIsNullString(TcpMessage, "INN", INN))
    {
        return false;
    }

    //Ошибка инициализации CURL
    CURL *Curl = curl_easy_init();
    if (!Curl)
    {
        ErrorString = LANG("Carat.Error.Query.OrganizationFormINN.Init");
        return false;
    }

    //Формируем заголовок запроса
    curl_slist *CurlList = nullptr;
    CurlList = curl_slist_append(CurlList, "Host: suggestions.dadata.ru");
    CurlList = curl_slist_append(CurlList, "Content-Type: application/json");
    CurlList = curl_slist_append(CurlList, "Accept: application/json");
    CurlList = curl_slist_append(CurlList, ISAlgorithm::StringF("Authorization: Token %s", TOKEN_DA_DATA_TOKEN).c_str());

    std::string Result,
        QueryString = ISAlgorithm::StringF("{\"query\":\"%s\"}", INN.c_str());

    //Настраиваем запрос
    curl_easy_setopt(Curl, CURLOPT_URL, "http://suggestions.dadata.ru/suggestions/api/4_1/rs/findById/party");
    curl_easy_setopt(Curl, CURLOPT_HTTPHEADER, CurlList);
    curl_easy_setopt(Curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, &ISTcpWorkerHelper::WriteFunctionINN);
    curl_easy_setopt(Curl, CURLOPT_WRITEDATA, &Result);
    curl_easy_setopt(Curl, CURLOPT_POSTFIELDS, QueryString.c_str());
    
    //Выполняем
    CURLcode res = curl_easy_perform(Curl);
    curl_easy_cleanup(Curl);
    if (res != CURLE_OK)
    {
        ErrorString = curl_easy_strerror(res);
        return false;
    }
    
    rapidjson::Document JsonDocument;
    JsonDocument.Parse(Result.c_str(), Result.size());
    rapidjson::ParseErrorCode ParseError = JsonDocument.GetParseError();
    if (ParseError != rapidjson::ParseErrorCode::kParseErrorNone)
    {
        ErrorString = GetParseError_En(ParseError);
        return false;
    }

    if (!JsonDocument.IsObject())
    {
        ErrorString = LANG("Carat.Error.Query.OrganizationFormINN.DocumentIsNotObject");
        return false;
    }

    rapidjson::Value &JsonArray = JsonDocument["suggestions"];
    if (!JsonArray.IsArray())
    {
        ErrorString = LANG("Carat.Error.Query.OrganizationFormINN.ResultIsNotArray");
        return false;
    }

    //Организация не найдена
    if (JsonArray.Empty())
    {
        ErrorString = ISAlgorithm::StringF(LANG("Carat.Error.Query.OrganizationFormINN.NotFound"), INN.c_str());
        return false;
    }

    rapidjson::Value &JsonObject = JsonArray.Begin()->GetObject();
    rapidjson::Value Value;
    Value.CopyFrom(JsonObject, TcpAnswer->Parameters.GetAllocator());
    TcpAnswer->Parameters.AddMember("Reply", Value, TcpAnswer->Parameters.GetAllocator());
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_PeriodContains(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);

    ISQuery qSelect(DBConnection, QS_PERIOD);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.PeriodContains.Select"), qSelect);
    }
    bool IsExist = qSelect.First();
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    TcpAnswer->Parameters.AddMember("Exist", IsExist, Allocator);
    TcpAnswer->Parameters.AddMember("Value", IsExist ? rapidjson::Value(qSelect.ReadColumn_Double(0)) : JSON_NULL, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_GetStockList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);

    ISQuery qSelect(DBConnection, QS_STOCK);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetStockList.Select"), qSelect);
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Обходим результаты выборки
    rapidjson::Value StockArray(rapidjson::Type::kArrayType);
    while (qSelect.Next())
    {
        const char *StockName = qSelect.ReadColumn(1);

        rapidjson::Value StockObject(rapidjson::Type::kObjectType);
        StockObject.AddMember("ID", qSelect.ReadColumn_UInt(0), Allocator);
        StockObject.AddMember("Name", JSON_STRINGA(StockName, Allocator), Allocator);
        StockArray.PushBack(StockObject, Allocator);
    }
    TcpAnswer->Parameters.AddMember("List", StockArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_StatementAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpAnswer);

    unsigned int ImplementationUnload = 0, UnloadStock = 0;
    double ValumeIncome = 0;
    if (!CheckIsNullUInt(TcpMessage, "ImplementationUnload", ImplementationUnload) ||
        !CheckIsNullUInt(TcpMessage, "UnloadStock", UnloadStock) ||
        !CheckIsNullDouble(TcpMessage, "ValumeIncome", ValumeIncome))
    {
        return false;
    }

    //Проверяем наличие такой записи
    ISQuery qSelectUnload(DBConnection, QS_STATEMENT);
    qSelectUnload.BindUInt(ImplementationUnload);
    if (!qSelectUnload.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.StatementAdd.Select"), qSelectUnload);
    }

    if (!qSelectUnload.First()) //Не удалось перейти на первую запись
    {
        ErrorString = qSelectUnload.GetErrorString();
        return false;
    }

    //Если выгрузки в ведомостях АЗС нет - добавляем
    if (qSelectUnload.ReadColumn_UInt(0) == 0)
    {
        ISQuery qInsert(DBConnection, QI_STATEMENT);
        qInsert.BindUInt(ImplementationUnload);
        qInsert.BindUInt(UnloadStock);
        qInsert.BindDouble(ValumeIncome);
        if (!qInsert.Execute()) //Не удалось добавить
        {
            return ErrorQuery(LANG("Carat.Error.Query.StatementAdd.Insert"), qInsert);
        }
    }

    //Выгрузка уже существует - все окей
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_GetGasStation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    unsigned int StatementID = 0;
    if (!CheckIsNullUInt(TcpMessage, "StatementID", StatementID))
    {
        return false;
    }

    //Вытаскиваем значения
    ISQuery qSelect(DBConnection, QS_FILL_IN_BASED);
    qSelect.BindUInt(StatementID);
    if (!qSelect.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetGasStation.Select"), qSelect);
    }

    if (!qSelect.First()) //Такая запись не существует
    {
        ErrorString = LANG("Carat.Error.Query.GetGasStation.NotExist");
        return false;
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    TcpAnswer->Parameters.AddMember("BalanceEndChange", qSelect.ReadColumn_Double(0), Allocator);
    TcpAnswer->Parameters.AddMember("CashboxTotalPayment", qSelect.ReadColumn_Double(1), Allocator);
    TcpAnswer->Parameters.AddMember("CashboxTotalActually", qSelect.ReadColumn_Double(2), Allocator);
    TcpAnswer->Parameters.AddMember("CashboxKKMTotal", qSelect.ReadColumn_Double(3), Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_GetDebtImplementation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    unsigned int CounterpartyID = 0;
    if (!CheckIsNullUInt(TcpMessage, "CounterpartyID", CounterpartyID))
    {
        return false;
    }

    //Вытаскиваем загрузки и выгрузки
    ISQuery qSelectLoadUnload(DBConnection, QS_IMPLEMENTATION_UNLOAD);
    qSelectLoadUnload.BindUInt(CounterpartyID);
    if (!qSelectLoadUnload.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetDebtImplementation.Select"), qSelectLoadUnload);
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Обходим результаты выборки
    rapidjson::Value LoadUnloadArray(rapidjson::Type::kArrayType);
    while (qSelectLoadUnload.Next())
    {
        std::string Date = qSelectLoadUnload.ReadColumn_Date(3).ToString();

        rapidjson::Value JsonObject(rapidjson::Type::kObjectType);
        JsonObject.AddMember("IsLoad", qSelectLoadUnload.ReadColumn_Bool(0), Allocator);
        JsonObject.AddMember("ImplementationID", qSelectLoadUnload.ReadColumn_UInt(1), Allocator);
        JsonObject.AddMember("LoadUnloadID", qSelectLoadUnload.ReadColumn_UInt(2), Allocator);
        JsonObject.AddMember("Date", JSON_STRINGA(Date.c_str(), Allocator), Allocator);
        JsonObject.AddMember("Cost", qSelectLoadUnload.ReadColumn_Double(4), Allocator);
        LoadUnloadArray.PushBack(JsonObject, Allocator);
    }
    TcpAnswer->Parameters.AddMember("LoadUnload", LoadUnloadArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_GetDebtCounterparty(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    unsigned int CounterpartyID = 0;
    if (!CheckIsNullUInt(TcpMessage, "CounterpartyID", CounterpartyID))
    {
        return false;
    }

    //Запрашиваем итоговые цифры
    ISQuery qSelectTitle(DBConnection, QS_COUNTERPARTY_DEBT);
    qSelectTitle.BindUInt(CounterpartyID);
    if (!qSelectTitle.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetDebtCounterparty.Select"), qSelectTitle);
    }

    if (!qSelectTitle.First()) //Такая запись не существует
    {
        ErrorString = LANG("Carat.Error.Query.GetDebtCounterparty.NotExist");
        return false;
    }
    auto &Allocator = TcpAnswer->Parameters.GetAllocator();
    TcpAnswer->Parameters.AddMember("TotalUnload", qSelectTitle.ReadColumn_Double(0), Allocator);
    TcpAnswer->Parameters.AddMember("TotalLoad", qSelectTitle.ReadColumn_Double(1), Allocator);
    TcpAnswer->Parameters.AddMember("TotalEntrollment", qSelectTitle.ReadColumn_Double(2), Allocator);
    TcpAnswer->Parameters.AddMember("MoveWagonSum", qSelectTitle.ReadColumn_Double(3), Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_GetUserConsumption(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);

    //Запрашиваем всех пользователей и их балансы
    ISQuery qSelectUsers(DBConnection, QS_USERS_CONSUMPTION);
    if (!qSelectUsers.Execute()) //Ошибка запроса
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetUserConsumption.SelectUsers"), qSelectUsers);
    }

    auto &Allocator = TcpAnswer->Parameters.GetAllocator();

    //Обходим пользователей
    rapidjson::Value UserArray(rapidjson::Type::kArrayType);
    while (qSelectUsers.Next())
    {
        unsigned int UserID = qSelectUsers.ReadColumn_UInt(0);

        //Запрашиваем все расходы пользователя
        ISQuery qSelectUserConsumption(DBConnection, QS_USER_CONSUMPTION);
        qSelectUserConsumption.BindUInt(UserID);
        if (!qSelectUserConsumption.Execute()) //Ошибка запроса
        {
            return ErrorQuery(LANG("Carat.Error.Query.GetUserConsumption.UserConsumption"), qSelectUserConsumption);
        }

        //Обходим расходы пользователя
        rapidjson::Value ConsumptionArray(rapidjson::Type::kArrayType);
        while (qSelectUserConsumption.Next())
        {
            std::string Date = ISTcpWorkerHelper::ConvertDateToString(qSelectUserConsumption.ReadColumn_Date(0)),
                Note = qSelectUserConsumption.ReadColumn_String(2);

            rapidjson::Value ConsumptionObject(rapidjson::Type::kObjectType);
            ConsumptionObject.AddMember("Date", JSON_STRINGA(Date.c_str(), Allocator), Allocator);
            ConsumptionObject.AddMember("Sum", qSelectUserConsumption.ReadColumn_Double(1), Allocator);
            ConsumptionObject.AddMember("Note", JSON_STRINGA(Note.c_str(), Allocator), Allocator);
            ConsumptionArray.PushBack(ConsumptionObject, Allocator);
        }

        //Запрашиваем все возвраты пользователя
        ISQuery qSelectUserReturn(DBConnection, QS_USER_RETURN);
        qSelectUserReturn.BindUInt(UserID);
        if (!qSelectUserReturn.Execute())
        {
            return ErrorQuery(LANG("Carat.Error.Query.GetUserConsumption.UserReturn"), qSelectUserReturn);
        }

        //Обходим возраты пользователя
        rapidjson::Value ReturnArray(rapidjson::Type::kArrayType);
        while (qSelectUserReturn.Next())
        {
            std::string Date = ISTcpWorkerHelper::ConvertDateToString(qSelectUserReturn.ReadColumn_Date(0)),
                Note = qSelectUserReturn.ReadColumn_String(2);

            rapidjson::Value ReturnObject(rapidjson::Type::kObjectType);
            ReturnObject.AddMember("Date", JSON_STRINGA(Date.c_str(), Allocator), Allocator);
            ReturnObject.AddMember("Sum", qSelectUserReturn.ReadColumn_Double(1), Allocator);
            ReturnObject.AddMember("Note", JSON_STRINGA(Note.c_str(), Allocator), Allocator);
        }

        const char *UserFIO = qSelectUsers.ReadColumn(1);
        std::string Balance = ISAlgorithm::FormatNumber(qSelectUsers.ReadColumn_Double(2), ' ', 2);

        rapidjson::Value UserObject(rapidjson::Type::kObjectType);
        UserObject.AddMember("ID", UserID, Allocator);
        UserObject.AddMember("FIO", JSON_STRINGA(UserFIO, Allocator), Allocator);
        UserObject.AddMember("Balance", JSON_STRINGA(Balance.c_str(), Allocator), Allocator);
        UserObject.AddMember("ConsumptionList", ConsumptionArray, Allocator);
        UserObject.AddMember("ReturnList", ReturnArray, Allocator);
        UserArray.PushBack(UserObject, Allocator);
    }

    //Получаем итоговые цифры
    ISQuery qSelectTotalBalance(DBConnection, QS_TOTAL_BALANCE);
    if (!qSelectTotalBalance.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetUserConsumption.SelectTotal"), qSelectTotalBalance);
    }
    qSelectTotalBalance.First();

    double TotalComing = qSelectTotalBalance.ReadColumn_Double(0),
        TotalConsumption = qSelectTotalBalance.ReadColumn_Double(1);
    TcpAnswer->Parameters.AddMember("TotalComing", TotalComing, Allocator);
    TcpAnswer->Parameters.AddMember("TotalConsumption", TotalConsumption, Allocator);
    TcpAnswer->Parameters.AddMember("Balance", TotalComing - TotalConsumption, Allocator);
    TcpAnswer->Parameters.AddMember("UserList", UserArray, Allocator);
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_LoadBanks(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    IS_UNUSED(TcpMessage);
    IS_UNUSED(TcpAnswer);

    /*QVariant ByteArray = CheckNullField("ByteArray", TcpMessage);
    if (!ByteArray.isValid())
    {
        return false;
    }

    QByteArray b = QByteArray::fromBase64(ByteArray.toByteArray());
    QString Content = QString::fromUtf8(b);

    QStringList StringList = Content.split('\n');
    StringList.removeFirst(); //Удаляем заголовки
    StringList.removeAll(QString()); //Удаляем пустые строки

    int Loaded = 0, Invalid = 0;

    //Подготовим запросы и обойдём все записи
    ISQuery qInsert(ISDatabase::Instance().GetDB(DBConnectionName), QI_BANK),
        qSelect(ISDatabase::Instance().GetDB(DBConnectionName), QS_BANK);
    for (QString &String : StringList)
    {
        QStringList Values = String.split('\t');
        Values.removeFirst(); //Удаляем первое поле "Есть файлы"
        if (Values.size() != 11) //Запись не валидная
        {
            ++Invalid;
            continue;
        }

        QString StringDate = Values[0],
            StringAdmission = Values[1],
            StringWriteOff = Values[2],
            StringPurposePayment = Values[3],
            StringCounterparty = Values[4],
            StringCheckNumber = Values[5],
            StringOperationType = Values[6],
            StringIncomingNumber = Values[7],
            StringIncomingDate = Values[8],
            StringBankAccount = Values[9],
            StringComment = Values[10];

        //Дата
        QDate Date = QDate::fromString(Values[0], FORMAT_DATE_V2);

        //Поступление
        bool AdmissionOK = false;
        StringAdmission.remove(160);
        StringAdmission.replace(',', '.');
        double Admission = StringAdmission.toDouble(&AdmissionOK);

        //Списание
        bool WriteOffOK = false;
        StringWriteOff.remove(160);
        StringWriteOff.replace(',', '.');
        double WriteOff = StringWriteOff.toDouble(&WriteOffOK);

        //Входящая дата
        QDate IncomingDate = QDate::fromString(StringIncomingDate, FORMAT_DATE_V2);

        //Проверяем наличие такой записи
        qSelect.BindValue(":Date", Date);
        qSelect.BindValue(":IncomingNumber", StringIncomingNumber);
        if (!qSelect.Execute()) //Не удалось проверить наличие записи
        {
            return ErrorQuery(LANG("Carat.Error.Query.LoadBank.Select"), qSelect);
        }

        if (!qSelect.First()) //Не удалось перейти на первую запись
        {
            ErrorString = qSelect.GetErrorString();
            return false;
        }

        //Если такая запись уже есть - переходим к следующей
        if (qSelect.ReadColumn("count").toInt() > 0)
        {
            continue;
        }

        qInsert.BindValue(":Date", Date);
        qInsert.BindValue(":Admission", AdmissionOK ? Admission : QVariant());
        qInsert.BindValue(":WriteOff", WriteOffOK ? WriteOff : QVariant());
        qInsert.BindValue(":PurposePayment", StringPurposePayment.isEmpty() ? QVariant() : StringPurposePayment);
        qInsert.BindValue(":Counterparty", StringCounterparty.isEmpty() ? QVariant() : StringCounterparty);
        qInsert.BindValue(":CheckNumber", StringCheckNumber.isEmpty() ? QVariant() : StringCheckNumber);
        qInsert.BindValue(":OperationType", StringOperationType.isEmpty() ? QVariant() : StringOperationType);
        qInsert.BindValue(":IncomingNumber", StringIncomingNumber.isEmpty() ? QVariant() : StringIncomingNumber);
        qInsert.BindValue(":IncomingDate", IncomingDate);
        qInsert.BindValue(":BankAccount", StringBankAccount.isEmpty() ? QVariant() : StringBankAccount);
        qInsert.BindValue(":Comment", StringComment.isEmpty() ? QVariant() : StringComment);
        if (!qInsert.Execute()) //Не удалось добавить запись
        {
            return ErrorQuery(LANG("Carat.Error.Query.LoadBank.Insert").arg(Values.join(' ')), qInsert);
        }
        ++Loaded;
    }
    TcpAnswer->Parameters["Loaded"] = Loaded;
    TcpAnswer->Parameters["Invalid"] = Invalid;
    TcpAnswer->Parameters["Total"] = StringList.size();
    Protocol(TcpMessage->TcpSocket->GetUserID(), "{85DCCA5C-723E-4E18-8286-FF33D12C6F4D}");*/
    return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::OilSphere_GetUserBalance(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
    ISQuery qSelect(DBConnection, QS_USER_BALANCE);
    qSelect.BindUInt(TcpMessage->TcpClient->UserID);
    if (!qSelect.Execute())
    {
        return ErrorQuery(LANG("Carat.Error.Query.GetUserBalance.Select"), qSelect);
    }

    if (!qSelect.First())
    {
        ErrorString = qSelect.GetErrorString();
        return false;
    }
    std::string String = ISAlgorithm::FormatNumber(qSelect.ReadColumn_Double(0), ' ', 2);
    TcpAnswer->Parameters.AddMember("Balance", JSON_STRINGA(String.c_str(), TcpAnswer->Parameters.GetAllocator()), TcpAnswer->Parameters.GetAllocator());
    return true;
}
//-----------------------------------------------------------------------------
