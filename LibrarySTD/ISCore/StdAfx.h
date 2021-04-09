#pragma once
#ifndef _STDAFX_H_INCLUDED
#define _STDAFX_H_INCLUDED
//-----------------------------------------------------------------------------
#ifdef WIN32
#pragma warning(disable: 6262)
#endif
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <Shlwapi.h>
#else
#include <sys/time.h>
#include <sys/stat.h>
#include <uuid/uuid.h>
#include <unistd.h>
#include <string.h>
#endif
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <array>
#include <map>
#include <queue>
#include <exception>
#include <algorithm>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
//-----------------------------------------------------------------------------
#endif
