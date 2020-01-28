#pragma once
//-----------------------------------------------------------------------------

//All
#define APPLICATION_NAME QApplication::applicationName()
#define APPLICATION_DIR_PATH QApplication::applicationDirPath()
#define APPLICATION_FILE_PATH QApplication::applicationFilePath()
#define APPLICATION_LOGS_PATH QString(APPLICATION_DIR_PATH + "/Logs")
#define APPLICATION_TEMP_PATH QString(APPLICATION_DIR_PATH + "/Temp")
#define APPLICATION_TRANSLATIONS_DIR QString(APPLICATION_DIR_PATH + "/translations")
#define CONFIG_FILE_PATH QString(APPLICATION_DIR_PATH + "/Config.ini")
#define DEBUG_VIEW_PATH QString(APPLICATION_DIR_PATH + "/DebugView.exe")
#define SCHEMA_TEMPLATE_FIELDS_PATH ":Other/ClassTemplateFields.xml"
#define CONFIGURATOR_SCHEME_PATH ":Other/Configurator.xml"
#define CONFIGURATION_SCHEME_PATH ":Other/Configuration.xml"

//DATABASE SERVICE
#define PG_DUMP_WINDOWS "pg_dump.exe"
#define PG_DUMP_LINUX "pg_dump"
#define PG_DUMPALL_WINDOWS "pg_dumpall.exe"
#define PG_DUMPALL_LINUX "pg_dumpall"

//PROPERTY
#define IS_PROPERTY_LINE_EDIT_SELECTED_MENU "LineEditSelectedMenu"

//OS names
#define OS_NAME_WINDOWS "Windows"
#define OS_NAME_LINUX "Linux"
#define OS_NAME_MACOS "Mac OS"
#define OS_NAME_UNKNOWN "Unknown"

//Thread Milliseconds Values
const int ONE_SECOND_TO_MILLISECOND = 1000;
const int FIVE_SECOND_TO_MILLISECOND = 5000;

//INTERFACE
const int SIZE_MINIMUM_HEIGHT_EDIT_FIELD = 24; //Минимальная высота поля ввода ввода информации
const int EDIT_FIELD_SPACING = 4;
#define LAYOUT_MARGINS_NULL QMargins(0, 0, 0, 0)
#define LAYOUT_MARGINS_1_PX QMargins(1, 1, 1, 1)
#define LAYOUT_MARGINS_2_PX QMargins(2, 2, 2, 2)
#define LAYOUT_MARGINS_4_PX QMargins(4, 4, 4, 4)
#define LAYOUT_MARGINS_5_PX QMargins(5, 5, 5, 5)
#define LAYOUT_MARGINS_10_PX QMargins(10, 10, 10, 10)
const int ISPUSHBUTTON_MINIMUM_WIDTH = 75;
const int ISPUSHBUTTON_MINIMUM_HEIGHT = 24;

//SIZES
#define SIZE_MAIN_WINDOW QSize(800, 559) //Размер главного окна
#define SIZE_MAIN_WINDOW_MINIMUM QSize(700, 500) //Минимальный размер главного окна
#define SIZE_TASK_OBJECT_FORM QSize(880, 520) //Начальный размер формы объекта задачи
#define SIZE_AUTHORIZATION_FORM QSize(350, 450) //Размер формы авторизации
#define SIZE_PROGRESS_FORM QSize(550, 100) //Размер формы прогресса
#define SIZE_SYSTEM_ICON SIZE_32_32 //Размер иконки системы на панели
#define SIZE_750_400 QSize(750, 400)
#define SIZE_700_500 QSize(700, 500)
#define SIZE_640_480 QSize(640, 480)
#define SIZE_500_90 QSize(500, 90)
#define SIZE_500_65 QSize(500, 65)
#define SIZE_450_450 QSize(450, 450)
#define SIZE_550_300 QSize(550, 300)
#define SIZE_390_230 QSize(390, 230)
#define SIZE_350_215 QSize(350, 215)
#define SIZE_320_60 QSize(320, 60)
#define SIZE_300_300 QSize(300, 300)
#define SIZE_256_256 QSize(256, 256)
#define SIZE_200_200 QSize(200, 200)
#define SIZE_120_120 QSize(120, 120)
#define SIZE_100_100 QSize(100, 100)
#define SIZE_64_64 QSize(64, 64)
#define SIZE_50_35 QSize(50, 35)
#define SIZE_45_45 QSize(45, 45)
#define SIZE_40_40 QSize(40, 40)
#define SIZE_35_35 QSize(35, 35)
#define SIZE_32_32 QSize(32, 32)
#define SIZE_25_25 QSize(25, 25)
#define SIZE_24_24 QSize(24, 24)
#define SIZE_22_22 QSize(22, 22)
#define SIZE_20_20 QSize(20, 20)
#define SIZE_18_18 QSize(18, 18)
#define SIZE_16_16 QSize(16, 16)
#define SIZE_10_10 QSize(10, 10)
#define SIZE_NULL QSize(0, 0)

