#pragma once
#ifndef _STDAFX_H_INCLUDED
#define _STDAFX_H_INCLUDED
//-----------------------------------------------------------------------------
#pragma warning(disable: 4251) //For use stl-classes in DLL
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#endif
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include <array>
//-----------------------------------------------------------------------------
#endif
