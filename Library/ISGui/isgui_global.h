#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISGUI_LIB
# define ISGUI_EXPORT Q_DECL_EXPORT
#else
# define ISGUI_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