//FONTS
#define FONT_APPLICATION_STRING QApplication::font().toString()
#define FONT_APPLICATION FONT_TAHOMA_9
#define FONT_APPLICATION_BOLD FONT_TAHOMA_9_BOLD

#define FONT_ARIAL_12_BOLD QFont("Arial", 12, QFont::Bold)
#define FONT_COURIER_12 QFont("Courier", 12)
#define FONT_TAHOMA_35 QFont("Tahoma", 35)
#define FONT_TAHOMA_18 QFont("Tahoma", 18)
#define FONT_TAHOMA_15 QFont("Tahoma", 15)
#define FONT_TAHOMA_15_BOLD QFont("Tahoma", 15, QFont::Bold)
#define FONT_TAHOMA_14 QFont("Tahoma", 14)
#define FONT_TAHOMA_14_BOLD QFont("Tahoma", 14, QFont::Bold)
#define FONT_TAHOMA_13_BOLD QFont("Tahoma", 13, QFont::Bold)
#define FONT_TAHOMA_12 QFont("Tahoma", 12)
#define FONT_TAHOMA_12_BOLD QFont("Tahoma", 12, QFont::Bold)
#define FONT_TAHOMA_11 QFont("Tahoma", 11)
#define FONT_TAHOMA_10_BOLD QFont("Tahoma", 10, QFont::Bold)
#define FONT_TAHOMA_10 QFont("Tahoma", 10)
#define FONT_TAHOMA_9 QFont("Tahoma", 9)
#define FONT_TAHOMA_9_BOLD QFont("Tahoma", 9, QFont::Bold)
#define FONT_TAHOMA_8 QFont("Tahoma", 8)

//CURSORS
#define CURSOR_ARROW Qt::ArrowCursor
#define CURSOR_POINTING_HAND Qt::PointingHandCursor
#define CURSOR_WHATS_THIS Qt::WhatsThisCursor
#define CURSOR_WAIT Qt::WaitCursor
#define CURSOR_BUSY Qt::BusyCursor
#define CURSOR_FORBIDDEN Qt::ForbiddenCursor
#define CURSOR_I_BEAM Qt::IBeamCursor

//LOCALIZATION
#define LOCALIZATION_FILE_CARAT "Carat"
#define LOCALIZATION_FILE_CONFIGURATOR "Configurator"
#define LOCALIZATION_FILE_CORE "Core"
#define LOCALIZATION_FILE_DATABASE_SERVICE "DatabaseService"
#define LOCALIZATION_FILE_INTEGRAL_SYSTEM "IntegralSystem"
#define LOCALIZATION_FILE_OBJECTS "Objects"

