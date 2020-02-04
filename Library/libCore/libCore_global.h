#pragma once
#ifndef _LIBCORE_GLOBAL_H_INCLUDED
#define _LIBCORE_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#if defined(LIBCORE_LIBRARY)
# define LIBCORE_EXPORT Q_DECL_EXPORT
#else
# define LIBCORE_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
#endif
