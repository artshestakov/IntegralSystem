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
const size_t    LOG_HEADER_SIZE = 64; //������������ ������ ���������
const size_t    LOG_BUFFER_SIZE = 10240; //������ ������ ��� ������ ���������
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
const char API_USER_PASSWORD_CREATE[] = "UserPasswordCreate"; //����� ������ ������������
const char API_USER_PASSWORD_RESET[] = "UserPasswordReset"; //����� ������ ������������
//PROTOCOL---------------------------------------------------------------------
const char CONST_UID_PROTOCOL_ENTER_APPLICATION[] = "2614cb96-ec54-4d4f-b1a6-06d2444962ca";
const char CONST_UID_PROTOCOL_GET_UPDATE_CLIENT[] = "0234643a-af60-44bf-a2eb-08a71307841c";
const char CONST_UID_PROTOCOL_USER_PASSWORD_CREATE[] = "6dd00b13-2a69-45fd-b565-3142c2d2ba97";
const char CONST_UID_PROTOCOL_USER_PASSWORD_RESET[] = "3b005c51-b44e-48c6-95f8-3ac6bd913bb3";
//-----------------------------------------------------------------------------
#endif