//SYMBOLS
#define SYMBOL_CIRCLE QString(QChar(9679)) //Символ "Круг"
#define SYMBOL_MULTIPLY QString(QChar(215)) //Символ "Умножение"
#define SYMBOL_DIVIDE QString(QChar(247)) //Символ "Деление"
#define SYMBOL_OBJECT_CHANGED QString(" {*}")
#define SYMBOL_SPACE " " //Символ пробела
#define SYMBOL_NUMBER "№" //Символ "Номер"
#define SYMBOL_SPACE_HIDE QString::fromLocal8Bit(" ") //Невидмый символ (НЕ ЯВЛЯЕТСЯ СТАНДАРТНЫМ СИМВОЛОМ ПРОБЕЛА)
#define SYMBOL_PLUS "+" //Символ "Плюс"
#define SYMBOL_MINUS "-" //Символ "Минус"
#define SYMBOL_PERCENT "%" //Символ процента
#define SYMBOL_COMMA "," //Символ запятой
#define SYMBOL_FIAS_SPLIT QString(SYMBOL_COMMA + SYMBOL_SPACE_HIDE)
#define SYMBOL_1 "1"
#define SYMBOL_2 "2"
#define SYMBOL_3 "3"
#define SYMBOL_4 "4"
#define SYMBOL_5 "5"
#define SYMBOL_6 "6"
#define SYMBOL_7 "7"
#define SYMBOL_8 "8"
#define SYMBOL_9 "9"
#define SYMBOL_0 "0"

//COLORS
#define EDIT_WIDGET_COLOR_RED QColor(236, 99, 99) //Цвет рамки вокруг незаполненного поля реадктирования информации
#define COLOR_BACKGROUND_INTERFACE QColor(230, 230, 230) //Цвет фона форм интерфейса
#define COLOR_WHITE QColor(Qt::white) //Цвет фона главной формы приложения
#define COLOR_MAIN_MENU_BAR QColor(81, 145, 189)
#define COLOR_WIDGET_BOX_TITLE QColor(235, 235, 235)
#define COLOR_CALENDAR_SELECTED_DATE_BACKGROUND QColor(215, 229, 240)
#define COLOR_CALENDAR_EVENT_FORM_FLASH QColor(223, 225, 255)
#define COLOR_CALENDAR_CELL_BACKGROUP QColor(245, 245, 245)
#define COLOR_CAALENDAR_DAY_NOT_CURRENT_MONTH QColor(245, 245, 245)
#define COLOR_SPLASH_SCREEN_TEXT QColor(37, 77, 158)
#define COLOR_MAIN_WINDOW_SHADOW QColor(0, 0, 0, 130)
#define COLOR_POPUP_BRUSH QColor(0, 0, 0, 180)
#define COLOR_FLASH_INCOMING_FORM QColor(226, 235, 255)
#define COLOR_STANDART "#000000"
#define COLOR_PATTERN_WIDGET_NOT_REGISTERED "#bbbbbb"
#define COLOR_RED QColor(Qt::red)
#define COLOR_BLUE QColor(Qt::blue)
#define COLOR_BLACK QColor(Qt::black)
#define COLOR_DARK_GREEN QColor(Qt::darkGreen)
#define COLOR_LIGHT_GRAY QColor(Qt::lightGray)
#define COLOR_MONITOR_USER QColor(210, 210, 210)
#define COLOR_NULL QColor()

//SYSTEM
#define SYSTEM_DATABASE_NAME "postgres"
#define SOURCE_LINE __LINE__
#define SOURCE_FILE __FILE__
#define FIGURES_STRING "0123456789"
#define EXIT_CODE_NORMAL EXIT_SUCCESS
#define EXIT_CODE_ERROR EXIT_FAILURE
#define SQL_DRIVER_QPSQL "QPSQL"
const int MAX_QUERY_TIME = 50; //Передельное время выполнения SQL-запроса (в миллисекундах)
#define INTEGER_MAXIMUM INT_MAX //Максимальное целое число
#define DOUBLE_MAXIMUM DBL_MAX //Максимальное число с плавающей запятой
#define CURRENT_PID QApplication::applicationPid()
#define AVIABLE_IMAGE_EXTENSION QStringList() << EXTENSION_PNG << EXTENSION_JPEG  << EXTENSION_JPG << EXTENSION_DDS << EXTENSION_GIF << EXTENSION_ICNS << EXTENSION_ICO << EXTENSION_SVG << EXTENSION_TGA << EXTENSION_TIFF << EXTENSION_WBMP << EXTENSION_BMP << EXTENSION_WEBP
const int CALENDAR_AUDIO_TIMEOUT = 20000;
const int DURATION_SHOW_HIDE_ANIMATION = 700;
const int DURATION_SHOW_HIDE_SEARCH_FORM = 300;
#define CLASS_IS_LIST_EDIT "ISListEdit"
#define CLIPBOARD_TEXT QApplication::clipboard()->text()
#define CLIPBOARD_PIXMAP QApplication::clipboard()->pixmap()
const int MINIMUM_PASSWORD_LENGHT = 8;
const int CARAT_PORT = 23071;
const int WAIT_LOAD_DATA_LIST_FORM = 400;
const int ASTERISK_RECONNECT_TIMEOUT = 10000;
#define LOG_FILE_ASSERT "ASSERT"
#define LOG_FILE_CRASH "CRASH"
#define DATABASE_CONNECTON_THREAD_QUERY "ThreadQuery"
#define DATABASE_CONNECTON_PATTERN_ACITIVITY "PatternActivity"
const int YEAR_MINIMUM = 1752;
const int YEAR_MAXIMUM = 8000;
#define CARAT_UID "{88DD90C4-2DFF-4515-8C32-7371C39C598A}"
const int MAX_FILE_SIZE_FIELD = ((1000 * 1024) * 10); //Максимальный размер файла в поле редактирования файла (10 мегабайт)
const int PLAYER_WINDING = 10000;

