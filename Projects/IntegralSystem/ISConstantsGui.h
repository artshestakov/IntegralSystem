#pragma once
#ifndef _ISCONSTANTSGUI_H_INCLUDED
#define _ISCONSTANTSGUI_H_INCLUDED
//-----------------------------------------------------------------------------
#include <QtCore/QSize>
#include <QtGui/QColor>
#include <QtGui/QFont>
//-----------------------------------------------------------------------------
const QSize   SIZE_MAIN_WINDOW_MINIMUM = QSize(700, 500);
//-----------------------------------------------------------------------------
const QColor  COLOR_MAIN_MENU_BAR = QColor(81, 145, 189);
const QColor  COLOR_CALENDAR_SELECTED_DATE_BACKGROUND = QColor(215, 229, 240);
//-----------------------------------------------------------------------------
const QFont   FONT_TAHOMA_12_BOLD = QFont("Tahoma", 12, QFont::Bold);
const QFont   FONT_TAHOMA_10 = QFont("Tahoma", 10);
const QFont   FONT_TAHOMA_9 = QFont("Tahoma", 9);
const QFont   FONT_TAHOMA_9_BOLD = QFont("Tahoma", 9, QFont::Bold);
const QFont   FONT_APPLICATION = FONT_TAHOMA_9;
const QFont   FONT_APPLICATION_BOLD = FONT_TAHOMA_9_BOLD;
//-----------------------------------------------------------------------------
const Qt::CursorShape CURSOR_ARROW = Qt::ArrowCursor;
const Qt::CursorShape CURSOR_POINTING_HAND = Qt::PointingHandCursor;
const Qt::CursorShape CURSOR_WHATS_THIS = Qt::WhatsThisCursor;
const Qt::CursorShape CURSOR_WAIT = Qt::WaitCursor;
const Qt::CursorShape CURSOR_BUSY = Qt::BusyCursor;
//-----------------------------------------------------------------------------
const char SYMBOL_SPACE = ' '; //Символ пробела
const char SYMBOL_COMMA = ','; //Символ "Запятая"
const char SYMBOL_POINT = '.'; //Символ "Точка"
const char SYMBOL_STAR = '*'; //Символ звездочка
const char SYMBOL_NULL_TERM = '\0'; //Символ конца строки
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
const char REG_EXP_UID[] = "UID=\"\\{\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12}\\}\""; //Уникальный идентфикатор из мета-данных
const char REG_EXP_LOGIN[] = "^([0-9A-Za-z._]+)$";
const char REG_EXP_URL[] = "^(http:\\/\\/www\\.|https:\\/\\/www\\.|http:\\/\\/|https:\\/\\/)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?(\\/.*)?$"; //Ссылка на сайт или страницу в интернете
//-----------------------------------------------------------------------------
const size_t CARAT_CONNECT_TIMEOUT = 3000; //Время ожидания подключения к Карату
const size_t CARAT_CONNECT_SLEEP = 10; //Время паузы для ожидания подключения
const size_t CARAT_RECONNECT_TIMEOUT = 5000; //Время паузы для попытки подключиться к серверу
//-----------------------------------------------------------------------------
const int EDIT_FIELD_SPACING = 4;
const int ISPUSHBUTTON_MINIMUM_WIDTH = 98; //Минимальная ширина базовых кнопок
const int ISPUSHBUTTON_MINIMUM_HEIGHT = 28; //Минимальная высота базовых кнопок
const int SIZE_MINIMUM_HEIGHT_EDIT_FIELD = ISPUSHBUTTON_MINIMUM_HEIGHT; //Минимальная высота поля ввода ввода информации
//-----------------------------------------------------------------------------
const char			SYSTEM_USER_LOGIN[] = "postgres"; //Логин системного пользователя
const char			CLASS_IS_LIST_EDIT[] = "ISListEdit";
const int			WAIT_LOAD_DATA_LIST_FORM = 100;
const unsigned int	YEAR_MINIMUM = 1;
const unsigned int	YEAR_MAXIMUM = (unsigned int)SHRT_MAX;
const char			COLOR_STANDART[] = "#000000";
const char			NO_ERROR_STRING[] = "No error.";
const size_t		EDIT_BLINK_COUNT = (3) * 2; //Количество миганий поля редактирования. Где 3 - количество миганий.
const char			UUID_PLACEHOLDER_TEXT[] = "{00000000-0000-0000-0000-000000000000}";
const char			FILE_EDIT_PROPERTY_NAME[] = "Name";
const char			FILE_EDIT_PROPERTY_LOGO[] = "Logo";
const char			FILE_EDIT_PROPERTY_DATA[] = "Data";
const size_t		MAX_DECIMAL_LEN = 17; //Максимальное количество чесел после запятой в десятичной дроби (аналог DBL_DECIMAL_DIG)
const size_t		MAX_INTEGER_LEN = 19; //Максимальная "длина" целого числа
const char			SYMBOL_OBJECT_CHANGED[] = " {*}";
const size_t		AUDIO_PLAYER_POS = 5000; //Перемотка в аудио-плеере
const char			LINK_FEEDBACK[] = "https://forms.yandex.ru/u/602e2ac1b129012bf81424fb/";
//-----------------------------------------------------------------------------
const char CONST_UID_PARAGRAPH_DESKTOP[] = "9a36ad64-5a2f-4b62-9da7-4fc545b6ab5f"; //Рабочий стол
const char CONST_UID_PARAGRAPH_WORKSPACE[] = "18a90f8b-1d99-420e-ba60-c3ebf60d595e"; //Рабочая область
const char CONST_UID_PARAGRAPH_CALENDAR[] = "1ba7f467-32aa-4719-b8f0-bcd8bc678cab"; //Календарь
const char CONST_UID_PARAGRAPH_TASKS[] = "588a7dba-cc77-4ea2-9691-7f800c629d7f"; //Задачи
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_SPECIAL_DESKTOP[] = "6948884f-5f69-4f74-a230-a2d892a44a52"; //Рабочий стол
const char CONST_UID_GROUP_ACCESS_SPECIAL_SETTINGS_DB[] = "e006a41a-465f-41ab-ba5d-2cbc41935b2c"; //Рабочий стол
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
const char LOCALIZATION_FILE_INTEGRAL_SYSTEM[] = "IntegralSystem";
const char LOCALIZATION_FILE_OBJECTS[] = "Objects";
//-----------------------------------------------------------------------------
const char FORMAT_DATE_V1[] = "d MMMM yyyy";					//30 Июль 2020
const char FORMAT_DATE_V2[] = "dd.MM.yyyy";					//30.07.2020
const char FORMAT_DATE_V3[] = "dd/MM/yyyy";					//30/07/2020
const char FORMAT_DATE_V4[] = "d MMMM yyyy (ddd)";			//30 Июль 2020 (Чт)
const char FORMAT_TIME_V1[] = "hh:mm";						//10:16
const char FORMAT_TIME_V2[] = "hh.mm";						//10.16
const char FORMAT_TIME_V3[] = "hh:mm:ss";						//10:16:13
const char FORMAT_TIME_V4[] = "mm:ss";						//16:13
const char FORMAT_TIME_V5[] = "hh.mm.ss";						//10.16.13
const char FORMAT_DATE_TIME_V1[] = "d MMMM yyyy hh:mm";			//4 Август 2020 12:59
const char FORMAT_DATE_TIME_V2[] = "dd.MM.yyyy hh:mm";				//04.08.2020 12:59
const char FORMAT_DATE_TIME_V3[] = "d MMMM yyyy hh:mm:ss";			//4 Август 2020 12:59:52
const char FORMAT_DATE_TIME_V4[] = "dd.MM.yyyy hh:mm:ss.zzz";		//04.08.2020 12:59:52.459
const char FORMAT_DATE_TIME_V5[] = "d MMMM yy hh:mm:ss.zzz";		//4 Август 20 12:59:52.459
const char FORMAT_DATE_TIME_V6[] = "dd.MM.yyyy hh.mm";				//04.08.2020 12.59
const char FORMAT_DATE_TIME_V7[] = "yyyy-MM-dd hh:mm:ss";			//2020-08-04 12:59:52
const char FORMAT_DATE_TIME_V8[] = "dd.MM.yyyy_hh.mm.ss";			//04.08.2020_12.59.52
const char FORMAT_DATE_TIME_V9[] = "dd.MM.yy hh:mm:ss.zzz";		//04.08.20 12:59:52.459
const char FORMAT_DATE_TIME_V10[] = "d MMMM yyyy (ddd) hh:mm:ss";	//4 Август (Вт) 2020 12:59:52
//-----------------------------------------------------------------------------
const char PATH_SCHEMA_TEMPLATE_FIELDS[] = ":Other/ClassTemplateFields.xml";
//-----------------------------------------------------------------------------
#endif
