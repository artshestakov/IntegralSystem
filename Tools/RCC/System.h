#pragma once
#ifndef _SYSTEM_H_INCLUDED
#define _SYSTEM_H_INCLUDED
//-----------------------------------------------------------------------------
#include <string>
//-----------------------------------------------------------------------------
#ifdef WIN32
const char PATH_SEPARATOR = '\\';
#define CHRONO_FORMAT "%lld"
#else
const char PATH_SEPARATOR = '/';
#define CHRONO_FORMAT "%ld"
#endif
//-----------------------------------------------------------------------------
#endif