//FEEDBACK
#define FEEDBACK_SERVER "smtp.yandex.ru"
#define FEEDBACK_PORT 465
#define FEEDBACK_SENDER "integralfeedback@yandex.ru"
#define FEEDBACK_SENDER_PASSWORD "QWPLH34JA961"
#define FEEDBACK_RECIPIENT "artemshestakov@outlook.com"

//CARAT INFORMER
#define CARAT_INFORMER_NAME "Carat-Informer"
#define CARAT_INFORMER_SERVER "smtp.yandex.ru"
const int CARAT_INFORMER_PORT = 465;
#define CARAT_INFORMER_LOGIN "carat-informer@yandex.ru"
#define CARAT_INFORMER_PASSWORD "QWPLH34JA961"
#define CARAT_INFORMER_RECIPIENT "artemshestakov@outlook.com"

//TOKENS
#define TOKEN_DA_DATA_TOKEN "f2c204cd6ba093b294324acec5d01b25fad632cf"
#define TOKEN_OPEN_WEATHER_MAP_TOKEN "d4e703421760e0ca1ca5944ac1b13692"
#define TOKEN_TELEGRAM "504082486:AAE1PIdf1Oqdi5jkVeHjMsL_za5hl3UEeOo"

//REPORT TYPE
#define REPORT_TYPE_HTML "Html"
#define REPORT_TYPE_WORD "Word"

//TEXT CODECS
#define TEXT_CODEC_IBM866 "IBM 866"

//RegExp's
#define REG_EXP_IP_ADDRESS "([0-9]{1,3}\\.){3}[0-9]{1,3}(\\/([0-9]|[1-2][0-9]|3[0-2]))?" //IP-адресс
#define REG_EXP_UID "UID=\"\\{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}\\}\"" //Уникальный идентфикатор из мета-данных
#define REG_EXP_DEBUG "[0-9]{2}.[0-9]{2}.[0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3} \\[[A-Za-z]+\\]: "
#define REG_EXP_LICENSE "^[a-f0-9]{32}$"
#define REG_EXP_LOGIN "^([a-z]+)$"
#define REG_EXP_BACKUP_FILE_DATE "[0-9]{2}\\.[0-9]{2}\\.[0-9]{4}"
#define REG_EXP_PASSWORD_ALT_SYMBOL "^[^а-яА-ЯёЁa-zA-Z0-9_]+$"
#define REG_EXP_PASSWORD_UPPER "^[А-ЯA-Z]+$"
#define REG_EXP_URL "^(http:\\/\\/www\\.|https:\\/\\/www\\.|http:\\/\\/|https:\\/\\/)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/.*)?$" //Ссылка на сайт или страницу в интернете
#define REG_EXP_FIAS_CONTENT "<\\w+ .+\\/>"
#define REG_EXP_FIAS_TABLE_NAME "[A-Za-z]+ "
#define REG_EXP_NUMBER_COUNT_8 "^[0-9]{8}$" //8 цифр
#define REG_EXP_NUMBER_COUNT_9 "^[0-9]{9}$" //9 цифр
#define REG_EXP_NUMBER_COUNT_10 "^[0-9]{10}$" //10 цифр
#define REG_EXP_NUMBER_COUNT_12 "^[0-9]{12}$" //12 цифр
#define REG_EXP_NUMBER_COUNT_13 "^[0-9]{13}$" //13 цифр
#define REG_EXP_NUMBER_COUNT_17 "^[0-9]{17}$" //17 цифр

