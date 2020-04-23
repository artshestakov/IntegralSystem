#pragma once
#ifndef _ISGUI_GLOBAL_H_INCLUDED
#define _ISGUI_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#if defined(ISGUI_LIBRARY)
# define ISGUI_EXPORT Q_DECL_EXPORT
#else
# define ISGUI_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
#endif
