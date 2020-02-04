#pragma once
#ifndef _LIBGUI_GLOBAL_H_INCLUDED
#define _LIBGUI_GLOBAL_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
#if defined(LIBGUI_LIBRARY)
# define LIBGUI_EXPORT Q_DECL_EXPORT
#else
# define LIBGUI_EXPORT Q_DECL_IMPORT
#endif
//-----------------------------------------------------------------------------
#endif
