#pragma once
#ifndef _PMETACLASSESCORT_H_INCLUDED
#define _PMETACLASSESCORT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassEscort : public PMetaClass
{
public:
	PMetaClassEscort();
	~PMetaClassEscort();

	void SetLocalName(const QString &local_name);
	QString GetLocalName() const;

	void SetTableName(const QString &table_name);
	QString GetTableName() const;

	void SetClassName(const QString &class_name);
	QString GetClassName() const;

	void SetClassFilter(const QString &class_filter);
	QString GetClassFilter() const;

private:
	QString LocalName;
	QString TableName;
	QString ClassName;
	QString ClassFilter;
};
//-----------------------------------------------------------------------------
#endif
