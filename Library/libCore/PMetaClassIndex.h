#pragma once
#ifndef _PMETACLASSINDEX_H_INCLUDED
#define _PMETACLASSINDEX_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassIndex : public PMetaClass
{
	PMetaClassIndex() : PMetaClass("Index") { }
	PMetaClassIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name) : PMetaClass("Index"), Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

	bool Unique;
	QString Alias;
	QString TableName;
	QString FieldName;
	QVectorString Fields;
};
//-----------------------------------------------------------------------------
#endif
