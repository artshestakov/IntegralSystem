#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISOBJECTS_LIB
# define ISOBJECTS_EXPORT Q_DECL_EXPORT
#else
# define ISOBJECTS_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