//DateTimes Formats
#define DATE_FORMAT_V1 "d MMMM yyyy"
#define DATE_FORMAT_V2 "dd.MM.yyyy"
#define DATE_FORMAT_V3 "dd/MM/yyyy"
#define DATE_FORMAT_V4 "d MMMM yyyy (ddd)"
#define TIME_FORMAT_V1 "hh:mm"
#define TIME_FORMAT_V2 "hh.mm"
#define TIME_FORMAT_V3 "hh:mm:ss"
#define TIME_FORMAT_V4 "mm:ss"
#define TIME_FORMAT_V5 "hh.mm.ss"
#define DATE_TIME_FORMAT_V1 "d MMMM yyyy hh:mm"
#define DATE_TIME_FORMAT_V2 "dd.MM.yyyy hh:mm"
#define DATE_TIME_FORMAT_V3 "d MMMM yyyy hh:mm:ss"
#define DATE_TIME_FORMAT_V4 "dd.MM.yyyy hh:mm:ss.zzz"
#define DATE_TIME_FORMAT_V5 "d MMMM yy hh:mm:ss.zzz"
#define DATE_TIME_FORMAT_V6 "dd.MM.yyyy hh.mm"
#define DATE_TIME_FORMAT_V7 "yyyy-MM-dd hh:mm:ss"
#define DATE_TIME_FORMAT_V8 "dd.MM.yyyy hh.mm.ss"
#define DATE_BEFORE_YESTERDAY DATE_TODAY.addDays(-2) //Позавчера
#define DATE_YESTERDAY DATE_TODAY.addDays(-1) //Вчера
#define DATE_TODAY QDate::currentDate() //Сегодня
#define DATE_TOMORROW DATE_TODAY.addDays(1) //Завтра
#define DATE_AFTER_TOMORROW DATE_TODAY.addDays(2) //Послезавтра

//Asterisk Manager API
#define AMI_SUCCESSFUL_AUTH "SuccessfulAuth"
#define AMI_USER_EVENT "UserEvent"
#define AMI_DIAL_BEGIN "DialBegin"
#define AMI_DIAL_END "DialEnd"
#define AMI_HANGUP "Hangup"
#define AMI_BLIND_TRANSFER "BlindTransfer"
#define AMI_ORIGINATE_RESPONSE "OriginateResponse"
#define AMI_EXTENSION_STATUS "ExtensionStatus"

//FileExtensions
#define EXTENSION_DLL "dll"
#define EXTENSION_SO "so"
#define EXTENSION_XLS "xls"
#define EXTENSION_XLSX "xlsx"
#define EXTENSION_CSV "csv"
#define EXTENSION_TXT "txt"
#define EXTENSION_HTML "html"
#define EXTENSION_HTM "htm"
#define EXTENSION_DBF "dbf"
#define EXTENSION_PDF "pdf"
#define EXTENSION_LOG "log"
#define EXTENSION_PNG "png"
#define EXTENSION_JPEG "jpeg"
#define EXTENSION_JPG "jpg"
#define EXTENSION_DDS "dds"
#define EXTENSION_GIF "gif"
#define EXTENSION_ICNS "icns"
#define EXTENSION_ICO "ico"
#define EXTENSION_SVG "svg"
#define EXTENSION_TGA "tga"
#define EXTENSION_TIFF "tiff"
#define EXTENSION_WBMP "wbmp"
#define EXTENSION_BMP "bmp"
#define EXTENSION_WEBP "webp"
#define EXTENSION_LANG "lang"
#define EXTENSION_MONITOR "monitor"
#define EXTENSION_DMP "dmp"
#define EXTENSION_SQL "sql"
#define EXTENSION_WAV "wav"
#define EXTENSION_LIC "lic"
#define EXTENSION_XML "xml"
