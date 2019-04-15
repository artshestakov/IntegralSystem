#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISCONTROLS_LIB
# define ISCONTROLS_EXPORT Q_DECL_EXPORT
#else
# define ISCONTROLS_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
