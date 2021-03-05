#pragma once
#ifndef _ISCORE_GLOBAL_H_INCLUDED
#define _ISCORE_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#if defined(ISCORE_LIBRARY)
# define ISCORE_EXPORT __declspec(dllexport)
#else
# define ISCORE_EXPORT __declspec(dllimport)
#endif
//-----------------------------------------------------------------------------
#endif
