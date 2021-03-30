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
const char EXTENSION_BINARY[] = ".exe"; //Расширение исполняемого файла для текущей операционной системы
const char EXTENSION_LIBRARY[] = ".dll"; //Расширение динамической библиотеки для текущей операционной системы
#else
const char EXTENSION_BINARY[] = ""; //Расширение исполняемого файла для текущей операционной системы
const char EXTENSION_LIBRARY[] = ".so"; //Расширение динамической библиотеки для текущей операционной системы
#endif
//LOCALIZATION-----------------------------------------------------------------
const char LOCALIZATION_FILE_INTEGRAL_SYSTEM[] = "IntegralSystem";
const char LOCALIZATION_FILE_OBJECTS[] = "Objects";
const char LOCALIZATION_FILE_CARAT[] = "Carat";
const char LOCALIZATION_FILE_CONFIGURATOR[] = "Configurator";
//REG_EXP----------------------------------------------------------------------
const char REG_EXP_UID[] = "UID=\"\\{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}\\}\""; //Уникальный идентфикатор из мета-данных
const char REG_EXP_LOGIN[] = "^([0-9A-Za-z._]+)$";
const char REG_EXP_URL[] = "^(http:\\/\\/www\\.|https:\\/\\/www\\.|http:\\/\\/|https:\\/\\/)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/.*)?$"; //Ссылка на сайт или страницу в интернете
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
const int ISPUSHBUTTON_MINIMUM_WIDTH = 98; //Минимальная ширина базовых кнопок
const int ISPUSHBUTTON_MINIMUM_HEIGHT = 28; //Минимальная высота базовых кнопок
const int SIZE_MINIMUM_HEIGHT_EDIT_FIELD = ISPUSHBUTTON_MINIMUM_HEIGHT; //Минимальная высота поля ввода ввода информации
//SYMBOLS----------------------------------------------------------------------
const char SYMBOL_SPACE = ' '; //Символ пробела
const char SYMBOL_MINUS = '-'; //Символ "Минус"
const char SYMBOL_COMMA = ','; //Символ "Запятая"
const char SYMBOL_POINT = '.'; //Символ "Точка"
const char SYMBOL_STAR = '*'; //Символ звездочка
const char SYMBOL_NULL_TERM = '\0'; //Символ конца строки
const char SYMBOL_SPECIAL_ARRAY[] = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
//DATABASE_CONNECTIONS---------------------------------------------------------
const char CONNECTION_DEFAULT[] = "DefaultDB";
const char CONNECTION_SYSTEM[] = "SystemDB";
const char CONNECTION_ASTERISK[] = "AsteriskDB";
const char CONNECTION_MONITOR[] = "MonitorDB";
//-----------------------------------------------------------------------------
const char CONFIG_TEMPLATE_SERVER[] = "Server"; //Щаблон конфигурационного файла для сервера
const char CONFIG_TEMPLATE_CLIENT[] = "Client"; //Щаблон конфигурационного файла для клиента
//SQL_DRIVERS------------------------------------------------------------------
const char SQL_DRIVER_QPSQL[] = "QPSQL";
//-----------------------------------------------------------------------------
const char			SYSTEM_DATABASE_NAME[] = "postgres"; //Имя системной БД
const char			SYSTEM_USER_LOGIN[] = "postgres"; //Логин системного пользователя
const char			SYSTEM_USER_UID[] = "f400ecfa-185e-4587-8961-8731deac3b5e"; //Идентификатор главного пользователя
const int			MAX_QUERY_TIME = 50; //Передельное время выполнения SQL-запроса (в миллисекундах)
const int			CALENDAR_AUDIO_TIMEOUT = 20000;
const char			CLASS_IS_LIST_EDIT[] = "ISListEdit";
const int			MINIMUM_PASSWORD_LENGHT = 8;
const int			WAIT_LOAD_DATA_LIST_FORM = 100;
const unsigned int	YEAR_MINIMUM = 1;
const unsigned int	YEAR_MAXIMUM = (unsigned int)SHRT_MAX;
const int			MAX_FILE_SIZE_FIELD = ((1000 * 1024) * 10); //Максимальный размер файла в поле редактирования файла (10 мегабайт)
const char			COLOR_STANDART[] = "#000000";
const char			NO_ERROR_STRING[] = "No error.";
const size_t		WORD_SIZE = 40; //Максимальный размер слова в словаре
const size_t		EDIT_BLINK_COUNT = (3) * 2; //Количество миганий поля редактирования. Где 3 - количество миганий.
const char			UUID_PLACEHOLDER_TEXT[] = "{00000000-0000-0000-0000-000000000000}";
const size_t		NPOS = std::string::npos;
const char			FILE_EDIT_PROPERTY_NAME[] = "Name";
const char			FILE_EDIT_PROPERTY_LOGO[] = "Logo";
const char			FILE_EDIT_PROPERTY_DATA[] = "Data";
const size_t		MAX_DECIMAL_LEN = 17; //Максимальное количество чесел после запятой в десятичной дроби (аналог DBL_DECIMAL_DIG)
const size_t		MAX_INTEGER_LEN = 19; //Максимальная "длина" целого числа
const char			SYMBOL_OBJECT_CHANGED[] = " {*}";
const size_t		AUDIO_PLAYER_POS = 5000; //Перемотка в аудио-плеере
const char			LOCAL_HOST_ADDRESS[] = "127.0.0.1";
const char			LINK_FEEDBACK[] = "https://forms.yandex.ru/u/602e2ac1b129012bf81424fb/";
const qint64		DAY_IN_SECONDS = 86400;
const size_t		UUID_STANDART_SIZE = 36; //Размер стандартного размера
//LOGGER-----------------------------------------------------------------------
const size_t	LOGGER_ARRAY_SIZE = 20000; //Максимальный размер массива
const size_t	LOGGER_TIMEOUT = 50; //Таймаут логгера
const size_t	LOGGER_MESSAGE_SIZE = 4098; //Максимальный размер строки логгера
//CARAT------------------------------------------------------------------------
const unsigned short	CARAT_TCP_PORT = 50000; //Стандартный порт TCP-сервера
const unsigned short	CARAT_CONTROLLER_PORT = 50001; //Стандартный порт контроллера
const size_t			CARAT_HASH_SIZE = 64; //Фиксированный размер хэша SHA256 для авторизации
const unsigned long     CARAT_SALT_SIZE = 32; //фиксированный размер соли
const size_t			CARAT_CONNECT_TIMEOUT = 3000; //Время ожидания подключения к Карату
const size_t			CARAT_CONNECT_SLEEP = 10; //Время паузы для ожидания подключения
const size_t			CARAT_RECONNECT_TIMEOUT = 5000; //Время паузы для попытки подключиться к серверу
const size_t			CARAT_BAN_ATTEMPT_COUNT = 5; //Количество попыток подключения
//ASTERISK---------------------------------------------------------------------
const char              ASTERISK_AMI_SEPARATOR[] = "\r\n\r\n"; //Разделитель пакетов AMI
const unsigned short    ASTERISK_AMI_PORT = 5038; //Порт AMI по умолчанию
//CARAT_API--------------------------------------------------------------------
const char API_UNKNOWN[] = "Unknown"; //Неизвестное сообщение
const char API_AUTH[] = "Auth"; //Авторизация
const char API_SLEEP[] = "Sleep"; //Ожидание
const char API_GET_META_DATA[] = "GetMetaData"; //Получение мета-данных
const char API_GET_LAST_CLIENT[] = "GetLastClient"; //Получить последнюю версию клиента
const char API_USER_PASSWORD_EXIST[] = "UserPasswordExist"; //Проверить существование пароля
const char API_USER_PASSWORD_CREATE[] = "UserPasswordCreate"; //Создание пароля пользователю
const char API_USER_PASSWORD_EDIT[] = "UserPasswordEdit"; //Изменение пароля пользователя
const char API_USER_PASSWORD_RESET[] = "UserPasswordReset"; //Сброс пароля пользователя
const char API_USER_SETTINGS_RESET[] = "UserSettingsReset"; //Сброс пользовательских настроек
const char API_GET_RECORD_CALL[] = "GetRecordCall"; //Получить запись разговора
const char API_GET_CLIENTS[] = "GetClients"; //Получить список подключенных клиентов
const char API_RECORD_ADD[] = "RecordAdd"; //Добавить запись
const char API_RECORD_EDIT[] = "RecordEdit"; //Добавить запись
const char API_RECORD_DELETE[] = "RecordDelete"; //Удалить запись
const char API_RECORD_GET[] = "RecordGet"; //Удалить запись
const char API_RECORD_GET_INFO[] = "RecordGetInfo"; //Получить информацию о записи
const char API_DISCUSSION_ADD[] = "DiscussionAdd"; //Добавить обсуждение
const char API_DISCUSSION_EDIT[] = "DiscussionEdit"; //Изменить обсуждение
const char API_DISCUSSION_COPY[] = "DiscussionCopy"; //Скопировать обсуждение
const char API_GET_TABLE_DATA[] = "GetTableData"; //Получить данные из таблицы
const char API_GET_TABLE_QUERY[] = "GetTableQuery"; //Получить данные по запросу
const char API_NOTE_RECORD_GET[] = "GetNoteRecord"; //Получить примечание записи
const char API_NOTE_RECORD_SET[] = "SetNoteRecord"; //Изменить примечание записи
const char API_FILE_STORAGE_ADD[] = "FileStorageAdd"; //Добавить файл в хранилище
const char API_FILE_STORAGE_COPY[] = "FileStorageCopy"; //Скопировать файл в хранилище
const char API_FILE_STORAGE_GET[] = "FileStorageGet"; //Получить файл из хранилища
const char API_SEARCH_TASK_TEXT[] = "SearchTaskText"; //Поиск задач по тексту
const char API_SEARCH_TASK_ID[] = "SearchTaskID"; //Поиск задач по номеру
const char API_SEARCH_FULL_TEXT[] = "SearchFullText"; //Полтекстовый поиск
const char API_GET_CALENDAR_EVENTS[] = "GetCalendarEvents"; //Получить события календаря
const char API_CALENDAR_DELETE[] = "CalendarDelete"; //Удаление события календаря
const char API_GET_INTERNAL_LISTS[] = "GetInternalLists"; //Получить список внутренних справочников
const char API_SAVE_META_DATA[] = "SaveMetaData"; //Сохранение мета-данных
const char API_GET_GROUP_RIGHTS[] = "GetGroupRights"; //Получить права группы
const char API_GROUP_RIGHT_SUBSYSTEM_ADD[] = "GroupRightSubSystemAdd"; //Добавить группу право на подсистему
const char API_GROUP_RIGHT_SUBSYSTEM_DELETE[] = "GroupRightSubSystemDelete"; //Удалить у группы право на подсистему
const char API_GROUP_RIGHT_TABLE_ADD[] = "GroupRightTableAdd"; //Добавить группе право на таблицу
const char API_GROUP_RIGHT_TABLE_DELETE[] = "GroupRightTableDelete"; //Удалить у группы право на таблицу
const char API_GROUP_RIGHT_SPECIAL_ADD[] = "GroupRightSpecialAdd"; //Добавить спец. право группе
const char API_GROUP_RIGHT_SPECIAL_DELETE[] = "GroupRightSpecialDelete"; //Удалить спец. право у группы
const char API_GET_RECORD_VALUE[] = "GetRecordValue"; //Получить значение поля записи
const char API_RECORD_FAVORITE_ADD[] = "RecordFavoriteAdd"; //Добавить запись в избранное
const char API_RECORD_FAVORITE_DELETE[] = "RecordFavoriteDelete"; //Удалить запись из избранного
const char API_GET_FAVORITE_NAMES[] = "GetFavoriteNames"; //Получить имена избранных записей
const char API_FAVORITES_DELETE[] = "FavoritesDelete"; //Удалить все избранные записи пользователя
const char API_LOG_GET_STRUCTURE[] = "LogGetStructure"; //Получить структуру лог-файлов
const char API_LOG_GET_CONTENT[] = "LogGetContent"; //Получить содержимое лог-файла
const char API_CALENDAR_CLOSE[] = "CalendarClose"; //Закрытие события календаря
const char API_GET_HISTORY_LIST[] = "GetHistoryList"; //Получить список открытых записей
const char API_TASK_COMMENT_ADD[] = "TaskCommentAdd"; //Добавить комментарий к задаче
const char API_GET_FOREIGN_LIST[] = "GetForeignList"; //Получить список значений по внешнему ключу
const char API_GET_SERVER_INFO[] = "GetServerInfo"; //Получить информацию о сервере
const char API_ORGANIZATION_FROM_INN[] = "OrganizationFromINN"; //Поиск организации по ИНН
const char API_STATEMENTS_QUERY_GET[] = "StatementsQueryGet"; //Получить статистику запросов
const char API_STATEMENTS_QUERY_RESET[] = "StatementsQueryReset"; //Очистить статистику запросов
const char API_GET_MONITOR[] = "GetMonitor"; //Получить метрики мониторинга
//-----------------------------------------------------------------------------
const char CONST_UID_SETTINGS_DATABASE[] = "1de3449c-c700-4efc-a455-e99e38063d9b"; //Идентификатор настроек базы данных
//-----------------------------------------------------------------------------
const char CONST_UID_PARAGRAPH_DESKTOP[] = "9a36ad64-5a2f-4b62-9da7-4fc545b6ab5f"; //Рабочий стол
const char CONST_UID_PARAGRAPH_WORKSPACE[] = "18a90f8b-1d99-420e-ba60-c3ebf60d595e"; //Рабочая область
const char CONST_UID_PARAGRAPH_CALENDAR[] = "1ba7f467-32aa-4719-b8f0-bcd8bc678cab"; //Календарь
const char CONST_UID_PARAGRAPH_TASKS[] = "588a7dba-cc77-4ea2-9691-7f800c629d7f"; //Задачи
//-----------------------------------------------------------------------------
const char CONST_UID_PROTOCOL_ENTER_APPLICATION[] = "2614CB96-EC54-4D4F-B1A6-06D2444962CA";
const char CONST_UID_PROTOCOL_EXIT_APPLICATION[] = "e9461667-e1ca-4c22-907b-fdc350a931c4";
const char CONST_UID_PROTOCOL_CREATE_OBJECT[] = "d1348312-298f-4a7c-b584-9ba8c4952cd3";
const char CONST_UID_PROTOCOL_CREATE_COPY_OBJECT[] = "efa8fe45-1174-4d2e-bbe6-4940380961d4";
const char CONST_UID_PROTOCOL_EDIT_OBJECT[] = "0361643d-0a62-4f51-84bd-313f53115efd";
const char CONST_UID_PROTOCOL_SHOW_OBJECT[] = "117e8972-97dc-4e72-93ac-dc3bb50d11cf";
const char CONST_UID_PROTOCOL_DELETE_OBJECT[] = "b6eefb63-8197-4f3a-8373-5c3b2fc409f3";
const char CONST_UID_PROTOCOL_RECORD_INFO[] = "fd98685e-d6dd-41e3-9f62-389567f58971";
const char CONST_UID_PROTOCOL_EXPORT_TABLE[] = "1c50a667-b1c4-4f2c-8f39-90106abb6774";
const char CONST_UID_PROTOCOL_SEARCH[] = "179f73bf-8983-4b52-b505-af0ef4bf8093";
const char CONST_UID_PROTOCOL_SEARCH_FULL_TEXT[] = "b0a1ef68-2aa4-4eb2-a5a9-f8e5604f864b";
const char CONST_UID_PROTOCOL_NOTE_RECORD_SHOW[] = "0a6320b0-827b-4043-886a-1d7fd5d9b6de";
const char CONST_UID_PROTOCOL_NOTE_RECORD_ADD[] = "662105b6-6671-46be-8e1a-e57061c5c268";
const char CONST_UID_PROTOCOL_NOTE_RECORD_EDIT[] = "c20634af-ef58-4d96-9215-8085c2b8c416";
const char CONST_UID_PROTOCOL_NOTE_RECORD_DELETE[] = "08904df2-3785-4454-b5e5-2d83a9faa231";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM[] = "ea765447-6811-43ad-8cfc-9b2e758a5b4e";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM[] = "5e5ec969-0d11-41fb-9652-7e2add466782";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE[] = "44fdda11-9f7b-4006-8178-944da65fb7aa";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE[] = "9d4e8104-33f9-4f9e-a6aa-272dda7c1a52";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL[] = "fb494e8e-bdf3-4804-af42-43487af8a871";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL[] = "4e7c34e5-c7fe-4c0d-bb4d-7b4a1d4cf9d2";
const char CONST_UID_PROTOCOL_USER_PASSWORD_CREATE[] = "6dd00b13-2a69-45fd-b565-3142c2d2ba97";
const char CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE[] = "0eee6d67-a0f3-4558-b7ad-173678a8c9dc";
const char CONST_UID_PROTOCOL_USER_PASSWORD_RESET[] = "3b005c51-b44e-48c6-95f8-3ac6bd913bb3";
const char CONST_UID_PROTOCOL_USER_SETTINGS_RESET[] = "075e58a5-5336-4a71-8d1d-91202173f4d4";
const char CONST_UID_PROTOCOL_FILE_STORAGE_ADD[] = "7e9ff116-cf0a-4568-b048-7fab8d773d57";
const char CONST_UID_PROTOCOL_FILE_STORAGE_COPY[] = "5cb370b6-d1aa-4e40-83a9-aeac7fea65e2";
const char CONST_UID_PROTOCOL_FILE_STORAGE_SAVE[] = "e73bedc4-79ff-4b94-acc9-2692e1927d18";
const char CONST_UID_PROTOCOL_TASK_SEARCH_TEXT[] = "e4698e8f-c1f4-4f04-a4b6-5cd8805b7fac";
const char CONST_UID_PROTOCOL_TASK_SEARCH_ID[] = "4dfd26f5-799f-4e34-8dbd-78e52f01423d";
const char CONST_UID_PROTOCOL_CALENDAR_DELETE[] = "d9372e46-8db0-41b4-80c0-6aec2cc6cd17";
const char CONST_UID_PROTOCOL_GET_UPDATE_CLIENT[] = "0234643a-af60-44bf-a2eb-08a71307841c";
const char CONST_UID_PROTOCOL_RECORD_FAVORITE_ADD[] = "8d08a9f9-fc50-4986-8d92-fec0d7d36da0";
const char CONST_UID_PROTOCOL_RECORD_FAVORITE_DELETE[] = "af2641f0-6838-441c-a0a9-b62090826998";
const char CONST_UID_PROTOCOL_CALENDAR_CLOSE[] = "f05b1de1-47f6-43db-a8c5-5d3582f7bd5f";
const char CONST_UID_PROTOCOL_GET_TABLE_DATA[] = "2a93a14e-4ba5-4cb8-a056-0ff2b3351c5d";
const char CONST_UID_PROTOCOL_SERVER_INFO[] = "e6f5d167-25e6-4172-8d81-219de8608647";
const char CONST_UID_PROTOCOL_STATEMENTS_QUERY_RESET[] = "3660e1be-755a-4034-89d8-15fe83776122";
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_TYPE_SHOW[] = "41743776-d7ce-414f-acf3-71c42a607742"; //Просмотр данных
const char CONST_UID_GROUP_ACCESS_TYPE_CREATE[] = "1cfa961d-08a2-4aa3-978e-ff23fc88ca08"; //Создание данных
const char CONST_UID_GROUP_ACCESS_TYPE_EDIT[] = "259ac6d5-63f8-4c44-b8a6-0108da542ba6"; //Изменение данных
const char CONST_UID_GROUP_ACCESS_TYPE_DELETE[] = "48e0575c-31a0-4ae1-87d7-6450b69a41a5"; //Удаление данных
const char CONST_UID_GROUP_ACCESS_TYPE_EXPORT[] = "e3db5851-69a2-4d32-b4f4-0eb98c331b22"; //Экспорт данных
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP[] = "6948884f-5f69-4f74-a230-a2d892a44a52"; //Рабочий стол
const char CONST_UID_GROUP_ACCESS_SPECIAL_SETTINGS_DB[] = "e006a41a-465f-41ab-ba5d-2cbc41935b2c"; //Рабочий стол
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_CLOSE[] = "4fa7688b-e0b8-4018-b6ec-fd4be898c90b"; //Закрытие задач
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_REOPEN[] = "d0a6aa06-ff19-4945-9441-aea245ae32df"; //Переоткрытие задач
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_EDIT[] = "988106be-292a-428a-a10d-5858c4dfe569"; //Редактирование задач
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_TRANSFORMATION[] = "d7837403-15ba-4b50-8b9a-660602c24b98"; //Преобразование задач в подзадач и наоборот
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_APPOINT[] = "14b3e83d-56d4-4527-9c33-0770d38b78d5"; //Назначение исполнителя
const char CONST_UID_GROUP_ACCESS_SPECIAL_TASK_SHOW_ALL[] = "093e482f-c108-4939-a1d5-66377ba354ac"; //Просмотр всех задач
//-----------------------------------------------------------------------------
const char CONST_UID_TASK_PRIORITY_LOW[] = "cd620874-5069-48e4-bdea-2d5aebf8aba7"; //Низкий
const char CONST_UID_TASK_PRIORITY_AVERAGE[] = "87ac0b68-9348-4a39-8466-b84dc641fe8b"; //Средний
const char CONST_UID_TASK_PRIORITY_HIGH[] = "5b4c2bdb-758b-49f3-b172-7351aaa51c58"; //Высокий
const char CONST_UID_TASK_PRIORITY_CRITICAL[] = "ced1b019-c1cd-4044-a9b1-2a97273143f4"; //Критический
//-----------------------------------------------------------------------------
const char CONST_UID_TASK_STATUS_OPEN[] = "2fd27958-6ec9-4e78-88f4-0a2bfdfe4716"; //Открыта
const char CONST_UID_TASK_STATUS_IN_WORK[] = "72beb1a3-0491-4932-aaa0-5725da41a0a8"; //В работе
const char CONST_UID_TASK_STATUS_DONE[] = "20f3d6a5-1a90-4228-9c39-8f05dfded9fe"; //Выполнена
const char CONST_UID_TASK_STATUS_NOT_DONE[] = "d3f65984-117e-4afe-81e0-1198533767f8"; //Не будет выполнена
const char CONST_UID_TASK_STATUS_CLOSE[] = "1617ec4f-50f6-4912-8143-4d702e1f5f8f"; //Закрыта
//-----------------------------------------------------------------------------
const char CONST_UID_DATABASE_SETTING_GENERAL_ACCESSDATABASE[] = "UserAccessDatabase";
const char CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA[] = "NumberSimbolsAfterComma";
const char CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE[] = "StorageFileMaxSize";
const char CONST_UID_DATABASE_SETTING_OTHER_TCPMESSAGEID[] = "TCPMessageID";
//-----------------------------------------------------------------------------
const char CONST_UID_SETTING_GROUP_GENERAL[] = "152517c5-b081-42e5-98e4-0ba102ef7ff8";
const char CONST_UID_SETTING_GROUP_VIEW[] = "3aecbf8a-531f-4484-b507-1269a10b5f2f";
const char CONST_UID_SETTING_GROUP_TABLES[] = "12b776e9-ce2e-44a8-855d-67edb4e58cd2";
const char CONST_UID_SETTING_TABLES_MINIMIZEHEIGHTROWS[] = "77ba2566-d78d-467a-92ce-dfd987f21c03";
const char CONST_UID_SETTING_TABLES_DOUBLECLICKEVENT[] = "e0b5eed0-947e-408e-9666-944cc31c5d1a";
const char CONST_UID_SETTING_TABLES_SHOWHORIZONTALHEADER[] = "22c5ff3b-83de-4c8d-bc2b-8a456c8c57d4";
const char CONST_UID_SETTING_TABLES_SHOWVERTICALHEADER[] = "6d14f262-b98e-4fbd-b9cc-a1a8e7b52d9f";
const char CONST_UID_SETTING_TABLES_SHOWCOUNTRECORD[] = "efd021b1-5267-4dfa-8d11-a4ff4a419536";
const char CONST_UID_SETTING_TABLES_SEARCH_FAST_VIEW[] = "a07aa634-f553-4c05-8eec-cc8a5ae7dc18";
const char CONST_UID_SETTING_TABLE_VISIBLE_FIELD_ID[] = "2876f177-b30f-41a0-8643-f2bca583726f";
const char CONST_UID_SETTING_GROUP_TABS[] = "c42701d0-3404-42c6-82d9-41ec50f2e556";
const char CONST_UID_SETTING_TABS_SELECTIONBEHAVIOR[] = "8c96bf79-a7fb-4562-abab-a9c520b4b68d";
const char CONST_UID_SETTING_GROUP_SOUND[] = "8460db7a-90fb-459e-8343-7a67529414c0";
const char CONST_UID_SETTING_SOUND_VOLUME[] = "203f8364-b041-4c20-a7ef-e1fba8cdb68b";
const char CONST_UID_SETTING_SOUND_CALENDAREVENT[] = "70680eed-160d-45e9-8f79-d6cbde8aa875";
const char CONST_UID_SETTING_GROUP_CALENDAR[] = "65f8b333-5c21-48a2-8f30-d947c36ef51b";
const char CONST_UID_SETTING_CALENDAR_NOTE_VIEW_TYPE[] = "b03fbac5-f793-4565-92a3-821c87ae3b56";
const char CONST_UID_SETTING_GROUP_TASK_TRACKER[] = "65f8b333-5c21-48a2-8f30-d947c36ef51b";
const char CONST_UID_SETTING_TASK_TRACKER_SHOW_TASK_AFTER_CREATE[] = "094cb33e-13b9-4e5c-87dc-79ae69cbe533";
const char CONST_UID_SETTING_GROUP_OTHER[] = "0baef651-d749-4566-b682-f11d03c6d849";
//-----------------------------------------------------------------------------
const char CONST_CONFIG_CONNECTION_SERVER[] = "Connection/Host";
const char CONST_CONFIG_CONNECTION_PORT[] = "Connection/Port";
const char CONST_CONFIG_CONNECTION_DATABASE[] = "Connection/Database";
const char CONST_CONFIG_CONNECTION_LOGIN[] = "Connection/Login";
const char CONST_CONFIG_CONNECTION_PASSWORD[] = "Connection/Password";
const char CONST_CONFIG_TCPSERVER_INCLUDE[] = "TCPServer/Include";
const char CONST_CONFIG_TCPSERVER_PORT[] = "TCPServer/Port";
const char CONST_CONFIG_TCPSERVER_WORKERS[] = "TCPServer/WorkerCount";
const char CONST_CONFIG_AMI_INCLUDE[] = "AMI/Include";
const char CONST_CONFIG_AMI_SERVER[] = "AMI/Host";
const char CONST_CONFIG_AMI_PORT[] = "AMI/Port";
const char CONST_CONFIG_AMI_LOGIN[] = "AMI/Login";
const char CONST_CONFIG_AMI_PASSWORD[] = "AMI/Password";
const char CONST_CONFIG_AMI_RECORD_DIR[] = "AMI/RecordDir";
const char CONST_CONFIG_MONITOR_INCLUDE[] = "Monitor/Include";
const char CONST_CONFIG_MONITOR_TIMEOUT[] = "Monitor/Timeout";
const char CONST_CONFIG_OTHER_UPDATE_CLIENT_DIR[] = "Other/UpdateClientDir";
const char CONST_CONFIG_OTHER_CONFIGURATION[] = "Other/Configuration";
//-----------------------------------------------------------------------------
#endif
