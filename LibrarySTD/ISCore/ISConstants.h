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
const int SOCKET_ERROR = -1;
#endif
//-----------------------------------------------------------------------------
const size_t    NPOS = std::string::npos;
const char      STRING_NO_ERROR[] = "No error.";
const char      STRING_UNKNOWN_ERROR[] = "Unknown error.";
const char      CONST_UID_SETTINGS_DATABASE[] = "1de3449c-c700-4efc-a455-e99e38063d9b"; //Идентификатор настроек базы данных
const size_t    STRING_FORMAT_BUFFER_SIZE = 4096;
const size_t    UUID_STANDART_SIZE = 36; //Размер стандартного UID
const size_t    MD5_SIZE = 16;
const size_t    MD5_RESULT_SIZE = 32;
const char      MD5_DIGITS[] = "0123456789abcdef";
const size_t    DAY_IN_SECONDS = 86400;
const char      TOKEN_DA_DATA_TOKEN[] = "f2c204cd6ba093b294324acec5d01b25fad632cf";
//CHARS------------------------------------------------------------------------
const char  CHAR_NULL_TERM = '\0'; //Символ конца строки
//LOCALIZATION-----------------------------------------------------------------
const char  LOCALIZATION_FILE_CARAT[] = "Carat";
//DATABASE---------------------------------------------------------------------
const long long         DB_MAX_QUERY_TIME = 50; //Максимальное время (в милисекундах) выполнения SQL-запроса
const char              DB_CONNECTION_DEFAULT[] = "DefaultDB";
//CONFIG-----------------------------------------------------------------------
const char  CONFIG_TEMPLATE_SERVER[] = "Server"; //Шаблон сервера
const char  CONFIG_TEMPLATE_CLIENT[] = "Client"; //Шаблон клиента
const char  CONFIG_TYPE_INT[] = "Int";
const char  CONFIG_TYPE_BOOL[] = "Bool";
const char  CONFIG_TYPE_STRING[] = "String";
//LOGGER-----------------------------------------------------------------------
const size_t    LOG_ARRAY_SIZE = 20000; //Максимальный размер массива
const size_t    LOG_TIMEOUT = 50; //Таймаут логгера
const size_t    LOG_HEADER_SIZE = 96; //Макисмальный размер заголовка
const size_t    LOG_BUFFER_SIZE = 10240; //Размер буфера для одного сообщения
const char      LOG_UNKNOWN[] = "Unknown";
const char      LOG_DEBUG[] = "Debug";
const char      LOG_INFO[] = "Info";
const char      LOG_WARNING[] = "Warning";
const char      LOG_ERROR[] = "Error";
const char      LOG_CRITICAL[] = "Critical";
const char      LOG_TRACE[] = "Trace";
const char      LOG_ASSERT[] = "Assert";
//ASTERISK---------------------------------------------------------------------
const size_t    TCP_PACKET_MAX_SIZE = 65535; //Максимальный размер одного TCP-пакета
//CARAT------------------------------------------------------------------------
const unsigned short    CARAT_TCP_PORT = 50000; //Стандартный порт TCP-сервера
const size_t            CARAT_HASH_SIZE = 64; //Фиксированный размер хэша SHA256 для авторизации
const size_t            CARAT_WAIT_WORKERS_MSEC = 1000; //Ожидание освобождения воркеров в милисекундах
const unsigned long     CARAT_SALT_SIZE = 32; //фиксированный размер соли
//CARAT_API--------------------------------------------------------------------
const char API_UNKNOWN[] = "Unknown"; //Неизвестное сообщение
const char API_AUTH[] = "Auth"; //Авторизация
const char API_SLEEP[] = "Sleep"; //Ожидание
const char API_GET_META_DATA[] = "GetMetaData"; //Получение мета-данных
const char API_GET_LAST_CLIENT[] = "GetLastClient"; //Получение обновления
const char API_USER_PASSWORD_EXIST[] = "UserPasswordExist"; //Проверка наличия пароля пользователя
const char API_USER_PASSWORD_CREATE[] = "UserPasswordCreate"; //Создание пароля пользователя
const char API_USER_PASSWORD_EDIT[] = "UserPasswordEdit"; //Изменение пароля пользователя
const char API_USER_PASSWORD_RESET[] = "UserPasswordReset"; //Сброс пароля пользователя
const char API_USER_SETTINGS_RESET[] = "UserSettingsReset"; //Сброс пароля пользователя
const char API_GET_TABLE_DATA[] = "GetTableData"; //Получение данных из таблицы
const char API_RECORD_GET_INFO[] = "RecordGetInfo"; //Получение информации о записи
const char API_GET_SERVER_INFO[] = "GetServerInfo"; //Получение информации о сервере
const char API_GET_RECORD_VALUE[] = "GetRecordValue"; //Получение значения записи
const char API_RECORD_ADD[] = "RecordAdd"; //Добавление новой записи
const char API_RECORD_GET[] = "RecordGet"; //Получение записи из БД
const char API_RECORD_EDIT[] = "RecordEdit"; //Изменение записи в БД
const char API_RECORD_DELETE[] = "RecordDelete"; //Удаление записи в БД
const char API_FILE_STORAGE_ADD[] = "FileStorageAdd"; //Добавление файла в хранилище
const char API_FILE_STORAGE_COPY[] = "FileStorageCopy"; //Создание копии файла в хранилище
const char API_FILE_STORAGE_GET[] = "FileStorageGet"; //Получение файла из хранилища
const char API_DISCUSSION_ADD[] = "DiscussionAdd"; //Добавление сообщения в обсуждение
const char API_DISCUSSION_EDIT[] = "DiscussionEdit"; //Изменение сообщения в обсуждении
const char API_DISCUSSION_COPY[] = "DiscussionCopy"; //Создание копии сообщения в обсуждении
const char API_GET_NOTE_RECORD[] = "GetNoteRecord"; //Получить примечание записи
const char API_SET_NOTE_RECORD[] = "SetNoteRecord"; //Изменить примечание записи
const char API_SEARCH_FULL_TEXT[] = "SearchFullText"; //Полнотекстовый поиск
const char API_GET_INTERNAL_LISTS[] = "GetInternalLists"; //Получение списка внутренних справочников
const char API_GET_FAVORITE_NAMES[] = "GetFavoriteNames"; //Получить локальные имена избранных объектов
const char API_RECORD_FAVORITE_ADD[] = "RecordFavoriteAdd"; //Добавить запись в избранное
const char API_RECORD_FAVORITE_DELETE[] = "RecordFavoriteDelete"; //Удалить запись из избранного
const char API_FAVORITES_DELETE[] = "FavoritesDelete"; //Удалить все избранные записи
const char API_GET_HISTORY_LIST[] = "GetHistoryList"; //Получить историю открытых объектов
const char API_GET_FOREIGN_LIST[] = "GetForeignList"; //Получить список значений по внешнему ключу
const char API_GET_GROUP_RIGHTS[] = "GetGroupRights"; //Получить права доступа группы
const char API_ORGANIZATION_FROM_INN[] = "OrganizationFromINN"; //Поиск организации по ИНН
const char API_GET_TABLE_QUERY[] = "GetTableQuery"; //Получить данные запроса
const char API_GROUP_RIGHT_SUBSYSTEM_ADD[] = "GroupRightSubSystemAdd"; //Добавить группу право на подсистему
const char API_GROUP_RIGHT_SUBSYSTEM_DELETE[] = "GroupRightSubSystemDelete"; //Удалить у группы право на подсистему
const char API_GROUP_RIGHT_TABLE_ADD[] = "GroupRightTableAdd"; //Добавить группе право на таблицу
const char API_GROUP_RIGHT_TABLE_DELETE[] = "GroupRightTableDelete"; //Удалить у группы право на таблицу
const char API_GROUP_RIGHT_SPECIAL_ADD[] = "GroupRightSpecialAdd"; //Добавить спец. право группе
const char API_GROUP_RIGHT_SPECIAL_DELETE[] = "GroupRightSpecialDelete"; //Удалить спец. право у группы
const char API_SAVE_META_DATA[] = "SaveMetaData"; //Сохранение мета-данных
//PROTOCOL---------------------------------------------------------------------
const char CONST_UID_PROTOCOL_ENTER_APPLICATION[] = "2614cb96-ec54-4d4f-b1a6-06d2444962ca";
const char CONST_UID_PROTOCOL_GET_UPDATE_CLIENT[] = "0234643a-af60-44bf-a2eb-08a71307841c";
const char CONST_UID_PROTOCOL_USER_PASSWORD_CREATE[] = "6dd00b13-2a69-45fd-b565-3142c2d2ba97";
const char CONST_UID_PROTOCOL_USER_PASSWORD_UPDATE[] = "0eee6d67-a0f3-4558-b7ad-173678a8c9dc";
const char CONST_UID_PROTOCOL_USER_PASSWORD_RESET[] = "3b005c51-b44e-48c6-95f8-3ac6bd913bb3";
const char CONST_UID_PROTOCOL_USER_SETTINGS_RESET[] = "075e58a5-5336-4a71-8d1d-91202173f4d4";
const char CONST_UID_PROTOCOL_SEARCH[] = "179f73bf-8983-4b52-b505-af0ef4bf8093";
const char CONST_UID_PROTOCOL_GET_TABLE_DATA[] = "2a93a14e-4ba5-4cb8-a056-0ff2b3351c5d";
const char CONST_UID_PROTOCOL_RECORD_INFO[] = "fd98685e-d6dd-41e3-9f62-389567f58971";
const char CONST_UID_PROTOCOL_SERVER_INFO[] = "e6f5d167-25e6-4172-8d81-219de8608647";
const char CONST_UID_PROTOCOL_CREATE_OBJECT[] = "d1348312-298f-4a7c-b584-9ba8c4952cd3";
const char CONST_UID_PROTOCOL_CREATE_COPY_OBJECT[] = "efa8fe45-1174-4d2e-bbe6-4940380961d4";
const char CONST_UID_PROTOCOL_SHOW_OBJECT[] = "117e8972-97dc-4e72-93ac-dc3bb50d11cf";
const char CONST_UID_PROTOCOL_EDIT_OBJECT[] = "0361643d-0a62-4f51-84bd-313f53115efd";
const char CONST_UID_PROTOCOL_DELETE_OBJECT[] = "b6eefb63-8197-4f3a-8373-5c3b2fc409f3";
const char CONST_UID_PROTOCOL_FILE_STORAGE_ADD[] = "7e9ff116-cf0a-4568-b048-7fab8d773d57";
const char CONST_UID_PROTOCOL_FILE_STORAGE_COPY[] = "5cb370b6-d1aa-4e40-83a9-aeac7fea65e2";
const char CONST_UID_PROTOCOL_FILE_STORAGE_SAVE[] = "e73bedc4-79ff-4b94-acc9-2692e1927d18";
const char CONST_UID_PROTOCOL_NOTE_RECORD_SHOW[] = "0a6320b0-827b-4043-886a-1d7fd5d9b6de";
const char CONST_UID_PROTOCOL_NOTE_RECORD_DELETE[] = "08904df2-3785-4454-b5e5-2d83a9faa231";
const char CONST_UID_PROTOCOL_NOTE_RECORD_ADD[] = "662105b6-6671-46be-8e1a-e57061c5c268";
const char CONST_UID_PROTOCOL_NOTE_RECORD_EDIT[] = "c20634af-ef58-4d96-9215-8085c2b8c416";
const char CONST_UID_PROTOCOL_SEARCH_FULL_TEXT[] = "b0a1ef68-2aa4-4eb2-a5a9-f8e5604f864b";
const char CONST_UID_PROTOCOL_RECORD_FAVORITE_ADD[] = "8d08a9f9-fc50-4986-8d92-fec0d7d36da0";
const char CONST_UID_PROTOCOL_RECORD_FAVORITE_DELETE[] = "af2641f0-6838-441c-a0a9-b62090826998";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SUBSYSTEM[] = "ea765447-6811-43ad-8cfc-9b2e758a5b4e";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SUBSYSTEM[] = "5e5ec969-0d11-41fb-9652-7e2add466782";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_TABLE[] = "44fdda11-9f7b-4006-8178-944da65fb7aa";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_TABLE[] = "9d4e8104-33f9-4f9e-a6aa-272dda7c1a52";
const char CONST_UID_PROTOCOL_ADD_ACCESS_TO_SPECIAL[] = "fb494e8e-bdf3-4804-af42-43487af8a871";
const char CONST_UID_PROTOCOL_DEL_ACCESS_TO_SPECIAL[] = "4e7c34e5-c7fe-4c0d-bb4d-7b4a1d4cf9d2";
const char CONST_UID_PROTOCOL_EXIT_APPLICATION[] = "e9461667-e1ca-4c22-907b-fdc350a931c4";
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_TYPE_SHOW[] = "41743776-d7ce-414f-acf3-71c42a607742"; //Просмотр данных
const char CONST_UID_GROUP_ACCESS_TYPE_CREATE[] = "1cfa961d-08a2-4aa3-978e-ff23fc88ca08"; //Создание данных
const char CONST_UID_GROUP_ACCESS_TYPE_EDIT[] = "259ac6d5-63f8-4c44-b8a6-0108da542ba6"; //Изменение данных
const char CONST_UID_GROUP_ACCESS_TYPE_DELETE[] = "48e0575c-31a0-4ae1-87d7-6450b69a41a5"; //Удаление данных
const char CONST_UID_GROUP_ACCESS_TYPE_EXPORT[] = "e3db5851-69a2-4d32-b4f4-0eb98c331b22"; //Экспорт данных
//-----------------------------------------------------------------------------
const char CONST_UID_DATABASE_SETTING_GENERAL_ACCESSDATABASE[] = "UserAccessDatabase";
const char CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA[] = "NumberSimbolsAfterComma";
const char CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE[] = "StorageFileMaxSize";
const char CONST_UID_DATABASE_SETTING_OTHER_TCPMESSAGEID[] = "TCPMessageID";
//-----------------------------------------------------------------------------
#endif
