#pragma once
#ifndef _PMETACLASSQUERY_H_INCLUDED
#define _PMETACLASSQUERY_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClassTable.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
struct PMetaClassQuery : public PMetaClassTable
{
public:
	PMetaClassQuery() : PMetaClassTable("Query") { }

	QString From;
	QString Where;
	QString Order;
	QString OrderType;
	QVectorString Joins;
};
//-----------------------------------------------------------------------------
#endif
