#pragma once
#ifndef _LIBCARAT_GLOBAL_H_INCLUDED
#define _LIBCARAT_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#if defined(LIBCARAT_LIBRARY)
# define LIBCARAT_EXPORT Q_DECL_EXPORT
#else
# define LIBCARAT_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
#endif
