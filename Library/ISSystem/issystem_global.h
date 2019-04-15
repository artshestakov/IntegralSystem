#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISSYSTEM_LIB
# define ISSYSTEM_EXPORT Q_DECL_EXPORT
#else
# define ISSYSTEM_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
