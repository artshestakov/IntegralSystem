#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISNETWORK_LIB
# define ISNETWORK_EXPORT Q_DECL_EXPORT
#else
# define ISNETWORK_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
