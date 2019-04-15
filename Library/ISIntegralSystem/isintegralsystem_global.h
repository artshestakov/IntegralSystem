#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISINTEGRALSYSTEM_LIB
# define ISINTEGRALSYSTEM_EXPORT Q_DECL_EXPORT
#else
# define ISINTEGRALSYSTEM_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
