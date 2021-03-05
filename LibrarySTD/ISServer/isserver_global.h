#pragma once
#ifndef _ISSERVER_GLOBAL_H_INCLUDED
#define _ISSERVER_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#if defined(ISSERVER_LIBRARY)
# define ISSERVER_EXPORT __declspec(dllexport)
#else
# define ISSERVER_EXPORT __declspec(dllimport)
#endif
//-----------------------------------------------------------------------------
#endif
