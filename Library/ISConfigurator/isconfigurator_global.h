#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#ifdef ISCONFIGURATOR_LIB
# define ISCONFIGURATOR_EXPORT Q_DECL_EXPORT
#else
# define ISCONFIGURATOR_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
