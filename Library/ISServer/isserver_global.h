#pragma once
#ifndef _ISSERVER_GLOBAL_H_INCLUDED
#define _ISSERVER_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#if defined(ISSERVER_LIBRARY)
#ifdef WIN32
# define ISSERVER_EXPORT __declspec(dllexport)
#else
# define ISSERVER_EXPORT __attribute__((visibility("default")))
#endif
#else
#ifdef WIN32
# define ISSERVER_EXPORT __declspec(dllimport)
#else
# define ISSERVER_EXPORT
#endif
#endif
//-----------------------------------------------------------------------------
#endif
