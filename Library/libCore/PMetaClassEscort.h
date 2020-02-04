#pragma once
#ifndef _PMETACLASSESCORT_H_INCLUDED
#define _PMETACLASSESCORT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassEscort : public PMetaClass
{
	Q_OBJECT

	Q_PROPERTY(QString LocalName READ GetLocalName WRITE SetLocalName)
	Q_PROPERTY(QString TableName READ GetTableName WRITE SetTableName)
	Q_PROPERTY(QString ClassName READ GetClassName WRITE SetClassName)
	Q_PROPERTY(QString ClassFilter READ GetClassFilter WRITE SetClassFilter)

public:
	PMetaClassEscort(QObject *parent = 0);
	virtual ~PMetaClassEscort();

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
