#pragma once
#ifndef _ISCORE_GLOBAL_H_INCLUDED
#define _ISCORE_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#if defined(ISCORE_LIBRARY)
# define ISCORE_EXPORT Q_DECL_EXPORT
#else
# define ISCORE_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
#endif
