#pragma once
#ifndef _ISCONSTANTS_H_INCLUDED
#define _ISCONSTANTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
const char PATH_SEPARATOR = '\\';
#else
const size_t MAX_PATH = 260;
const char PATH_SEPARATOR = '/';
#endif
//EXTENSIONS-------------------------------------------------------------------
const char EXTENSION_LOG[] = "log";
const char EXTENSION_PNG[] = "png";
const char EXTENSION_JPEG[] = "jpeg";
const char EXTENSION_JPG[] = "jpg";
const char EXTENSION_DDS[] = "dds";
const char EXTENSION_GIF[] = "gif";
const char EXTENSION_ICNS[] = "icns";
const char EXTENSION_ICO[] = "ico";
const char EXTENSION_SVG[] = "svg";
const char EXTENSION_TGA[] = "tga";
const char EXTENSION_TIFF[] = "tiff";
const char EXTENSION_WBMP[] = "wbmp";
const char EXTENSION_BMP[] = "bmp";
const char EXTENSION_WEBP[] = "webp";
const char EXTENSION_LANG[] = "lang";
const char EXTENSION_XML[] = "xml";
const char EXTENSION_INI[] = "ini";
const char EXTENSION_CRASH[] = "crash";
const char EXTENSION_PID[] = "pid";
const char EXTENSION_OLD[] = "old";
const char EXTENSION_WAV[] = "wav";
#ifdef WIN32
const char EXTENSION_BINARY[] = ".exe"; //���������� ������������ ����� ��� ������� ������������ �������
const char EXTENSION_LIBRARY[] = ".dll"; //���������� ������������ ���������� ��� ������� ������������ �������
#else
const char EXTENSION_BINARY[] = ""; //���������� ������������ ����� ��� ������� ������������ �������
const char EXTENSION_LIBRARY[] = ".so"; //���������� ������������ ���������� ��� ������� ������������ �������
#endif
//LOCALIZATION-----------------------------------------------------------------
const char LOCALIZATION_FILE_INTEGRAL_SYSTEM[] = "IntegralSystem";
const char LOCALIZATION_FILE_OBJECTS[] = "Objects";
const char LOCALIZATION_FILE_CARAT[] = "Carat";
const char LOCALIZATION_FILE_CONFIGURATOR[] = "Configurator";
//REG_EXP----------------------------------------------------------------------
const char REG_EXP_UID[] = "UID=\"\\{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}\\}\""; //���������� ������������ �� ����-������
const char REG_EXP_LOGIN[] = "^([0-9A-Za-z._]+)$";
const char REG_EXP_URL[] = "^(http:\\/\\/www\\.|https:\\/\\/www\\.|http:\\/\\/|https:\\/\\/)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/.*)?$"; //������ �� ���� ��� �������� � ���������
//DATE_TIME_FORMATS------------------------------------------------------------
const char FORMAT_DATE_V1[] =		"d MMMM yyyy";					//30 ���� 2020
const char FORMAT_DATE_V2[] =		"dd.MM.yyyy";					//30.07.2020
const char FORMAT_DATE_V3[] =		"dd/MM/yyyy";					//30/07/2020
const char FORMAT_DATE_V4[] =		"d MMMM yyyy (ddd)";			//30 ���� 2020 (��)
const char FORMAT_TIME_V1[] =		"hh:mm";						//10:16
const char FORMAT_TIME_V2[] =		"hh.mm";						//10.16
const char FORMAT_TIME_V3[] =		"hh:mm:ss";						//10:16:13
const char FORMAT_TIME_V4[] =		"mm:ss";						//16:13
const char FORMAT_TIME_V5[] =		"hh.mm.ss";						//10.16.13
const char FORMAT_DATE_TIME_V1[] =	"d MMMM yyyy hh:mm";			//4 ������ 2020 12:59
const char FORMAT_DATE_TIME_V2[] =	"dd.MM.yyyy hh:mm";				//04.08.2020 12:59
const char FORMAT_DATE_TIME_V3[] =	"d MMMM yyyy hh:mm:ss";			//4 ������ 2020 12:59:52
const char FORMAT_DATE_TIME_V4[] =	"dd.MM.yyyy hh:mm:ss.zzz";		//04.08.2020 12:59:52.459
const char FORMAT_DATE_TIME_V5[] =	"d MMMM yy hh:mm:ss.zzz";		//4 ������ 20 12:59:52.459
const char FORMAT_DATE_TIME_V6[] =	"dd.MM.yyyy hh.mm";				//04.08.2020 12.59
const char FORMAT_DATE_TIME_V7[] =	"yyyy-MM-dd hh:mm:ss";			//2020-08-04 12:59:52
const char FORMAT_DATE_TIME_V8[] =	"dd.MM.yyyy_hh.mm.ss";			//04.08.2020_12.59.52
const char FORMAT_DATE_TIME_V9[] =	"dd.MM.yy hh:mm:ss.zzz";		//04.08.20 12:59:52.459
const char FORMAT_DATE_TIME_V10[] =	"d MMMM yyyy (ddd) hh:mm:ss";	//4 ������ (��) 2020 12:59:52
//TOKENS-----------------------------------------------------------------------
const char TOKEN_DA_DATA_TOKEN[] = "f2c204cd6ba093b294324acec5d01b25fad632cf";
//-----------------------------------------------------------------------------
const char PATH_SCHEMA_TEMPLATE_FIELDS[] = ":Other/ClassTemplateFields.xml";
const char PATH_CONFIGURATOR_SCHEME[] = ":Other/Configurator.xml";
const char PATH_BUILD_INFO[] = ":Build/Build.ini.tmp";
//REPORT_TYPE------------------------------------------------------------------
const char REPORT_TYPE_HTML[] = "Html";
//CURSORS----------------------------------------------------------------------
const Qt::CursorShape CURSOR_ARROW = Qt::ArrowCursor;
const Qt::CursorShape CURSOR_POINTING_HAND = Qt::PointingHandCursor;
const Qt::CursorShape CURSOR_WHATS_THIS = Qt::WhatsThisCursor;
const Qt::CursorShape CURSOR_WAIT = Qt::WaitCursor;
const Qt::CursorShape CURSOR_BUSY = Qt::BusyCursor;
//INTERFACE--------------------------------------------------------------------
const int EDIT_FIELD_SPACING = 4;
const int ISPUSHBUTTON_MINIMUM_WIDTH = 98; //����������� ������ ������� ������
const int ISPUSHBUTTON_MINIMUM_HEIGHT = 28; //����������� ������ ������� ������
const int SIZE_MINIMUM_HEIGHT_EDIT_FIELD = ISPUSHBUTTON_MINIMUM_HEIGHT; //����������� ������ ���� ����� ����� ����������
//SYMBOLS----------------------------------------------------------------------
const char SYMBOL_SPACE = ' '; //������ �������
const char SYMBOL_MINUS = '-'; //������ "�����"
const char SYMBOL_COMMA = ','; //������ "�������"
const char SYMBOL_POINT = '.'; //������ "�����"
const char SYMBOL_STAR = '*'; //������ ���������
const char SYMBOL_NULL_TERM = '\0'; //������ ����� ������
const char SYMBOL_SPECIAL_ARRAY[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
//DATABASE_CONNECTIONS---------------------------------------------------------
const char CONNECTION_DEFAULT[] = "DefaultDB";
const char CONNECTION_SYSTEM[] = "SystemDB";
const char CONNECTION_QUERY_POOL[] = "QueryPoolDB";
const char CONNECTION_ASTERISK[] = "AsteriskDB";
//-----------------------------------------------------------------------------
const char CONFIG_TEMPLATE_SERVER[] = "Server"; //������ ����������������� ����� ��� �������
const char CONFIG_TEMPLATE_CLIENT[] = "Client"; //������ ����������������� ����� ��� �������
//SQL_DRIVERS------------------------------------------------------------------
const char SQL_DRIVER_QPSQL[] = "QPSQL";
const char SQL_DRIVER_QODBC[] = "QODBC";
const char SQL_DRIVER_QSQLITE[] = "QSQLITE";
//-----------------------------------------------------------------------------
const char			SYSTEM_DATABASE_NAME[] = "postgres"; //��� ��������� ��
const char			SYSTEM_USER_LOGIN[] = "postgres"; //����� ���������� ������������
const char			SYSTEM_USER_UID[] = "{F400ECFA-185E-4587-8961-8731DEAC3B5E}"; //������������� �������� ������������
const int			MAX_QUERY_TIME = 50; //����������� ����� ���������� SQL-������� (� �������������)
const int			CALENDAR_AUDIO_TIMEOUT = 20000;
const char			CLASS_IS_LIST_EDIT[] = "ISListEdit";
const int			MINIMUM_PASSWORD_LENGHT = 8;
const int			WAIT_LOAD_DATA_LIST_FORM = 100;
const unsigned int	YEAR_MINIMUM = 1;
const unsigned int	YEAR_MAXIMUM = (unsigned int)SHRT_MAX;
const int			MAX_FILE_SIZE_FIELD = ((1000 * 1024) * 10); //������������ ������ ����� � ���� �������������� ����� (10 ��������)
const char			COLOR_STANDART[] = "#000000";
const char			NO_ERROR_STRING[] = "No error.";
const size_t		WORD_SIZE = 40; //������������ ������ ����� � �������
const size_t		EDIT_BLINK_COUNT = (3) * 2; //���������� ������� ���� ��������������. ��� 3 - ���������� �������.
const char			UUID_PLACEHOLDER_TEXT[] = "{00000000-0000-0000-0000-000000000000}";
const size_t		NPOS = std::string::npos;
const char			FILE_EDIT_PROPERTY_NAME[] = "Name";
const char			FILE_EDIT_PROPERTY_LOGO[] = "Logo";
const char			FILE_EDIT_PROPERTY_DATA[] = "Data";
const size_t		MAX_DECIMAL_LEN = 17; //������������ ���������� ����� ����� ������� � ���������� ����� (������ DBL_DECIMAL_DIG)
const size_t		MAX_INTEGER_LEN = 19; //������������ "�����" ������ �����
const char			SYMBOL_OBJECT_CHANGED[] = " {*}";
const size_t		AUDIO_PLAYER_POS = 5000; //��������� � �����-������
const char			LOCAL_HOST_ADDRESS[] = "127.0.0.1";
//LOGGER-----------------------------------------------------------------------
const size_t	LOGGER_ARRAY_SIZE = 20000; //������������ ������ �������
const size_t	LOGGER_TIMEOUT = 50; //������� �������
const size_t	LOGGER_MESSAGE_SIZE = 4098; //������������ ������ ������ �������
//CARAT------------------------------------------------------------------------
const unsigned short	CARAT_TCP_PORT = 50000; //����������� ���� TCP-�������
const unsigned short	CARAT_CONTROLLER_PORT = 50001; //����������� ���� �����������
const size_t			CARAT_HASH_SIZE = 64; //������������� ������ ���� SHA256 ��� �����������
const unsigned long     CARAT_SALT_SIZE = 32; //������������� ������ ����
const size_t			CARAT_CONNECT_TIMEOUT = 3000; //����� �������� ����������� � ������
const size_t			CARAT_CONNECT_SLEEP = 10; //����� ����� ��� �������� �����������
const size_t			CARAT_RECONNECT_TIMEOUT = 5000; //����� ����� ��� ������� ������������ � �������
const size_t			CARAT_BAN_ATTEMPT_COUNT = 5; //���������� ������� �����������
//ASTERISK---------------------------------------------------------------------
const char              ASTERISK_AMI_SEPARATOR[] = "\r\n\r\n"; //����������� ������� AMI
const unsigned short    ASTERISK_AMI_PORT = 5038; //���� AMI �� ���������
//CARAT_API--------------------------------------------------------------------
const char API_AUTH[] = "Auth"; //�����������
const char API_SLEEP[] = "Sleep"; //��������
const char API_GET_META_DATA[] = "GetMetaData"; //��������� ����-������
const char API_GET_LAST_CLIENT[] = "GetLastClient"; //�������� ��������� ������ �������
const char API_USER_PASSWORD_EXIST[] = "UserPasswordExist"; //��������� ������������� ������
const char API_USER_PASSWORD_CREATE[] = "UserPasswordCreate"; //�������� ������ ������������
const char API_USER_PASSWORD_EDIT[] = "UserPasswordEdit"; //��������� ������ ������������
const char API_USER_PASSWORD_RESET[] = "UserPasswordReset"; //����� ������ ������������
const char API_USER_SETTINGS_RESET[] = "UserSettingsReset"; //����� ���������������� ��������
const char API_GET_RECORD_CALL[] = "GetRecordCall"; //�������� ������ ���������
const char API_GET_CLIENTS[] = "GetClients"; //�������� ������ ������������ ��������
const char API_RECORD_DELETE[] = "RecordDelete"; //������� ������
const char API_DISCUSSION_ADD[] = "DiscussionAdd"; //�������� ����������
const char API_DISCUSSION_EDIT[] = "DiscussionEdit"; //�������� ����������
const char API_DISCUSSION_COPY[] = "DiscussionCopy"; //����������� ����������
const char API_GET_TABLE_DATA[] = "GetTableData"; //�������� ������ �� �������
const char API_NOTE_RECORD_GET[] = "GetNoteRecord"; //�������� ���������� ������
const char API_NOTE_RECORD_SET[] = "SetNoteRecord"; //�������� ���������� ������
const char API_FILE_STORAGE_ADD[] = "FileStorageAdd"; //�������� ���� � ���������
const char API_FILE_STORAGE_COPY[] = "FileStorageCopy"; //����������� ���� � ���������
const char API_FILE_STORAGE_GET[] = "FileStorageGet"; //�������� ���� �� ���������
const char API_SEARCH_TASK_TEXT[] = "SearchTaskText"; //����� ����� �� ������
const char API_SEARCH_TASK_ID[] = "SearchTaskID"; //����� ����� �� ������
const char API_SEARCH_FULL_TEXT[] = "SearchFullText"; //������������ �����
const char API_GET_CALENDAR_EVENTS[] = "GetCalendarEvents"; //�������� ������� ���������
const char API_CALENDAR_DELETE[] = "CalendarDelete"; //�������� ������� ���������
const char API_GET_INTERNAL_LISTS[] = "GetInternalLists"; //�������� ������ ���������� ������������
const char API_SAVE_META_DATA[] = "SaveMetaData"; //���������� ����-������
const char API_GET_GROUP_RIGHTS[] = "GetGroupRights"; //�������� ����� ������
const char API_GROUP_RIGHT_SUBSYSTEM_ADD[] = "GroupRightSubSystemAdd"; //�������� ������ ����� �� ����������
const char API_GROUP_RIGHT_SUBSYSTEM_DELETE[] = "GroupRightSubSystemDelete"; //������� � ������ ����� �� ����������
const char API_GROUP_RIGHT_TABLE_ADD[] = "GroupRightTableAdd"; //�������� ������ ����� �� �������
const char API_GROUP_RIGHT_TABLE_DELETE[] = "GroupRightTableDelete"; //������� � ������ ����� �� �������
const char API_GROUP_RIGHT_SPECIAL_ADD[] = "GroupRightSpecialAdd"; //�������� ����. ����� ������
const char API_GROUP_RIGHT_SPECIAL_DELETE[] = "GroupRightSpecialDelete"; //������� ����. ����� � ������
const char API_GET_RECORD_VALUE[] = "GetRecordValue"; //�������� �������� ���� ������
const char API_RECORD_FAVORITE_ADD[] = "RecordFavoriteAdd"; //�������� ������ � ���������
const char API_RECORD_FAVORITE_DELETE[] = "RecordFavoriteDelete"; //������� ������ �� ����������
const char API_GET_FAVORITE_NAMES[] = "GetFavoriteNames"; //�������� ����� ��������� �������
const char API_FAVORITES_DELETE[] = "FavoritesDelete"; //������� ��� ��������� ������ ������������
const char API_CALENDAR_CLOSE[] = "CalendarClose"; //�������� ������� ���������
const char API_GET_HISTORY_LIST[] = "GetHistoryList"; //�������� ������ �������� �������
const char API_TASK_COMMENT_ADD[] = "TaskCommentAdd"; //�������� ����������� � ������
//CARAT_LOCAL_API--------------------------------------------------------------
const char	CARAT_LOCAL_API_SHUTDOWN[] = "shutdown"; //���������� ������
//-----------------------------------------------------------------------------
const char CONST_UID_SETTINGS_DATABASE[] = "{1DE3449C-C700-4EFC-A455-E99E38063D9B}"; //������������� �������� ���� ������
//-----------------------------------------------------------------------------
const char CONST_UID_PARAGRAPH_DESKTOP[] = "{9A36AD64-5A2F-4B62-9DA7-4FC545B6AB5F}"; //������� ����
const char CONST_UID_PARAGRAPH_WORKSPACE[] = "{18A90F8B-1D99-420E-BA60-C3EBF60D595E}"; //������� �������
const char CONST_UID_PARAGRAPH_CALENDAR[] = "{1BA7F467-32AA-4719-B8F0-BCD8BC678CAB}"; //���������
const char CONST_UID_PARAGRAPH_TASKS[] = "{588A7DBA-CC77-4EA2-9691-7F800C629D7F}"; //������
//-----------------------------------------------------------------------------
const char CONST_UID_PROTOCOL_ENTER_APPLICATION[] = "{2614CB96-EC54-4D4F-B1A6-06D2444962CA}";
const char CONST_UID_PROTOCOL_EXIT_APPLICATION[] = "{E9461667-E1CA-4C22-907B-FDC350A931C4}";
const char CONST_UID_PROTOCOL_CREATE_OBJECT[] = "{D1348312-298F-4A7C-B584-9BA8C4952CD3}";
const char CONST_UID_PROTOCOL_CREATE_COPY_OBJECT[] = "{EFA8FE45-1174-4D2E-BBE6-4940380961D4}";
const char CONST_UID_PROTOCOL_EDIT_OBJECT[] = "{0361643D-0A62-4F51-84BD-313F53115EFD}";
const char CONST_UID_PROTOCOL_SHOW_OBJECT[] = "{117E8972-97DC-4E72-93AC-DC3BB50D11CF}";
const char CONST_UID_PROTOCOL_DELETE_OBJECT[] = "{B6EEFB63-8197-4F3A-8373-5C3B2FC409F3}";
const char CONST_UID_PROTOCOL_EXPORT_TABLE[] = "{1C50A667-B1C4-4F2C-8F39-90106ABB6774}";
const char CONST_UID_PROTOCOL_SEARCH[] = "{179F73BF-8983-4B52-B505-AF0EF4BF8093}";
const char CONST_UID_PROTOCOL_SEARCH_FULL_TEXT[] = "{B0A1EF68-2AA4-4EB2-A5A9-F8E5604F864B}";
const char CONST_UID_PROTOCOL_NOTE_RECORD_SHOW[] = "{0A6320B0-827B-4043-886A-1D7FD5D9B6DE}";
const char CONST_UID_PROTOCOL_NOTE_RECORD_ADD[] = "{662105B6-6671-46BE-8E1A-E57061C5C268}";
const char CONST_UID_PROTOCOL_NOTE_RECORD_EDIT[] = "{C20634AF-EF58-4D96-9215-8085C2B8C416}";
const char CONST_UID_PROTOCOL_NOTE_RECORD_DELETE[] = "{08904DF2-3785-4454-B5E5-2D83A9FAA231}";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM[] = "{EA765447-6811-43AD-8CFC-9B2E758A5B4E}";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM[] = "{5E5EC969-0D11-41FB-9652-7E2ADD466782}";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE[] = "{44FDDA11-9F7B-4006-8178-944DA65FB7AA}";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE[] = "{9D4E8104-33F9-4F9E-A6AA-272DDA7C1A52}";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL[] = "{FB494E8E-BDF3-4804-AF42-43487AF8A871}";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL[] = "{4E7C34E5-C7FE-4C0D-BB4D-7B4A1D4CF9D2}";
const char CONST_UID_PROTOCOL_USER_PASSWORD_CREATE[] = "{6DD00B13-2A69-45FD-B565-3142C2D2BA97}";
const char CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE[] = "{0EEE6D67-A0F3-4558-B7AD-173678A8C9DC}";
const char CONST_UID_PROTOCOL_USER_PASSWORD_RESET[] = "{3B005C51-B44E-48C6-95F8-3AC6BD913BB3}";
const char CONST_UID_PROTOCOL_USER_SETTINGS_RESET[] = "{075E58A5-5336-4A71-8D1D-91202173F4D4}";
const char CONST_UID_PROTOCOL_FILE_STORAGE_ADD[] = "{7E9FF116-CF0A-4568-B048-7FAB8D773D57}";
const char CONST_UID_PROTOCOL_FILE_STORAGE_COPY[] = "{5CB370B6-D1AA-4E40-83A9-AEAC7FEA65E2}";
const char CONST_UID_PROTOCOL_FILE_STORAGE_SAVE[] = "{E73BEDC4-79FF-4B94-ACC9-2692E1927D18}";
const char CONST_UID_PROTOCOL_TASK_SEARCH_TEXT[] = "{E4698E8F-C1F4-4F04-A4B6-5CD8805B7FAC}";
const char CONST_UID_PROTOCOL_TASK_SEARCH_ID[] = "{4DFD26F5-799F-4E34-8DBD-78E52F01423D}";
const char CONST_UID_PROTOCOL_CALENDAR_DELETE[] = "{D9372E46-8DB0-41B4-80C0-6AEC2CC6CD17}";
const char CONST_UID_PROTOCOL_GET_UPDATE_CLIENT[] = "{0234643A-AF60-44BF-A2EB-08A71307841C}";
const char CONST_UID_PROTOCOL_RECORD_FAVORITE_ADD[] = "{8D08A9F9-FC50-4986-8D92-FEC0D7D36DA0}";
const char CONST_UID_PROTOCOL_RECORD_FAVORITE_DELETE[] = "{AF2641F0-6838-441C-A0A9-B62090826998}";
const char CONST_UID_PROTOCOL_CALENDAR_CLOSE[] = "{F05B1DE1-47F6-43DB-A8C5-5D3582F7BD5F}";
const char CONST_UID_PROTOCOL_GET_TABLE_DATA[] = "{2A93A14E-4BA5-4CB8-A056-0FF2B3351C5D}";
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_TYPE_SHOW[] = "{41743776-D7CE-414F-ACF3-71C42A607742}"; //�������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_CREATE[] = "{1CFA961D-08A2-4AA3-978E-FF23FC88CA08}"; //�������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_EDIT[] = "{259AC6D5-63F8-4C44-B8A6-0108DA542BA6}"; //��������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_DELETE[] = "{48E0575C-31A0-4AE1-87D7-6450B69A41A5}"; //�������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_EXPORT[] = "{E3DB5851-69A2-4D32-B4F4-0EB98C331B22}"; //������� ������
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP[] = "{6948884F-5F69-4F74-A230-A2D892A44A52}"; //������� ����
const char CONST_UID_GROUP_ACCESS_SPECIAL_SETTINGS_DB[] = "{E006A41A-465F-41AB-BA5D-2CBC41935B2C}"; //������� ����
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_CLOSE[] = "{4FA7688B-E0B8-4018-B6EC-FD4BE898C90B}"; //�������� �����
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_REOPEN[] = "{D0A6AA06-FF19-4945-9441-AEA245AE32DF}"; //������������ �����
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_EDIT[] = "{988106BE-292A-428A-A10D-5858C4DFE569}"; //�������������� �����
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_TRANSFORMATION[] = "{D7837403-15BA-4B50-8B9A-660602C24B98}"; //�������������� ����� � �������� � ��������
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_APPOINT[] = "{14B3E83D-56D4-4527-9C33-0770D38B78D5}"; //���������� �����������
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_SHOW_ALL[] = "{093E482F-C108-4939-A1D5-66377BA354AC}"; //�������� ���� �����
//-----------------------------------------------------------------------------
const char CONST_UID_TASK_PRIORITY_LOW[] = "{CD620874-5069-48E4-BDEA-2D5AEBF8ABA7}"; //������
const char CONST_UID_TASK_PRIORITY_AVERAGE[] = "{87AC0B68-9348-4A39-8466-B84DC641FE8B}"; //�������
const char CONST_UID_TASK_PRIORITY_HIGH[] = "{5B4C2BDB-758B-49F3-B172-7351AAA51C58}"; //�������
const char CONST_UID_TASK_PRIORITY_CRITICAL[] = "{CED1B019-C1CD-4044-A9B1-2A97273143F4}"; //�����������
//-----------------------------------------------------------------------------
const char CONST_UID_TASK_STATUS_OPEN[] = "{2FD27958-6EC9-4E78-88F4-0A2BFDFE4716}"; //�������
const char CONST_UID_TASK_STATUS_IN_WORK[] = "{72BEB1A3-0491-4932-AAA0-5725DA41A0A8}"; //� ������
const char CONST_UID_TASK_STATUS_DONE[] = "{20F3D6A5-1A90-4228-9C39-8F05DFDED9FE}"; //���������
const char CONST_UID_TASK_STATUS_NOT_DONE[] = "{D3F65984-117E-4AFE-81E0-1198533767F8}"; //�� ����� ���������
const char CONST_UID_TASK_STATUS_CLOSE[] = "{1617EC4F-50F6-4912-8143-4D702E1F5F8F}"; //�������
//-----------------------------------------------------------------------------
const char CONST_UID_DATABASE_SETTING_GENERAL_ACCESSDATABASE[] = "UserAccessDatabase";
const char CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA[] = "NumberSimbolsAfterComma";
const char CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE[] = "StorageFileMaxSize";
//-----------------------------------------------------------------------------
const char CONST_UID_SETTING_GROUP_GENERAL[] = "{152517C5-B081-42E5-98E4-0BA102EF7FF8}";
const char CONST_UID_SETTING_GROUP_VIEW[] = "{3AECBF8A-531F-4484-B507-1269A10B5F2F}";
const char CONST_UID_SETTING_GROUP_TABLES[] = "{12B776E9-CE2E-44A8-855D-67EDB4E58CD2}";
const char CONST_UID_SETTING_TABLES_MINIMIZEHEIGHTROWS[] = "{77BA2566-D78D-467A-92CE-DFD987F21C03}";
const char CONST_UID_SETTING_TABLES_DOUBLECLICKEVENT[] = "{E0B5EED0-947E-408E-9666-944CC31C5D1A}";
const char CONST_UID_SETTING_TABLES_SHOWHORIZONTALHEADER[] = "{22C5FF3B-83DE-4C8D-BC2B-8A456C8C57D4}";
const char CONST_UID_SETTING_TABLES_SHOWVERTICALHEADER[] = "{6D14F262-B98E-4FBD-B9CC-A1A8E7B52D9F}";
const char CONST_UID_SETTING_TABLES_SHOWCOUNTRECORD[] = "{EFD021B1-5267-4DFA-8D11-A4FF4A419536}";
const char CONST_UID_SETTING_TABLES_SHOWNAVIGATION[] = "{1A5C6B4E-CD33-4478-A516-BF35E2DA01EE}";
const char CONST_UID_SETTING_TABLES_PAGE_NAVIGATION[] = "{9F2A193A-9EFA-42A9-954F-5BC49B17C102}";
const char CONST_UID_SETTING_TABLES_PAGE_NAVIGATION_LIMIT[] = "{6F5CA447-F94A-4CCF-8241-C82F6E8BD82B}";
const char CONST_UID_SETTING_TABLES_SEARCH_FAST_VIEW[] = "{A07AA634-F553-4C05-8EEC-CC8A5AE7DC18}";
const char CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID[] = "{2876F177-B30F-41A0-8643-F2BCA583726F}";
const char CONST_UID_SETTING_GROUP_TABS[] = "{C42701D0-3404-42C6-82D9-41EC50F2E556}";
const char CONST_UID_SETTING_TABS_SELECTIONBEHAVIOR[] = "{8C96BF79-A7FB-4562-ABAB-A9C520B4B68D}";
const char CONST_UID_SETTING_GROUP_SOUND[] = "{8460DB7A-90FB-459E-8343-7A67529414C0}";
const char CONST_UID_SETTING_SOUND_VOLUME[] = "{203F8364-B041-4C20-A7EF-E1FBA8CDB68B}";
const char CONST_UID_SETTING_SOUND_CALENDAREVENT[] = "{70680EED-160D-45E9-8F79-D6CBDE8AA875}";
const char CONST_UID_SETTING_GROUP_CALENDAR[] = "{65F8B333-5C21-48A2-8F30-D947C36EF51B}";
const char CONST_UID_SETTING_CALENDAR_NOTE_VIEW_TYPE[] = "{B03FBAC5-F793-4565-92A3-821C87AE3B56}";
const char CONST_UID_SETTING_GROUP_TASK_TRACKER[] = "{65F8B333-5C21-48A2-8F30-D947C36EF51B}";
const char CONST_UID_SETTING_TASK_TRACKER_SHOW_TASK_AFTER_CREATE[] = "{094CB33E-13B9-4E5C-87DC-79AE69CBE533}";
const char CONST_UID_SETTING_GROUP_OTHER[] = "{0BAEF651-D749-4566-B682-F11D03C6D849}";
//-----------------------------------------------------------------------------
const char CONST_CONFIG_CONNECTION_SERVER[] = "Connection/Host";
const char CONST_CONFIG_CONNECTION_PORT[] = "Connection/Port";
const char CONST_CONFIG_CONNECTION_DATABASE[] = "Connection/Database";
const char CONST_CONFIG_CONNECTION_LOGIN[] = "Connection/Login";
const char CONST_CONFIG_CONNECTION_PASSWORD[] = "Connection/Password";
const char CONST_CONFIG_REMEMBER_USER_INCLUDE[] = "RememberUser/Include";
const char CONST_CONFIG_REMEMBER_USER_LOGIN[] = "RememberUser/Login";
const char CONST_CONFIG_TCPSERVER_INCLUDE[] = "TCPServer/Include";
const char CONST_CONFIG_TCPSERVER_PORT[] = "TCPServer/Port";
const char CONST_CONFIG_TCPSERVER_WORKERS[] = "TCPServer/WorkerCount";
const char CONST_CONFIG_CONTROLLER_INCLUDE[] = "Controller/Include";
const char CONST_CONFIG_CONTROLLER_PORT[] = "Controller/Port";
const char CONST_CONFIG_AMI_INCLUDE[] = "AMI/Include";
const char CONST_CONFIG_AMI_SERVER[] = "AMI/Host";
const char CONST_CONFIG_AMI_PORT[] = "AMI/Port";
const char CONST_CONFIG_AMI_LOGIN[] = "AMI/Login";
const char CONST_CONFIG_AMI_PASSWORD[] = "AMI/Password";
const char CONST_CONFIG_AMI_RECORD_DIR[] = "AMI/RecordDir";
const char CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR[] = "Other/UpdateClientDir";
const char CONST_CONFIG_OTHER_CONFIGURATION[] = "Other/Configuration";
//-----------------------------------------------------------------------------
#endif
