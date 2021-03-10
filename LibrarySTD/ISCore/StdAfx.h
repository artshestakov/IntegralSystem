#pragma once
#ifndef _STDAFX_H_INCLUDED
#define _STDAFX_H_INCLUDED
//-----------------------------------------------------------------------------
#ifdef WIN32
#pragma warning(disable: 4251) //For use stl-classes in DLL
#endif
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#else
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#endif
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include <array>
#include <cstdlib>
#include <iosfwd>
//-----------------------------------------------------------------------------
#endif
