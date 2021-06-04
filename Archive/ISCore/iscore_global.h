#pragma once
#ifndef _ISCORE_GLOBAL_H_INCLUDED
#define _ISCORE_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#if defined(ISCORE_LIBRARY)
#ifdef WIN32
# define ISCORE_EXPORT __declspec(dllexport)
#else
# define ISCORE_EXPORT __attribute__((visibility("default")))
#endif
#else
#ifdef WIN32
# define ISCORE_EXPORT __declspec(dllimport)
#else
# define ISCORE_EXPORT
#endif
#endif
//-----------------------------------------------------------------------------
#endif
