#pragma once
#ifndef _PMETACLASSESCORT_H_INCLUDED
#define _PMETACLASSESCORT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassEscort : public PMetaClass
{
	PMetaClassEscort() : PMetaClass("Escort") { }

	QString LocalName;
	QString TableName;
	QString ClassName;
	QString ClassFilter;
};
//-----------------------------------------------------------------------------
#endif
