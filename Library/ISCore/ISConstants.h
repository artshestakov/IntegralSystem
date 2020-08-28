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
#ifdef WIN32
const char EXTENSION_BINARY[] = ".exe"; //Расширение исполняемого файла для текущей операционной системы
const char EXTENSION_LIBRARY[] = ".dll"; //Расширение динамической библиотеки для текущей операционной системы
#else
const char EXTENSION_BINARY[] = ""; //Расширение исполняемого файла для текущей операционной системы
const char EXTENSION_LIBRARY[] = ".so"; //Расширение динамической библиотеки для текущей операционной системы
#endif
//LOCALIZATION-----------------------------------------------------------------
const char LOCALIZATION_FILE_INTEGRAL_SYSTEM[] = "IntegralSystem";
const char LOCALIZATION_FILE_OBJECTS[] = "Objects";
const char LOCALIZATION_FILE_CONFIGURATOR[] = "Configurator";
//REG_EXP----------------------------------------------------------------------
const char REG_EXP_IP_ADDRESS[] = "([0-9]{1,3}\\.){3}[0-9]{1,3}(\\/([0-9]|[1-2][0-9]|3[0-2]))?"; //IP-адресс
const char REG_EXP_UID[] = "UID=\"\\{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}\\}\""; //Уникальный идентфикатор из мета-данных
const char REG_EXP_LOGIN[] = "^([a-z]+)$";
const char REG_EXP_PASSWORD_ALT_SYMBOL[] = "^[^а-яА-ЯёЁa-zA-Z0-9_]+$";
const char REG_EXP_PASSWORD_UPPER[] = "^[А-ЯA-Z]+$";
const char REG_EXP_URL[] = "^(http:\\/\\/www\\.|https:\\/\\/www\\.|http:\\/\\/|https:\\/\\/)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/.*)?$"; //Ссылка на сайт или страницу в интернете
const char REG_EXP_FIAS_CONTENT[] = "<\\w+ .+\\/>";
const char REG_EXP_FIAS_TABLE_NAME[] = "[A-Za-z]+ ";
//DATE_TIME_FORMATS------------------------------------------------------------
const char FORMAT_DATE_V1[] =		"d MMMM yyyy";					//30 Июль 2020
const char FORMAT_DATE_V2[] =		"dd.MM.yyyy";					//30.07.2020
const char FORMAT_DATE_V3[] =		"dd/MM/yyyy";					//30/07/2020
const char FORMAT_DATE_V4[] =		"d MMMM yyyy (ddd)";			//30 Июль 2020 (Чт)
const char FORMAT_TIME_V1[] =		"hh:mm";						//10:16
const char FORMAT_TIME_V2[] =		"hh.mm";						//10.16
const char FORMAT_TIME_V3[] =		"hh:mm:ss";						//10:16:13
const char FORMAT_TIME_V4[] =		"mm:ss";						//16:13
const char FORMAT_TIME_V5[] =		"hh.mm.ss";						//10.16.13
const char FORMAT_DATE_TIME_V1[] =	"d MMMM yyyy hh:mm";			//4 Август 2020 12:59
const char FORMAT_DATE_TIME_V2[] =	"dd.MM.yyyy hh:mm";				//04.08.2020 12:59
const char FORMAT_DATE_TIME_V3[] =	"d MMMM yyyy hh:mm:ss";			//4 Август 2020 12:59:52
const char FORMAT_DATE_TIME_V4[] =	"dd.MM.yyyy hh:mm:ss.zzz";		//04.08.2020 12:59:52.459
const char FORMAT_DATE_TIME_V5[] =	"d MMMM yy hh:mm:ss.zzz";		//4 Август 20 12:59:52.459
const char FORMAT_DATE_TIME_V6[] =	"dd.MM.yyyy hh.mm";				//04.08.2020 12.59
const char FORMAT_DATE_TIME_V7[] =	"yyyy-MM-dd hh:mm:ss";			//2020-08-04 12:59:52
const char FORMAT_DATE_TIME_V8[] =	"dd.MM.yyyy_hh.mm.ss";			//04.08.2020_12.59.52
const char FORMAT_DATE_TIME_V9[] =	"dd.MM.yy hh:mm:ss.zzz";		//04.08.20 12:59:52.459
const char FORMAT_DATE_TIME_V10[] =	"d MMMM yyyy (ddd) hh:mm:ss";	//4 Август (Вт) 2020 12:59:52
//TOKENS-----------------------------------------------------------------------
const char TOKEN_DA_DATA_TOKEN[] = "f2c204cd6ba093b294324acec5d01b25fad632cf";
//PROPERTY---------------------------------------------------------------------
const char PROPERTY_LINE_EDIT_SELECTED_MENU[] = "LineEditSelectedMenu";
//-----------------------------------------------------------------------------
const char PATH_SCHEMA_TEMPLATE_FIELDS[] = ":Other/ClassTemplateFields.xml";
const char PATH_CONFIGURATOR_SCHEME[] = ":Other/Configurator.xml";
const char PATH_CONFIGURATIONS_SCHEME[] = ":Other/Configurations.xml";
const char PATH_SPLASH_WIDGET_LOGO[] = ":Images/SplashWidgetLogo.png";
const char PATH_BUILD_INFO[] = ":Build/Build.ini.tmp";
//REPORT_TYPE------------------------------------------------------------------
const char REPORT_TYPE_HTML[] = "Html";
//CURSORS----------------------------------------------------------------------
const Qt::CursorShape CURSOR_ARROW = Qt::ArrowCursor;
const Qt::CursorShape CURSOR_POINTING_HAND = Qt::PointingHandCursor;
const Qt::CursorShape CURSOR_WHATS_THIS = Qt::WhatsThisCursor;
const Qt::CursorShape CURSOR_WAIT = Qt::WaitCursor;
const Qt::CursorShape CURSOR_BUSY = Qt::BusyCursor;
const Qt::CursorShape CURSOR_FORBIDDEN = Qt::ForbiddenCursor;
const Qt::CursorShape CURSOR_I_BEAM = Qt::IBeamCursor;
//INTERFACE--------------------------------------------------------------------
const int SIZE_MINIMUM_HEIGHT_EDIT_FIELD = 24; //Минимальная высота поля ввода ввода информации
const int EDIT_FIELD_SPACING = 4;
const int ISPUSHBUTTON_MINIMUM_WIDTH = 75;
const int ISPUSHBUTTON_MINIMUM_HEIGHT = 24;
//SYMBOLS----------------------------------------------------------------------
const char SYMBOL_SPACE = ' '; //Символ пробела
const char SYMBOL_MINUS = '-'; //Символ "Минус"
const char SYMBOL_COMMA = ','; //Символ "Запятая"
const char SYMBOL_POINT = '.'; //Символ "Точка"
const char SYMBOL_STAR = '*'; //Символ звездочка
//ASTERISK_MANAGER_API---------------------------------------------------------
const char AMI_SUCCESSFUL_AUTH[] = "SuccessfulAuth";
const char AMI_USER_EVENT[] = "UserEvent";
const char AMI_DIAL_BEGIN[] = "DialBegin";
const char AMI_DIAL_END[] = "DialEnd";
const char AMI_HANGUP[] = "Hangup";
const char AMI_BLIND_TRANSFER[] = "BlindTransfer";
const char AMI_ORIGINATE_RESPONSE[] = "OriginateResponse";
const char AMI_EXTENSION_STATUS[] = "ExtensionStatus";
//DATABASE_CONNECTIONS---------------------------------------------------------
const char CONNECTION_DEFAULT[] = "DefaultDB";
const char CONNECTION_USER[] = "UserDB";
const char CONNECTION_SYSTEM[] = "SystemDB";
const char CONNECTION_PATTERN_ACITIVITY[] = "PatternActivityDB";
const char CONNECTION_FULL_TEXT_SEARCH[] = "FullTextSearchDB";
const char CONNECTION_QUERY_POOL[] = "QueryPoolDB";
//-----------------------------------------------------------------------------
const char		SYSTEM_DATABASE_NAME[] = "postgres"; //Имя системной БД
const char		FIGURES_STRING[] = "0123456789";
const char		SQL_DRIVER_QPSQL[] = "QPSQL";
const int		MAX_QUERY_TIME = 50; //Передельное время выполнения SQL-запроса (в миллисекундах)
const int		CALENDAR_AUDIO_TIMEOUT = 20000;
const char		CLASS_IS_LIST_EDIT[] = "ISListEdit";
const int		MINIMUM_PASSWORD_LENGHT = 8;
const int		WAIT_LOAD_DATA_LIST_FORM = 400;
const int		ASTERISK_RECONNECT_TIMEOUT = 10000;
const int		YEAR_MINIMUM = 1752;
const int		YEAR_MAXIMUM = 8000;
const int		MAX_FILE_SIZE_FIELD = ((1000 * 1024) * 10); //Максимальный размер файла в поле редактирования файла (10 мегабайт)
const int		PLAYER_WINDING = 10000;
const char		COLOR_STANDART[] = "#000000";
const char		COLOR_PATTERN_WIDGET_NOT_REGISTERED[] = "#bbbbbb";
const char		NO_ERROR_STRING[] = "No error.";
const size_t	WORD_SIZE = 40; //Максимальный размер слова в словаре
const int		QUERY_POOL_TIMEOUT = 1000; //Таймаут выполнения запросов из очереди
const size_t	EDIT_BLINK_COUNT = (3) * 2; //Количество миганий поля редактирования. Где 3 - количество миганий.
const char		UUID_PLACEHOLDER_TEXT[] = "{00000000-0000-0000-0000-000000000000}";
const size_t	NPOS = std::string::npos;
const char		FILE_EDIT_PROPERTY_NAME[] = "Name";
const char		FILE_EDIT_PROPERTY_LOGO[] = "Logo";
const char		FILE_EDIT_PROPERTY_DATA[] = "Data";
const size_t    MAX_DECIMAL_LEN = 17; //Максимальное количество чесел после запятой в десятичной дроби (аналог DBL_DECIMAL_DIG)
const size_t	MAX_INTEGER_LEN = 19; //Максимальная "длина" целого числа
//LOGGER-----------------------------------------------------------------------
const size_t	LOGGER_ARRAY_SIZE = 20000; //Максимальный размер массива
const size_t	LOGGER_TIMEOUT = 50; //Таймаут логгера
//CARAT------------------------------------------------------------------------
const char		CARAT_UID[] = "{88DD90C4-2DFF-4515-8C32-7371C39C598A}"; //Уникальный идентификатор для контроля единого запущенного экземпляра
const char		CARAT_CORE_START_FLAG[] = "exec"; //Посылаемое сообщение от ядра означающее успешный запуск ядра
const int		CARAT_CORE_START_TIMEOUT = 5000; //Таймаут ожидания первого собщения от ядра
const char		CARAT_DEBUGGER_PORT[] = "{381A6F7D-1E01-438E-B7B4-3B0F9881EB8F}"; //Условный порт для отладчика
const int		CARAT_DEBUGGER_MAX_CLIENTS = 1; //Максимальное количество клиентов для режима отладчика
const int		CARAT_DEBUGGER_CONNECT_TIMEOUT = 2000; //Таймаут попытки подключения к карату в режиме отладчика
const int		CARAT_TIMEOUT_CONNECT = 2000; //Таймаут ожидания принятия подключения
const int		CARAT_TIMEOUT_STARTED_WORKER = 3000; //Таймаут ожидания запуска воркера
const quint16	CARAT_DEFAULT_PORT = 50000; //Стандартный порт
const char		CARAT_CONTROLLER_PORT[] = "{9C3430ED-17E2-49C6-AD58-83140981C3C5}"; //Порт для контроллера воркеров
//CARAT_API--------------------------------------------------------------------
const char		API_AUTH[] = "Auth"; //Авторизация
const char		API_TEST_QUERY[] = "TestQuery"; //Тестовый запрос
const char		API_SLEEP[] = "Sleep"; //Ожидание
const char		API_GET_META_DATA[] = "GetMetaData"; //Получение мета-данных
const char		API_FUNCTION_LIST[] = "FunctionList"; //Получение списка API-функций
//-----------------------------------------------------------------------------
const char CONST_UID_USER_POSTGRES[] = "{F400ECFA-185E-4587-8961-8731DEAC3B5E}"; //Идентификатор пользователя postgres
const char CONST_UID_SETTINGS_DATABASE[] = "{1DE3449C-C700-4EFC-A455-E99E38063D9B}"; //Идентификатор настроек базы данных
const char CONST_UID_SEX_MALE[] = "{601C7C32-7413-45DA-AAAC-926A77C824F8}"; //Мужской
//-----------------------------------------------------------------------------
const char CONST_UID_PARAGRAPH_DESKTOP[] = "{9A36AD64-5A2F-4B62-9DA7-4FC545B6AB5F}"; //Рабочий стол
const char CONST_UID_PARAGRAPH_WORKSPACE[] = "{18A90F8B-1D99-420E-BA60-C3EBF60D595E}"; //Рабочая область
const char CONST_UID_PARAGRAPH_CALENDAR[] = "{1BA7F467-32AA-4719-B8F0-BCD8BC678CAB}"; //Календарь
const char CONST_UID_PARAGRAPH_TASKS[] = "{588A7DBA-CC77-4EA2-9691-7F800C629D7F}"; //Задачи
const char CONST_UID_PARAGRAPH_TELEPHONY[] = "{47EFC431-D5AB-481D-BB8C-6BB5E80DCD9B}"; //IP-Телефония
//-----------------------------------------------------------------------------
const char CONST_UID_PROTOCOL_ENTER_APPLICATION[] = "{2614CB96-EC54-4D4F-B1A6-06D2444962CA}";
const char CONST_UID_PROTOCOL_EXIT_APPLICATION[] = "{E9461667-E1CA-4C22-907B-FDC350A931C4}";
const char CONST_UID_PROTOCOL_CREATE_OBJECT[] = "{D1348312-298F-4A7C-B584-9BA8C4952CD3}";
const char CONST_UID_PROTOCOL_CREATE_COPY_OBJECT[] = "{EFA8FE45-1174-4D2E-BBE6-4940380961D4}";
const char CONST_UID_PROTOCOL_EDIT_OBJECT[] = "{0361643D-0A62-4F51-84BD-313F53115EFD}";
const char CONST_UID_PROTOCOL_SHOW_OBJECT[] = "{117E8972-97DC-4E72-93AC-DC3BB50D11CF}";
const char CONST_UID_PROTOCOL_SHOW_SYSTEM_INFO_OBJECT[] = "{2937C918-2E34-431A-86EF-52AEE9BF83AA}";
const char CONST_UID_PROTOCOL_OPEN_PARAGRAPH[] = "{C9C18C76-AC52-4B3E-B89D-5040F06AE3CE}";
const char CONST_UID_PROTOCOL_OPEN_SYSTEM[] = "{66E93262-02C6-49E3-B47E-FF20ED8DDDC5}";
const char CONST_UID_PROTOCOL_OPEN_SUB_SYSTEM[] = "{5F3EBAED-0970-4E0E-883C-535E6F084133}";
const char CONST_UID_PROTOCOL_DELETE_OBJECT[] = "{275EDA32-F757-4EAA-BBD4-6C48AFD5DA9B}";
const char CONST_UID_PROTOCOL_DELETE_CASCADE_OBJECT[] = "{B6EEFB63-8197-4F3A-8373-5C3B2FC409F3}";
const char CONST_UID_PROTOCOL_RECOVERY_OBJECT[] = "{C0000A7D-EDA1-4C06-9D7C-1DC942EE438E}";
const char CONST_UID_PROTOCOL_EXPORT_TABLE[] = "{1C50A667-B1C4-4F2C-8F39-90106ABB6774}";
const char CONST_UID_PROTOCOL_PRINT[] = "{916A2119-5D8D-41C9-8358-AE587F365898}";
const char CONST_UID_PROTOCOL_SEARCH[] = "{179F73BF-8983-4B52-B505-AF0EF4BF8093}";
const char CONST_UID_PROTOCOL_SEARCH_FULL_TEXT[] = "{B0A1EF68-2AA4-4EB2-A5A9-F8E5604F864B}";
const char CONST_UID_PROTOCOL_ADD_NOTE_RECORD[] = "{662105B6-6671-46BE-8E1A-E57061C5C268}";
const char CONST_UID_PROTOCOL_EDIT_NOTE_RECORD[] = "{C20634AF-EF58-4D96-9215-8085C2B8C416}";
const char CONST_UID_PROTOCOL_SHOW_ABOUT_FORM[] = "{A0115A19-05CC-4AFD-8568-F9AE970E7157}";
const char CONST_UID_PROTOCOL_CHANGE_PASSWORD[] = "{CDCB7490-C975-4ECA-AC3A-4532A32ECC4D}";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM[] = "{EA765447-6811-43AD-8CFC-9B2E758A5B4E}";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM[] = "{5E5EC969-0D11-41FB-9652-7E2ADD466782}";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE[] = "{44FDDA11-9F7B-4006-8178-944DA65FB7AA}";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE[] = "{9D4E8104-33F9-4F9E-A6AA-272DDA7C1A52}";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL[] = "{FB494E8E-BDF3-4804-AF42-43487AF8A871}";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL[] = "{4E7C34E5-C7FE-4C0D-BB4D-7B4A1D4CF9D2}";
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_TYPE_SHOW[] = "{41743776-D7CE-414F-ACF3-71C42A607742}"; //Просмотр данных
const char CONST_UID_GROUP_ACCESS_TYPE_CREATE[] = "{1CFA961D-08A2-4AA3-978E-FF23FC88CA08}"; //Создание данных
const char CONST_UID_GROUP_ACCESS_TYPE_EDIT[] = "{259AC6D5-63F8-4C44-B8A6-0108DA542BA6}"; //Изменение данных
const char CONST_UID_GROUP_ACCESS_TYPE_IS_DELETED[] = "{312BA9A1-E3FF-4C5D-B7A2-69A6E41B945A}"; //Просмотр удаленных данных
const char CONST_UID_GROUP_ACCESS_TYPE_EXPORT[] = "{E3DB5851-69A2-4D32-B4F4-0EB98C331B22}"; //Экспорт данных
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP[] = "{6948884F-5F69-4F74-A230-A2D892A44A52}"; //Рабочий стол
const char CONST_UID_GROUP_ACCESS_SPECIAL_CHANGE_PASSWORD[] = "{AE04B08B-8BF8-43C2-973A-8343EBE2F7D6}"; //Смена пароля учётной записи
const char CONST_UID_GROUP_ACCESS_SPECIAL_REPORT_FORM_EDIT[] = "{2C6AA9DB-0804-46A6-8095-D2A0D6011C03}"; //Редактирование печатных форм
const char CONST_UID_GROUP_ACCESS_SPECIAL_RECORD_NOTE[] = "{1196470D-AE52-4361-AC0D-9C15612663D0}"; //Добавление примечаний к записям в таблицах
//-----------------------------------------------------------------------------
const char CONST_UID_USER_PASSWORD_CHANGE_TYPE_CREATE[] = "{6DD00B13-2A69-45FD-B565-3142C2D2BA97}";
const char CONST_UID_USER_PASSWORD_CHANGE_TYPE_UPDATE[] = "{0EEE6D67-A0F3-4558-B7AD-173678A8C9DC}";
const char CONST_UID_USER_PASSWORD_CHANGE_TYPE_DELETE[] = "{62892860-8644-419A-BE6A-6783EA685424}";
//-----------------------------------------------------------------------------
const char CONST_UID_TELEPHONY_ASTERISK[] = "{AAF1708E-B224-441C-B4E9-7569267F3B55}";
const char CONST_UID_TELEPHONY_BEELINE[] = "{CA82CCF7-D66B-4415-BE7B-DC02006DFABF}";
//-----------------------------------------------------------------------------
const char CONST_UID_TASK_PRIORITY_LOW[] = "{CD620874-5069-48E4-BDEA-2D5AEBF8ABA7}"; //Низкий
const char CONST_UID_TASK_PRIORITY_AVERAGE[] = "{87AC0B68-9348-4A39-8466-B84DC641FE8B}"; //Средний
const char CONST_UID_TASK_PRIORITY_HIGH[] = "{5B4C2BDB-758B-49F3-B172-7351AAA51C58}"; //Высокий
//-----------------------------------------------------------------------------
const char CONST_UID_TASK_STATUS_NEW[] = "{2FD27958-6EC9-4E78-88F4-0A2BFDFE4716}"; //Новая
const char CONST_UID_TASK_STATUS_IN_WORK[] = "{72BEB1A3-0491-4932-AAA0-5725DA41A0A8}"; //В работе
const char CONST_UID_TASK_STATUS_DONE[] = "{20F3D6A5-1A90-4228-9C39-8F05DFDED9FE}"; //Выполнена
const char CONST_UID_TASK_STATUS_CLOSE[] = "{1617EC4F-50F6-4912-8143-4D702E1F5F8F}"; //Закрыта
//-----------------------------------------------------------------------------
const char CONST_UID_ASTERISK_DIRECTION_INCOMING[] = "{EB1D0FC1-6F56-406F-9819-9C71990998EC}"; //Входящий
const char CONST_UID_ASTERISK_DIRECTION_OUTGOING[] = "{3E1A44EB-74FD-48A2-A472-FB5B0225DAE9}"; //Исходящий
const char CONST_UID_ASTERISK_DIRECTION_INTERNAL[] = "{660C2835-7624-4A95-A742-525152A0E253}"; //Внутренний вызов
const char CONST_UID_ASTERISK_DIRECTION_TRANSFER[] = "{9768EF57-C631-4968-B4F4-C3D2974B96DC}"; //Перевод вызова
//-----------------------------------------------------------------------------
const char CONST_UID_ASTERISK_CALL_STATUS_NOANSWER[] = "{A9E94860-E621-43E6-9FE2-509324962965}";
const char CONST_UID_ASTERISK_CALL_STATUS_CONGESTION[] = "{792B9531-F48B-45F6-A152-DFF24BB34127}";
const char CONST_UID_ASTERISK_CALL_STATUS_FAILED[] = "{40CD938C-5F20-45D3-8356-7B682E7B5843}";
const char CONST_UID_ASTERISK_CALL_STATUS_BUSY[] = "{2E154D05-B19A-4D5B-8846-343C079F4F51}";
const char CONST_UID_ASTERISK_CALL_STATUS_ANSWER[] = "{6DA83A49-93D3-4D5F-AFC9-C5C89C82E894}";
//-----------------------------------------------------------------------------
const char CONST_UID_ASTERISK_QUEUE_TYPE_OUT_CALLED[] = "{D1980C82-101A-4618-BB99-AD30748145FC}";
//-----------------------------------------------------------------------------
const char CONST_UID_DATABASE_SETTING_GENERAL_ACCESSDATABASE[] = "UserAccessDatabase";
const char CONST_UID_DATABASE_SETTING_GENERAL_ADMINISTRATOR[] = "Administrator";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_FULLNAME[] = "OrganizationNameFull";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_SMALLNAME[] = "OrganizationNameSmall";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_GENERALPRINCIPAL[] = "GeneralPrincipal";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_CHIEFACCOUNTANT[] = "ChiefAccountant";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_STOREKEEPER[] = "Storekeeper";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_LEGALADDRESS[] = "LegalAddress";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_ACTUALADDRESS[] = "ActualAddress";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_INN[] = "INN";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_KPP[] = "KPP";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_OGRN[] = "OGRN";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_DATEREGISTRATION[] = "DateRegistration";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_OKVED[] = "OKVED";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_OKPO[] = "OKPO";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_CHECKINGACCOUNT[] = "CheckingAccount";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_FULLNAMEBANK[] = "FullNameBank";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_BIK[] = "BIK";
const char CONST_UID_DATABASE_SETTING_ORGANIZATION_ACCOUNTCURRENCY[] = "AccountCurrency";
const char CONST_UID_DATABASE_SETTING_CONTACTS_PHONEMAIN[] = "PhoneMain";
const char CONST_UID_DATABASE_SETTING_CONTACTS_PHONEOTHER[] = "PhoneOther";
const char CONST_UID_DATABASE_SETTING_CONTACTS_PHONEFAX[] = "PhoneFax";
const char CONST_UID_DATABASE_SETTING_CONTACTS_WEBSITE[] = "WebSite";
const char CONST_UID_DATABASE_SETTING_CONTACTS_EMAIL[] = "EMail";
const char CONST_UID_DATABASE_SETTING_CONTACTS_ICQ[] = "ICQ";
const char CONST_UID_DATABASE_SETTING_CONTACTS_CONTACTPERSON[] = "ContactPerson";
const char CONST_UID_DATABASE_SETTING_CONTACTS_CONTACTPERSONPHONE[] = "PhoneContactPerson";
const char CONST_UID_DATABASE_SETTING_ASTERISK_SERVER[] = "AsteriskServer";
const char CONST_UID_DATABASE_SETTING_ASTERISK_PORT[] = "AsteriskPort";
const char CONST_UID_DATABASE_SETTING_ASTERISK_LOGIN[] = "AsteriskLogin";
const char CONST_UID_DATABASE_SETTING_ASTERISK_PASSWORD[] = "AsteriskPassword";
const char CONST_UID_DATABASE_SETTING_ASTERISK_DIGITNUMBERS[] = "AsteriskDigitNumbers";
const char CONST_UID_DATABASE_SETTING_ASTERISK_REPOSITORY_LOGIN[] = "AsteriskRepositoryLogin";
const char CONST_UID_DATABASE_SETTING_ASTERISK_REPOSITORY_PASSWORD[] = "AsteriskRepositoryPassword";
const char CONST_UID_DATABASE_SETTING_PASSWORD_SYSTEM_USER[] = "PasswordSystemUser";
const char CONST_UID_DATABASE_SETTING_LOGO_FULL[] = "Logo";
const char CONST_UID_DATABASE_SETTING_LOGO_SMALL[] = "LogoSmall";
const char CONST_UID_DATABASE_SETTING_OTHER_INFORMATIONMESSAGE[] = "InformationMessage";
const char CONST_UID_DATABASE_SETTING_OTHER_INFORMATIONMESSAGECOLOR[] = "InformationMessageColor";
const char CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA[] = "NumberSimbolsAfterComma";
const char CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE[] = "StorageFileMaxSize";
//-----------------------------------------------------------------------------
const char CONST_UID_SETTING_GROUP_GENERAL[] = "{152517C5-B081-42E5-98E4-0BA102EF7FF8}";
const char CONST_UID_SETTING_GENERAL_SHOWNOTIFICATIONFORM[] = "{98F5F993-7647-422A-8C81-340C065F58FE}";
const char CONST_UID_SETTING_GROUP_VIEW[] = "{3AECBF8A-531F-4484-B507-1269A10B5F2F}";
const char CONST_UID_SETTING_VIEW_FULLSCREEN[] = "{6AB8CD71-06BA-4059-A795-F9BCA5255634}";
const char CONST_UID_SETTING_VIEW_STARTEDPARAGRAPH[] = "{33171C9C-A536-426D-B74F-91F174DF00C6}";
const char CONST_UID_SETTING_VIEW_PARAGRAPHVIEW[] = "{916102BF-3121-4696-8A51-D0D053581B56}";
const char CONST_UID_SETTING_VIEW_INCOMINGFORMPOSITION[] = "{BDB8F660-4014-4A16-913C-94E8C02B7630}";
const char CONST_UID_SETTING_VIEW_WEATHERTIMEOUT[] = "{2EFCB5E2-4CDF-44F8-B5E7-9D45135DD8D3}";
const char CONST_UID_SETTING_VIEW_CURRENCYTIMEOUT[] = "{15DEAFA7-8579-4E32-B560-84634F56CAA1}";
const char CONST_UID_SETTING_GROUP_TABLES[] = "{12B776E9-CE2E-44A8-855D-67EDB4E58CD2}";
const char CONST_UID_SETTING_TABLES_HIGHLIGHTINGODDROWS[] = "{9CBB5512-9559-4740-A7E5-A44D94BEC434}";
const char CONST_UID_SETTING_TABLES_MINIMIZEHEIGHTROWS[] = "{77BA2566-D78D-467A-92CE-DFD987F21C03}";
const char CONST_UID_SETTING_TABLES_SELECTIONBEHAVIOR[] = "{E2C337EE-688E-47EC-B0A9-3EE10BC4DFC2}";
const char CONST_UID_SETTING_TABLES_DOUBLECLICKEVENT[] = "{E0B5EED0-947E-408E-9666-944CC31C5D1A}";
const char CONST_UID_SETTING_TABLES_SHOWHORIZONTALHEADER[] = "{22C5FF3B-83DE-4C8D-BC2B-8A456C8C57D4}";
const char CONST_UID_SETTING_TABLES_SHOWVERTICALHEADER[] = "{6D14F262-B98E-4FBD-B9CC-A1A8E7B52D9F}";
const char CONST_UID_SETTING_TABLES_SHOWCOUNTRECORD[] = "{EFD021B1-5267-4DFA-8D11-A4FF4A419536}";
const char CONST_UID_SETTING_TABLES_REMEMBERSORTING[] = "{3F05B53E-3B86-480F-A5F9-3394F15DB790}";
const char CONST_UID_SETTING_TABLES_REMEMBERCOLUMNSIZE[] = "{D9999522-2BD0-464A-9714-F7399B20490F}";
const char CONST_UID_SETTING_TABLES_SHOWTOOLTIP[] = "{1635F5D1-5575-4D3E-A93E-757D28511E50}";
const char CONST_UID_SETTING_TABLES_SHOWNAVIGATION[] = "{1A5C6B4E-CD33-4478-A516-BF35E2DA01EE}";
const char CONST_UID_SETTING_TABLES_PAGE_NAVIGATION[] = "{9F2A193A-9EFA-42A9-954F-5BC49B17C102}";
const char CONST_UID_SETTING_TABLES_PAGE_NAVIGATION_LIMIT[] = "{6F5CA447-F94A-4CCF-8241-C82F6E8BD82B}";
const char CONST_UID_SETTING_TABLES_SEARCH_FAST_REMEMBER[] = "{D5CC8283-1C21-4FFC-8D4F-B9671C854271}";
const char CONST_UID_SETTING_TABLES_SEARCH_FAST_VIEW[] = "{A07AA634-F553-4C05-8EEC-CC8A5AE7DC18}";
const char CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID[] = "{2876F177-B30F-41A0-8643-F2BCA583726F}";
const char CONST_UID_SETTING_TABLE_SCROLL_SELECTION[] = "{A7EE38D5-AC04-4FAF-8C0F-ECED895039DB}";
const char CONST_UID_SETTING_GROUP_TABS[] = "{C42701D0-3404-42C6-82D9-41EC50F2E556}";
const char CONST_UID_SETTING_TABS_WHEELSCROLLINGTABS[] = "{3398D347-6B46-465E-BB58-2B93B92C85EF}";
const char CONST_UID_SETTING_TABS_VIEWUSESSCROLLBUTTONS[] = "{307693A2-BD35-41AE-8F83-016CD340AD43}";
const char CONST_UID_SETTING_TABS_SELECTIONBEHAVIOR[] = "{8C96BF79-A7FB-4562-ABAB-A9C520B4B68D}";
const char CONST_UID_SETTING_GROUP_SOUND[] = "{8460DB7A-90FB-459E-8343-7A67529414C0}";
const char CONST_UID_SETTING_SOUND_VOLUME[] = "{203F8364-B041-4C20-A7EF-E1FBA8CDB68B}";
const char CONST_UID_SETTING_SOUND_CALENDAREVENT[] = "{70680EED-160D-45E9-8F79-D6CBDE8AA875}";
const char CONST_UID_SETTING_GROUP_CALENDAR[] = "{65F8B333-5C21-48A2-8F30-D947C36EF51B}";
const char CONST_UID_SETTING_CALENDAR_NOTE_VIEW_TYPE[] = "{B03FBAC5-F793-4565-92A3-821C87AE3B56}";
const char CONST_UID_SETTING_GROUP_TELEPHONY[] = "{5F122A7F-1E74-4935-88CC-3154E3C207AD}";
const char CONST_UID_SETTING_TELEPHONY_AUTOCLOSEINCOMINGFORM[] = "{10FDBAF9-2E4D-4480-9CFF-ED87E8B31A59}";
const char CONST_UID_SETTING_TELEPHONY_AUTOCLOSEINCOMINGFORMTIMEOUT[] = "{0C9AAC0D-1BDD-4FD9-BEB8-80FAF90D5FDE}";
const char CONST_UID_SETTING_GROUP_OTHER[] = "{0BAEF651-D749-4566-B682-F11D03C6D849}";
const char CONST_UID_SETTING_OTHER_ENABLE_HISTORY_TRACKING[] = "{97D72002-CEF4-4D2A-B00F-7A3E9F1B8DA2}";
const char CONST_UID_SETTING_OTHER_AUTOCLEANTEMPDIR[] = "{118A02D4-4437-496C-BC89-BB81B6CD2391}";
const char CONST_UID_SETTING_OTHER_SELECTED_MENU[] = "{3AD4888F-EDC7-4D69-A97F-F9678B6AAC44}";
const char CONST_UID_SETTING_OTHER_HIDE_OFFLINE_MONITOR_ACTIVITY[] = "{3E366971-6521-47EC-A250-95F51370EC72}";
const char CONST_UID_SETTING_OTHER_DATE_FORMAT[] = "{74031D08-FFA4-4206-9F03-215FCDEB12A0}";
const char CONST_UID_SETTING_OTHER_TIME_FORMAT[] = "{B404CED9-B5FF-45A6-833F-C60B6DC15D32}";
//-----------------------------------------------------------------------------
const char CONST_CONFIG_CONNECTION_SERVER[] = "Connection/Server";
const char CONST_CONFIG_CONNECTION_PORT[] = "Connection/Port";
const char CONST_CONFIG_CONNECTION_DATABASE[] = "Connection/Database";
const char CONST_CONFIG_CONNECTION_UPDATE_DIR[] = "Connection/UpdateDir";
const char CONST_CONFIG_CONNECTION_LOGIN[] = "Connection/Login";
const char CONST_CONFIG_CONNECTION_PASSWORD[] = "Connection/Password";
const char CONST_CONFIG_REMEMBER_USER_INCLUDE[] = "RememberUser/Include";
const char CONST_CONFIG_REMEMBER_USER_LOGIN[] = "RememberUser/Login";
//-----------------------------------------------------------------------------
#endif
