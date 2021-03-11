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
const size_t	LOGGER_ARRAY_SIZE = 20000; //Максимальный размер массива
const size_t	LOGGER_TIMEOUT = 50; //Таймаут логгера
const size_t	LOGGER_MESSAGE_SIZE = 4098; //Максимальный размер строки логгера
//TCP-----------------------------------------------------------------------------
const size_t	TCP_PACKET_MAX_SIZE = 65535; //Максимальный размер одного TCP-пакета
const char		TCP_PAKCET_END_CHAR = '\0'; //Символ конца пакета
//LOGGER-----------------------------------------------------------------------
#endif
