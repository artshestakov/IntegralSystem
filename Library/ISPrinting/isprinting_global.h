#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISPRINTING_LIB
# define ISPRINTING_EXPORT Q_DECL_EXPORT
#else
# define ISPRINTING_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
