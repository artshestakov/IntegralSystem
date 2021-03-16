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
//-----------------------------------------------------------------------------
const size_t	NPOS = std::string::npos;
const char		STRING_NO_ERROR[] = "No error.";
const char		STRING_UNKNOWN_ERROR[] = "Unknown error.";
const char		CHAR_NULL_TERM = '\0'; //������ ����� ������
//LOGGER-----------------------------------------------------------------------
const size_t	LOG_ARRAY_SIZE = 2000; //������������ ������ �������
const size_t	LOG_TIMEOUT = 50; //������� �������
const size_t	LOGGER_MESSAGE_SIZE = 4098; //������������ ������ ������ �������
const size_t	LOG_HEADER_SIZE = 64; //������������ ������ ���������
const size_t	LOG_BUFFER_SIZE = /*10240*/32; //������ ������ ��� ������ ���������
//TCP-----------------------------------------------------------------------------
const size_t	TCP_PACKET_MAX_SIZE = 65535; //������������ ������ ������ TCP-������
//CARAT_API--------------------------------------------------------------------
const char API_UNKNOWN[] = "Unknown"; //����������� ���������
const char API_AUTH[] = "Auth"; //�����������
const char API_SLEEP[] = "Sleep"; //��������
//-----------------------------------------------------------------------------
#endif
