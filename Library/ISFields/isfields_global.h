#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISFIELDS_LIB
# define ISFIELDS_EXPORT Q_DECL_EXPORT
#else
# define ISFIELDS_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
