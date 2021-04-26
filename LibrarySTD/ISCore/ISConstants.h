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
const char      CONST_UID_SETTINGS_DATABASE[] = "1de3449c-c700-4efc-a455-e99e38063d9b"; //������������� �������� ���� ������
const size_t    STRING_FORMAT_BUFFER_SIZE = 4096;
const size_t    UUID_STANDART_SIZE = 36; //������ ������������ UID
const size_t    MD5_SIZE = 16;
const size_t    MD5_RESULT_SIZE = 32;
const char      MD5_DIGITS[] = "0123456789abcdef";
const size_t    DAY_IN_SECONDS = 86400;
//CHARS------------------------------------------------------------------------
const char  CHAR_NULL_TERM = '\0'; //������ ����� ������
//LOCALIZATION-----------------------------------------------------------------
const char  LOCALIZATION_FILE_CARAT[] = "Carat";
//DATABASE---------------------------------------------------------------------
const long long         DB_MAX_QUERY_TIME = 50; //������������ ����� (� ������������) ���������� SQL-�������
const char              DB_CONNECTION_DEFAULT[] = "DefaultDB";
//CONFIG-----------------------------------------------------------------------
const char  CONFIG_TEMPLATE_SERVER[] = "Server"; //������ �������
const char  CONFIG_TEMPLATE_CLIENT[] = "Client"; //������ �������
const char  CONFIG_TYPE_INT[] = "Int";
const char  CONFIG_TYPE_BOOL[] = "Bool";
const char  CONFIG_TYPE_STRING[] = "String";
//LOGGER-----------------------------------------------------------------------
const size_t    LOG_ARRAY_SIZE = 20000; //������������ ������ �������
const size_t    LOG_TIMEOUT = 50; //������� �������
const size_t    LOG_HEADER_SIZE = 96; //������������ ������ ���������
const size_t    LOG_BUFFER_SIZE = 10240; //������ ������ ��� ������ ���������
const char      LOG_UNKNOWN[] = "Unknown";
const char      LOG_DEBUG[] = "Debug";
const char      LOG_INFO[] = "Info";
const char      LOG_WARNING[] = "Warning";
const char      LOG_ERROR[] = "Error";
const char      LOG_CRITICAL[] = "Critical";
const char      LOG_TRACE[] = "Trace";
const char      LOG_ASSERT[] = "Assert";
//ASTERISK---------------------------------------------------------------------
const size_t    TCP_PACKET_MAX_SIZE = 65535; //������������ ������ ������ TCP-������
//CARAT------------------------------------------------------------------------
const unsigned short    CARAT_TCP_PORT = 50000; //����������� ���� TCP-�������
const size_t            CARAT_HASH_SIZE = 64; //������������� ������ ���� SHA256 ��� �����������
const size_t            CARAT_WAIT_WORKERS_MSEC = 1000; //�������� ������������ �������� � ������������
const unsigned long     CARAT_SALT_SIZE = 32; //������������� ������ ����
//CARAT_API--------------------------------------------------------------------
const char API_UNKNOWN[] = "Unknown"; //����������� ���������
const char API_AUTH[] = "Auth"; //�����������
const char API_SLEEP[] = "Sleep"; //��������
const char API_GET_META_DATA[] = "GetMetaData"; //��������� ����-������
const char API_GET_LAST_CLIENT[] = "GetLastClient"; //��������� ����������
const char API_USER_PASSWORD_EXIST[] = "UserPasswordExist"; //�������� ������� ������ ������������
const char API_USER_PASSWORD_CREATE[] = "UserPasswordCreate"; //�������� ������ ������������
const char API_USER_PASSWORD_EDIT[] = "UserPasswordEdit"; //��������� ������ ������������
const char API_USER_PASSWORD_RESET[] = "UserPasswordReset"; //����� ������ ������������
const char API_USER_SETTINGS_RESET[] = "UserSettingsReset"; //����� ������ ������������
const char API_GET_TABLE_DATA[] = "GetTableData"; //��������� ������ �� �������
const char API_RECORD_GET_INFO[] = "RecordGetInfo"; //��������� ���������� � ������
const char API_GET_SERVER_INFO[] = "GetServerInfo"; //��������� ���������� � �������
const char API_GET_RECORD_VALUE[] = "GetRecordValue"; //��������� �������� ������
const char API_RECORD_ADD[] = "RecordAdd"; //���������� ����� ������
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
//-----------------------------------------------------------------------------
const char CONST_UID_GROUP_ACCESS_TYPE_SHOW[] = "41743776-d7ce-414f-acf3-71c42a607742"; //�������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_CREATE[] = "1cfa961d-08a2-4aa3-978e-ff23fc88ca08"; //�������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_EDIT[] = "259ac6d5-63f8-4c44-b8a6-0108da542ba6"; //��������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_DELETE[] = "48e0575c-31a0-4ae1-87d7-6450b69a41a5"; //�������� ������
const char CONST_UID_GROUP_ACCESS_TYPE_EXPORT[] = "e3db5851-69a2-4d32-b4f4-0eb98c331b22"; //������� ������
//-----------------------------------------------------------------------------
const char CONST_UID_DATABASE_SETTING_GENERAL_ACCESSDATABASE[] = "UserAccessDatabase";
const char CONST_UID_DATABASE_SETTING_OTHER_NUMBERSIMBOLSAFTERCOMMA[] = "NumberSimbolsAfterComma";
const char CONST_UID_DATABASE_SETTING_OTHER_STORAGEFILEMAXSIZE[] = "StorageFileMaxSize";
const char CONST_UID_DATABASE_SETTING_OTHER_TCPMESSAGEID[] = "TCPMessageID";
//-----------------------------------------------------------------------------
#endif
