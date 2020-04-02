#pragma once
#ifndef _ISCARAT_GLOBAL_H_INCLUDED
#define _ISCARAT_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#if defined(ISCARAT_LIBRARY)
# define ISCARAT_EXPORT Q_DECL_EXPORT
#else
# define ISCARAT_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
#endif
