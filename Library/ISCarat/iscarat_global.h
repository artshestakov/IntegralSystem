#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISCARAT_LIB
# define ISCARAT_EXPORT Q_DECL_EXPORT
#else
# define ISCARAT_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
