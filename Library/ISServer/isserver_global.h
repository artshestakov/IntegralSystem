#pragma once
#ifndef _ISSERVER_GLOBAL_H_INCLUDED
#define _ISSERVER_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#if defined(ISSERVER_LIBRARY)
# define ISSERVER_EXPORT Q_DECL_EXPORT
#else
# define ISSERVER_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
#endif
