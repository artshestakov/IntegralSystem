#pragma once
#ifndef _ISDEFINES_H_INCLUDED
#define _ISDEFINES_H_INCLUDED
//-----------------------------------------------------------------------------
//All
const QString APPLICATION_NAME = QApplication::applicationName();
const QString APPLICATION_DIR_PATH = QApplication::applicationDirPath();
const QString APPLICATION_FILE_PATH = QApplication::applicationFilePath();
const QString APPLICATION_LOGS_PATH = APPLICATION_DIR_PATH + "/Logs";
const QString APPLICATION_TEMP_PATH = APPLICATION_DIR_PATH + "/Temp";
const QString APPLICATION_TRANSLATIONS_DIR = APPLICATION_DIR_PATH + "/translations";
const QString CONFIG_FILE_PATH = APPLICATION_DIR_PATH + "/Config.ini";
const QString DEBUG_VIEW_PATH = APPLICATION_DIR_PATH + "/DebugView.exe";
const char SCHEMA_TEMPLATE_FIELDS_PATH[] = ":Other/ClassTemplateFields.xml";
const char CONFIGURATOR_SCHEME_PATH[] = ":Other/Configurator.xml";
const char CONFIGURATION_SCHEME_PATH[] = ":Other/Configuration.xml";
//-----------------------------------------------------------------------------
//PROPERTY
const char IS_PROPERTY_LINE_EDIT_SELECTED_MENU[] = "LineEditSelectedMenu";
//-----------------------------------------------------------------------------
//OS names
const char OS_NAME_WINDOWS[] = "Windows";
const char OS_NAME_LINUX[] = "Linux";
const char OS_NAME_MACOS[] = "Mac OS";
const char OS_NAME_UNKNOWN[] = "Unknown";
//-----------------------------------------------------------------------------
//Thread Milliseconds Values
const int ONE_SECOND_TO_MILLISECOND = 1000;
const int FIVE_SECOND_TO_MILLISECOND = 5000;
//-----------------------------------------------------------------------------
//INTERFACE
const int SIZE_MINIMUM_HEIGHT_EDIT_FIELD = 24; //Минимальная высота поля ввода ввода информации
const int EDIT_FIELD_SPACING = 4;
const QMargins LAYOUT_MARGINS_NULL = QMargins(0, 0, 0, 0);
const QMargins LAYOUT_MARGINS_1_PX = QMargins(1, 1, 1, 1);
const QMargins LAYOUT_MARGINS_2_PX = QMargins(2, 2, 2, 2);
const QMargins LAYOUT_MARGINS_4_PX = QMargins(4, 4, 4, 4);
const QMargins LAYOUT_MARGINS_5_PX = QMargins(5, 5, 5, 5);
const QMargins LAYOUT_MARGINS_10_PX = QMargins(10, 10, 10, 10);
const int ISPUSHBUTTON_MINIMUM_WIDTH = 75;
const int ISPUSHBUTTON_MINIMUM_HEIGHT = 24;
//-----------------------------------------------------------------------------
//SIZES
const QSize SIZE_MAIN_WINDOW = QSize(800, 559); //Размер главного окна
const QSize SIZE_MAIN_WINDOW_MINIMUM = QSize(700, 500); //Минимальный размер главного окна
const QSize SIZE_TASK_OBJECT_FORM = QSize(880, 520); //Начальный размер формы объекта задачи
const QSize SIZE_AUTHORIZATION_FORM = QSize(350, 450); //Размер формы авторизации
const QSize SIZE_PROGRESS_FORM = QSize(550, 100); //Размер формы прогресса
const QSize SIZE_640_480 = QSize(640, 480);
const QSize SIZE_500_90 = QSize(500, 90);
const QSize SIZE_450_450 = QSize(450, 450);
const QSize SIZE_550_300 = QSize(550, 300);
const QSize SIZE_200_200 = QSize(200, 200);
const QSize SIZE_100_100 = QSize(100, 100);
const QSize SIZE_64_64 = QSize(64, 64);
const QSize SIZE_45_45 = QSize(45, 45);
const QSize SIZE_32_32 = QSize(32, 32);
const QSize SIZE_25_25 = QSize(25, 25);
const QSize SIZE_24_24 = QSize(24, 24);
const QSize SIZE_22_22 = QSize(22, 22);
const QSize SIZE_20_20 = QSize(20, 20);
const QSize SIZE_18_18 = QSize(18, 18);
const QSize SIZE_16_16 = QSize(16, 16);
const QSize SIZE_10_10 = QSize(10, 10);
//-----------------------------------------------------------------------------
//CURSORS
const Qt::CursorShape CURSOR_ARROW = Qt::ArrowCursor;
const Qt::CursorShape CURSOR_POINTING_HAND = Qt::PointingHandCursor;
const Qt::CursorShape CURSOR_WHATS_THIS = Qt::WhatsThisCursor;
const Qt::CursorShape CURSOR_WAIT = Qt::WaitCursor;
const Qt::CursorShape CURSOR_BUSY = Qt::BusyCursor;
const Qt::CursorShape CURSOR_FORBIDDEN = Qt::ForbiddenCursor;
const Qt::CursorShape CURSOR_I_BEAM = Qt::IBeamCursor;
//-----------------------------------------------------------------------------
//LOCALIZATION
const char LOCALIZATION_FILE_CARAT[] = "Carat";
const char LOCALIZATION_FILE_CONFIGURATOR[] = "Configurator";
const char LOCALIZATION_FILE_CORE[] = "Core";
const char LOCALIZATION_FILE_DATABASE_SERVICE[] = "DatabaseService";
const char LOCALIZATION_FILE_INTEGRAL_SYSTEM[] = "IntegralSystem";
const char LOCALIZATION_FILE_OBJECTS[] = "Objects";
//-----------------------------------------------------------------------------
//SYMBOLS
const QString SYMBOL_CIRCLE = QString(QChar(9679)); //Символ "Круг"
const QString SYMBOL_MULTIPLY = QString(QChar(215)); //Символ "Умножение"
const QString SYMBOL_DIVIDE = QString(QChar(247)); //Символ "Деление"
const QString SYMBOL_OBJECT_CHANGED = QString(" {*}");
const char SYMBOL_SPACE = ' '; //Символ пробела
const char SYMBOL_NUMBER = '№'; //Символ "Номер"
const QString SYMBOL_SPACE_HIDE = QString::fromLocal8Bit(" "); //Невидмый символ (НЕ ЯВЛЯЕТСЯ СТАНДАРТНЫМ СИМВОЛОМ ПРОБЕЛА)
const char SYMBOL_PLUS = '+'; //Символ "Плюс"
const char SYMBOL_MINUS = '-'; //Символ "Минус"
const char SYMBOL_PERCENT = '%'; //Символ процента
const char SYMBOL_COMMA = ','; //Символ запятой
const QString SYMBOL_FIAS_SPLIT = QString(SYMBOL_COMMA + SYMBOL_SPACE_HIDE);
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
//-----------------------------------------------------------------------------
//FileExtensions
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
//-----------------------------------------------------------------------------
//SYSTEM
const char SYSTEM_DATABASE_NAME[] = "postgres";
const char FIGURES_STRING[] = "0123456789";
const char SQL_DRIVER_QPSQL[] = "QPSQL";
const int MAX_QUERY_TIME = 50; //Передельное время выполнения SQL-запроса (в миллисекундах)
const int INTEGER_MAXIMUM = INT_MAX; //Максимальное целое число
const double DOUBLE_MAXIMUM = DBL_MAX; //Максимальное число с плавающей запятой
const int CURRENT_PID = QApplication::applicationPid();
const QStringList AVIABLE_IMAGE_EXTENSION = QStringList() << EXTENSION_PNG << EXTENSION_JPEG << EXTENSION_JPG << EXTENSION_DDS << EXTENSION_GIF << EXTENSION_ICNS << EXTENSION_ICO << EXTENSION_SVG << EXTENSION_TGA << EXTENSION_TIFF << EXTENSION_WBMP << EXTENSION_BMP << EXTENSION_WEBP;
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
//-----------------------------------------------------------------------------
//FEEDBACK
const char FEEDBACK_SERVER[] = "smtp.yandex.ru";
const int FEEDBACK_PORT = 465;
const char FEEDBACK_SENDER[] = "integralfeedback@yandex.ru";
const char FEEDBACK_SENDER_PASSWORD[] = "QWPLH34JA961";
const char FEEDBACK_RECIPIENT[] = "art.shestakov@icloud.com";
//-----------------------------------------------------------------------------
//CARAT INFORMER
const char CARAT_INFORMER_NAME[] = "Carat-Informer";
const char CARAT_INFORMER_SERVER[] = "smtp.yandex.ru";
const int CARAT_INFORMER_PORT = 465;
const char CARAT_INFORMER_LOGIN[] = "carat-informer@yandex.ru";
const char CARAT_INFORMER_PASSWORD[] = "QWPLH34JA961";
const char CARAT_INFORMER_RECIPIENT[] = "art.shestakov@icloud.com";
//-----------------------------------------------------------------------------
//TOKENS
const char TOKEN_DA_DATA_TOKEN[] = "f2c204cd6ba093b294324acec5d01b25fad632cf";
//-----------------------------------------------------------------------------
//REPORT TYPE
const char REPORT_TYPE_HTML[] = "Html";
const char REPORT_TYPE_WORD[] = "Word";
//-----------------------------------------------------------------------------
//TEXT CODECS
const char TEXT_CODEC_IBM866[] = "IBM 866";
//-----------------------------------------------------------------------------
//RegExp's
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
//-----------------------------------------------------------------------------
//DateTimes Formats
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
#define DATE_BEFORE_YESTERDAY DATE_TODAY.addDays(-2) //Позавчера
#define DATE_YESTERDAY DATE_TODAY.addDays(-1) //Вчера
#define DATE_TODAY QDate::currentDate() //Сегодня
#define DATE_TOMORROW DATE_TODAY.addDays(1) //Завтра
#define DATE_AFTER_TOMORROW DATE_TODAY.addDays(2) //Послезавтра
//-----------------------------------------------------------------------------
//Asterisk Manager API
const char AMI_SUCCESSFUL_AUTH[] = "SuccessfulAuth";
const char AMI_USER_EVENT[] = "UserEvent";
const char AMI_DIAL_BEGIN[] = "DialBegin";
const char AMI_DIAL_END[] = "DialEnd";
const char AMI_HANGUP[] = "Hangup";
const char AMI_BLIND_TRANSFER[] = "BlindTransfer";
const char AMI_ORIGINATE_RESPONSE[] = "OriginateResponse";
const char AMI_EXTENSION_STATUS[] = "ExtensionStatus";
//-----------------------------------------------------------------------------
#ifdef GUI
//COLORS
const QColor EDIT_WIDGET_COLOR_RED = QColor(236, 99, 99); //Цвет рамки вокруг незаполненного поля реадктирования информации
const QColor COLOR_BACKGROUND_INTERFACE = QColor(230, 230, 230); //Цвет фона форм интерфейса
const QColor COLOR_WHITE = QColor(Qt::white); //Цвет фона главной формы приложения
const QColor COLOR_MAIN_MENU_BAR = QColor(81, 145, 189);
const QColor COLOR_WIDGET_BOX_TITLE = QColor(235, 235, 235);
const QColor COLOR_CALENDAR_SELECTED_DATE_BACKGROUND = QColor(215, 229, 240);
const QColor COLOR_CALENDAR_EVENT_FORM_FLASH = QColor(223, 225, 255);
const QColor COLOR_CALENDAR_CELL_BACKGROUP = QColor(245, 245, 245);
const QColor COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH = QColor(245, 245, 245);
const QColor COLOR_SPLASH_SCREEN_TEXT = QColor(37, 77, 158);
const QColor COLOR_MAIN_WINDOW_SHADOW = QColor(0, 0, 0, 130);
const QColor COLOR_POPUP_BRUSH = QColor(0, 0, 0, 180);
const QColor COLOR_FLASH_INCOMING_FORM = QColor(226, 235, 255);
const char COLOR_STANDART[] = "#000000";
const char COLOR_PATTERN_WIDGET_NOT_REGISTERED[] = "#bbbbbb";
const QColor COLOR_RED = QColor(Qt::red);
const QColor COLOR_BLUE = QColor(Qt::blue);
const QColor COLOR_BLACK = QColor(Qt::black);
const QColor COLOR_DARK_GREEN = QColor(Qt::darkGreen);
const QColor COLOR_LIGHT_GRAY = QColor(Qt::lightGray);
const QColor COLOR_MONITOR_USER = QColor(210, 210, 210);
const QColor COLOR_NULL = QColor();
//-----------------------------------------------------------------------------
//FONTS
const QFont FONT_COURIER_12 = QFont("Courier", 12);
const QFont FONT_TAHOMA_35 = QFont("Tahoma", 35);
const QFont FONT_TAHOMA_18 = QFont("Tahoma", 18);
const QFont FONT_TAHOMA_15 = QFont("Tahoma", 15);
const QFont FONT_TAHOMA_15_BOLD = QFont("Tahoma", 15, QFont::Bold);
const QFont FONT_TAHOMA_14 = QFont("Tahoma", 14);
const QFont FONT_TAHOMA_14_BOLD = QFont("Tahoma", 14, QFont::Bold);
const QFont FONT_TAHOMA_12 = QFont("Tahoma", 12);
const QFont FONT_TAHOMA_12_BOLD = QFont("Tahoma", 12, QFont::Bold);
const QFont FONT_TAHOMA_11 = QFont("Tahoma", 11);
const QFont FONT_TAHOMA_10_BOLD = QFont("Tahoma", 10, QFont::Bold);
const QFont FONT_TAHOMA_10 = QFont("Tahoma", 10);
const QFont FONT_TAHOMA_9 = QFont("Tahoma", 9);
const QFont FONT_TAHOMA_9_BOLD = QFont("Tahoma", 9, QFont::Bold);
const QFont FONT_TAHOMA_8 = QFont("Tahoma", 8);
const QFont FONT_APPLICATION = FONT_TAHOMA_9;
const QFont FONT_APPLICATION_BOLD = FONT_TAHOMA_9_BOLD;
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
#endif
