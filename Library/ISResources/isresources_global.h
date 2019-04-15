#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISRESOURCES_LIB
# define ISRESOURCES_EXPORT Q_DECL_EXPORT
#else
# define ISRESOURCES_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
