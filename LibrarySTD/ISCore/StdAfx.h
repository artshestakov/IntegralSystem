#pragma once
#ifndef _STDAFX_H_INCLUDED
#define _STDAFX_H_INCLUDED
//-----------------------------------------------------------------------------
#ifdef WIN32
#pragma warning(disable: 6262)
#pragma warning(disable: 4702) //for ISMetaData::GetType
#endif
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Shlwapi.h>
#else
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <bits/local_lim.h>
#include <uuid/uuid.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
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
#include <unordered_map>
#include <queue>
#include <exception>
#include <algorithm>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <iomanip>
//-----------------------------------------------------------------------------
#endif
