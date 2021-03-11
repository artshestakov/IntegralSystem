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
//LOGGER-----------------------------------------------------------------------
const size_t	LOGGER_ARRAY_SIZE = 20000; //������������ ������ �������
const size_t	LOGGER_TIMEOUT = 50; //������� �������
const size_t	LOGGER_MESSAGE_SIZE = 4098; //������������ ������ ������ �������
//TCP-----------------------------------------------------------------------------
const size_t	TCP_PACKET_MAX_SIZE = 65535; //������������ ������ ������ TCP-������
const char		TCP_PAKCET_END_CHAR = '\0'; //������ ����� ������
//LOGGER-----------------------------------------------------------------------
#endif
