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
const size_t    LOG_HEADER_SIZE = 64; //Макисмальный размер заголовка
const size_t    LOG_BUFFER_SIZE = 10240; //Размер буфера для одного сообщения
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
const char API_USER_PASSWORD_CREATE[] = "UserPasswordCreate"; //Сброс пароля пользователя
const char API_USER_PASSWORD_RESET[] = "UserPasswordReset"; //Сброс пароля пользователя
//PROTOCOL---------------------------------------------------------------------
const char CONST_UID_PROTOCOL_ENTER_APPLICATION[] = "2614cb96-ec54-4d4f-b1a6-06d2444962ca";
const char CONST_UID_PROTOCOL_GET_UPDATE_CLIENT[] = "0234643a-af60-44bf-a2eb-08a71307841c";
const char CONST_UID_PROTOCOL_USER_PASSWORD_CREATE[] = "6dd00b13-2a69-45fd-b565-3142c2d2ba97";
const char CONST_UID_PROTOCOL_USER_PASSWORD_RESET[] = "3b005c51-b44e-48c6-95f8-3ac6bd913bb3";
//-----------------------------------------------------------------------------
#endif
