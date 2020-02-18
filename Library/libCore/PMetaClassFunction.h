#pragma once
#ifndef _PMETACLASSFUNCTION_H_INCLUDED
#define _PMETACLASSFUNCTION_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassFunction : public PMetaClass
{
	PMetaClassFunction() : PMetaClass("Function") { }

	QString Text;
};
//-----------------------------------------------------------------------------
#endif
