#pragma once
#ifndef _ISDEFINES_H_INCLUDED
#define _ISDEFINES_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
//EXTENSIONS-------------------------------------------------------------------
const char EXTENSION_DLL[] = "dll";
const char EXTENSION_SO[] = "so";
const char EXTENSION_XLS[] = "xls";
const char EXTENSION_XLSX[] = "xlsx";
const char EXTENSION_CSV[] = "csv";
const char EXTENSION_TXT[] = "txt";
const char EXTENSION_HTML[] = "html";
const char EXTENSION_HTM[] = "htm";
const char EXTENSION_DBF[] = "dbf";
const char EXTENSION_PDF[] = "pdf";
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
const char EXTENSION_MONITOR[] = "monitor";
const char EXTENSION_DMP[] = "dmp";
const char EXTENSION_SQL[] = "sql";
const char EXTENSION_WAV[] = "wav";
const char EXTENSION_LIC[] = "lic";
const char EXTENSION_XML[] = "xml";
const char EXTENSION_INI[] = "ini";
//OS_TYPES---------------------------------------------------------------------
const char OS_NAME_WINDOWS[] = "Windows";
const char OS_NAME_LINUX[] = "Linux";
const char OS_NAME_MACOS[] = "Mac OS";
const char OS_NAME_UNKNOWN[] = "Unknown";
//LOCALIZATION-----------------------------------------------------------------
const char LOCALIZATION_FILE_CARAT[] = "Carat";
const char LOCALIZATION_FILE_CONFIGURATOR[] = "Configurator";
const char LOCALIZATION_FILE_CORE[] = "Core";
const char LOCALIZATION_FILE_DATABASE_SERVICE[] = "DatabaseService";
const char LOCALIZATION_FILE_INTEGRAL_SYSTEM[] = "IntegralSystem";
const char LOCALIZATION_FILE_OBJECTS[] = "Objects";
//REG_EXP----------------------------------------------------------------------
const char REG_EXP_IP_ADDRESS[] = "([0-9]{1,3}\\.){3}[0-9]{1,3}(\\/([0-9]|[1-2][0-9]|3[0-2]))?"; //IP-адресс
const char REG_EXP_UID[] = "UID=\"\\{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}\\}\""; //Уникальный идентфикатор из мета-данных
const char REG_EXP_DEBUG[] = "[0-9]{2}.[0-9]{2}.[0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3} \\[[A-Za-z]+\\]: ";
const char REG_EXP_LICENSE[] = "^[a-f0-9]{32}$";
const char REG_EXP_LOGIN[] = "^([a-z]+)$";
const char REG_EXP_BACKUP_FILE_DATE[] = "[0-9]{2}\\.[0-9]{2}\\.[0-9]{4}";
const char REG_EXP_PASSWORD_ALT_SYMBOL[] = "^[^а-яА-ЯёЁa-zA-Z0-9_]+$";
const char REG_EXP_PASSWORD_UPPER[] = "^[А-ЯA-Z]+$";
const char REG_EXP_URL[] = "^(http:\\/\\/www\\.|https:\\/\\/www\\.|http:\\/\\/|https:\\/\\/)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/.*)?$"; //Ссылка на сайт или страницу в интернете
const char REG_EXP_FIAS_CONTENT[] = "<\\w+ .+\\/>";
const char REG_EXP_FIAS_TABLE_NAME[] = "[A-Za-z]+ ";
const char REG_EXP_NUMBER_COUNT_8[] = "^[0-9]{8}$"; //8 цифр
const char REG_EXP_NUMBER_COUNT_9[] = "^[0-9]{9}$"; //9 цифр
const char REG_EXP_NUMBER_COUNT_10[] = "^[0-9]{10}$"; //10 цифр
const char REG_EXP_NUMBER_COUNT_12[] = "^[0-9]{12}$"; //12 цифр
const char REG_EXP_NUMBER_COUNT_13[] = "^[0-9]{13}$"; //13 цифр
const char REG_EXP_NUMBER_COUNT_17[] = "^[0-9]{17}$"; //17 цифр
//DATE_TIME_FORMATS------------------------------------------------------------
const char DATE_FORMAT_V1[] = "d MMMM yyyy";
const char DATE_FORMAT_V2[] = "dd.MM.yyyy";
const char DATE_FORMAT_V3[] = "dd/MM/yyyy";
const char DATE_FORMAT_V4[] = "d MMMM yyyy (ddd)";
const char TIME_FORMAT_V1[] = "hh:mm";
const char TIME_FORMAT_V2[] = "hh.mm";
const char TIME_FORMAT_V3[] = "hh:mm:ss";
const char TIME_FORMAT_V4[] = "mm:ss";
const char TIME_FORMAT_V5[] = "hh.mm.ss";
const char DATE_TIME_FORMAT_V1[] = "d MMMM yyyy hh:mm";
const char DATE_TIME_FORMAT_V2[] = "dd.MM.yyyy hh:mm";
const char DATE_TIME_FORMAT_V3[] = "d MMMM yyyy hh:mm:ss";
const char DATE_TIME_FORMAT_V4[] = "dd.MM.yyyy hh:mm:ss.zzz";
const char DATE_TIME_FORMAT_V5[] = "d MMMM yy hh:mm:ss.zzz";
const char DATE_TIME_FORMAT_V6[] = "dd.MM.yyyy hh.mm";
const char DATE_TIME_FORMAT_V7[] = "yyyy-MM-dd hh:mm:ss";
const char DATE_TIME_FORMAT_V8[] = "dd.MM.yyyy hh.mm.ss";
//TOKENS-----------------------------------------------------------------------
const char TOKEN_DA_DATA_TOKEN[] = "f2c204cd6ba093b294324acec5d01b25fad632cf";
//PROPERTY---------------------------------------------------------------------
const char IS_PROPERTY_LINE_EDIT_SELECTED_MENU[] = "LineEditSelectedMenu";
//-----------------------------------------------------------------------------
const char PATH_SCHEMA_TEMPLATE_FIELDS[] = ":Other/ClassTemplateFields.xml";
const char PATH_CONFIGURATOR_SCHEME[] = ":Other/Configurator.xml";
const char PATH_CONFIGURATION_SCHEME[] = ":Other/Configuration.xml";
//REPORT_TYPE------------------------------------------------------------------
const char REPORT_TYPE_HTML[] = "Html";
const char REPORT_TYPE_WORD[] = "Word";
//TEXT_CODECS------------------------------------------------------------------
const char TEXT_CODEC_IBM866[] = "IBM 866";
//CURSORS----------------------------------------------------------------------
const Qt::CursorShape CURSOR_ARROW = Qt::ArrowCursor;
const Qt::CursorShape CURSOR_POINTING_HAND = Qt::PointingHandCursor;
const Qt::CursorShape CURSOR_WHATS_THIS = Qt::WhatsThisCursor;
const Qt::CursorShape CURSOR_WAIT = Qt::WaitCursor;
const Qt::CursorShape CURSOR_BUSY = Qt::BusyCursor;
const Qt::CursorShape CURSOR_FORBIDDEN = Qt::ForbiddenCursor;
const Qt::CursorShape CURSOR_I_BEAM = Qt::IBeamCursor;
//-----------------------------------------------------------------------------
const int ONE_SECOND_TO_MILLISECOND = 1000;
const int FIVE_SECOND_TO_MILLISECOND = 5000;
//INTERFACE--------------------------------------------------------------------
const int SIZE_MINIMUM_HEIGHT_EDIT_FIELD = 24; //Минимальная высота поля ввода ввода информации
const int EDIT_FIELD_SPACING = 4;
const int ISPUSHBUTTON_MINIMUM_WIDTH = 75;
const int ISPUSHBUTTON_MINIMUM_HEIGHT = 24;
//SYMBOLS----------------------------------------------------------------------
const char SYMBOL_SPACE = ' '; //Символ пробела
const char SYMBOL_NUMBER = '№'; //Символ "Номер"
const char SYMBOL_PLUS = '+'; //Символ "Плюс"
const char SYMBOL_MINUS = '-'; //Символ "Минус"
const char SYMBOL_PERCENT = '%'; //Символ процента
const char SYMBOL_COMMA = ','; //Символ запятой
const char SYMBOL_POINT = '.'; //Символ точки
const char SYMBOL_1 = '1';
const char SYMBOL_2 = '2';
const char SYMBOL_3 = '3';
const char SYMBOL_4 = '4';
const char SYMBOL_5 = '5';
const char SYMBOL_6 = '6';
const char SYMBOL_7 = '7';
const char SYMBOL_8 = '8';
const char SYMBOL_9 = '9';
const char SYMBOL_0 = '0';
//ASTERISK_MANAGER_API---------------------------------------------------------
const char AMI_SUCCESSFUL_AUTH[] = "SuccessfulAuth";
const char AMI_USER_EVENT[] = "UserEvent";
const char AMI_DIAL_BEGIN[] = "DialBegin";
const char AMI_DIAL_END[] = "DialEnd";
const char AMI_HANGUP[] = "Hangup";
const char AMI_BLIND_TRANSFER[] = "BlindTransfer";
const char AMI_ORIGINATE_RESPONSE[] = "OriginateResponse";
const char AMI_EXTENSION_STATUS[] = "ExtensionStatus";
//-----------------------------------------------------------------------------
const char SYSTEM_DATABASE_NAME[] = "postgres";
const char FIGURES_STRING[] = "0123456789";
const char SQL_DRIVER_QPSQL[] = "QPSQL";
const int MAX_QUERY_TIME = 50; //Передельное время выполнения SQL-запроса (в миллисекундах)
const int MAXIMUM_INTEGER = INT_MAX; //Максимальное целое число
const double MAXIMUM_DOUBLE = DBL_MAX; //Максимальное число с плавающей запятой
const int CALENDAR_AUDIO_TIMEOUT = 20000;
const int DURATION_SHOW_HIDE_ANIMATION = 700;
const int DURATION_SHOW_HIDE_SEARCH_FORM = 300;
const char CLASS_IS_LIST_EDIT[] = "ISListEdit";
const int MINIMUM_PASSWORD_LENGHT = 8;
const int CARAT_PORT = 23071;
const int WAIT_LOAD_DATA_LIST_FORM = 400;
const int ASTERISK_RECONNECT_TIMEOUT = 10000;
const char LOG_FILE_ASSERT[] = "ASSERT";
const char LOG_FILE_CRASH[] = "CRASH";
const char DATABASE_CONNECTON_THREAD_QUERY[] = "ThreadQuery";
const char DATABASE_CONNECTON_PATTERN_ACITIVITY[] = "PatternActivity";
const int YEAR_MINIMUM = 1752;
const int YEAR_MAXIMUM = 8000;
const char CARAT_UID[] = "{88DD90C4-2DFF-4515-8C32-7371C39C598A}";
const int MAX_FILE_SIZE_FIELD = ((1000 * 1024) * 10); //Максимальный размер файла в поле редактирования файла (10 мегабайт)
const int PLAYER_WINDING = 10000;
const char COLOR_STANDART[] = "#000000";
const char COLOR_PATTERN_WIDGET_NOT_REGISTERED[] = "#bbbbbb";
//-----------------------------------------------------------------------------
#define DATE_BEFORE_YESTERDAY DATE_TODAY.addDays(-2) //Позавчера
#define DATE_YESTERDAY DATE_TODAY.addDays(-1) //Вчера
#define DATE_TODAY QDate::currentDate() //Сегодня
#define DATE_TOMORROW DATE_TODAY.addDays(1) //Завтра
#define DATE_AFTER_TOMORROW DATE_TODAY.addDays(2) //Послезавтра
//-----------------------------------------------------------------------------
LIBCORE_EXPORT void DefinesInitialize(); //Инициализация вычисляемых дефайнов
//-----------------------------------------------------------------------------
extern LIBCORE_EXPORT QString APPLICATION_NAME;
extern LIBCORE_EXPORT QString PATH_APPLICATION_DIR;
extern LIBCORE_EXPORT QString PATH_APPLICATION_FILE;
extern LIBCORE_EXPORT QString PATH_LOGS_DIR;
extern LIBCORE_EXPORT QString PATH_TEMP_DIR;
extern LIBCORE_EXPORT QString PATH_TRANSLATIONS_DIR;
extern LIBCORE_EXPORT QString PATH_CONFIG_FILE;
extern LIBCORE_EXPORT QString SYMBOL_CIRCLE; //Символ "Круг"
extern LIBCORE_EXPORT QString SYMBOL_MULTIPLY; //Символ "Умножение"
extern LIBCORE_EXPORT QString SYMBOL_DIVIDE; //Символ "Деление"
extern LIBCORE_EXPORT QString SYMBOL_OBJECT_CHANGED;
extern LIBCORE_EXPORT QString SYMBOL_SPACE_HIDE; //Невидмый символ (НЕ ЯВЛЯЕТСЯ СТАНДАРТНЫМ СИМВОЛОМ ПРОБЕЛА)
extern LIBCORE_EXPORT QString SYMBOL_FIAS_SPLIT;
//#ifdef GUI
extern LIBCORE_EXPORT QStringList AVIABLE_IMAGE_EXTENSION;
extern LIBCORE_EXPORT QMargins MARGINS_LAYOUT_NULL;
extern LIBCORE_EXPORT QMargins MARGINS_LAYOUT_1_PX;
extern LIBCORE_EXPORT QMargins MARGINS_LAYOUT_2_PX;
extern LIBCORE_EXPORT QMargins MARGINS_LAYOUT_4_PX;
extern LIBCORE_EXPORT QMargins MARGINS_LAYOUT_5_PX;
extern LIBCORE_EXPORT QMargins MARGINS_LAYOUT_10_PX;
extern LIBCORE_EXPORT QSize SIZE_MAIN_WINDOW;
extern LIBCORE_EXPORT QSize SIZE_MAIN_WINDOW_MINIMUM;
extern LIBCORE_EXPORT QSize SIZE_TASK_OBJECT_FORM;
extern LIBCORE_EXPORT QSize SIZE_AUTHORIZATION_FORM;
extern LIBCORE_EXPORT QSize SIZE_PROGRESS_FORM;
extern LIBCORE_EXPORT QSize SIZE_640_480;
extern LIBCORE_EXPORT QSize SIZE_500_90;
extern LIBCORE_EXPORT QSize SIZE_450_450;
extern LIBCORE_EXPORT QSize SIZE_550_300;
extern LIBCORE_EXPORT QSize SIZE_200_200;
extern LIBCORE_EXPORT QSize SIZE_100_100;
extern LIBCORE_EXPORT QSize SIZE_64_64;
extern LIBCORE_EXPORT QSize SIZE_45_45;
extern LIBCORE_EXPORT QSize SIZE_32_32;
extern LIBCORE_EXPORT QSize SIZE_25_25;
extern LIBCORE_EXPORT QSize SIZE_24_24;
extern LIBCORE_EXPORT QSize SIZE_22_22;
extern LIBCORE_EXPORT QSize SIZE_20_20;
extern LIBCORE_EXPORT QSize SIZE_18_18;
extern LIBCORE_EXPORT QSize SIZE_16_16;
extern LIBCORE_EXPORT QSize SIZE_10_10;
extern LIBCORE_EXPORT QColor EDIT_WIDGET_COLOR_RED; //Цвет рамки вокруг незаполненного поля реадктирования информации
extern LIBCORE_EXPORT QColor COLOR_BACKGROUND_INTERFACE; //Цвет фона форм интерфейса
extern LIBCORE_EXPORT QColor COLOR_MAIN_MENU_BAR;
extern LIBCORE_EXPORT QColor COLOR_WIDGET_BOX_TITLE;
extern LIBCORE_EXPORT QColor COLOR_CALENDAR_SELECTED_DATE_BACKGROUND;
extern LIBCORE_EXPORT QColor COLOR_CALENDAR_EVENT_FORM_FLASH;
extern LIBCORE_EXPORT QColor COLOR_CALENDAR_CELL_BACKGROUP;
extern LIBCORE_EXPORT QColor COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH;
extern LIBCORE_EXPORT QColor COLOR_SPLASH_SCREEN_TEXT;
extern LIBCORE_EXPORT QColor COLOR_MAIN_WINDOW_SHADOW;
extern LIBCORE_EXPORT QColor COLOR_POPUP_BRUSH;
extern LIBCORE_EXPORT QColor COLOR_FLASH_INCOMING_FORM;
extern LIBCORE_EXPORT QColor COLOR_WHITE;
extern LIBCORE_EXPORT QColor COLOR_RED;
extern LIBCORE_EXPORT QColor COLOR_BLUE;
extern LIBCORE_EXPORT QColor COLOR_BLACK;
extern LIBCORE_EXPORT QColor COLOR_DARK_GREEN;
extern LIBCORE_EXPORT QColor COLOR_LIGHT_GRAY;
extern LIBCORE_EXPORT QColor COLOR_MONITOR_USER;
extern LIBCORE_EXPORT QColor COLOR_NULL;
extern LIBCORE_EXPORT QFont FONT_COURIER_12;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_35;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_18;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_15;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_15_BOLD;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_14;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_14_BOLD;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_12;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_12_BOLD;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_11;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_10_BOLD;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_10;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_9;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_9_BOLD;
extern LIBCORE_EXPORT QFont FONT_TAHOMA_8;
extern LIBCORE_EXPORT QFont FONT_APPLICATION;
extern LIBCORE_EXPORT QFont FONT_APPLICATION_BOLD;
//#endif
//-----------------------------------------------------------------------------
#endif
