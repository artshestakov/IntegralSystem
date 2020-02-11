#pragma once
#ifndef _PMETACLASSINDEX_H_INCLUDED
#define _PMETACLASSINDEX_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassIndex : public PMetaClass
{
public:
	PMetaClassIndex();
	PMetaClassIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name);
	~PMetaClassIndex();

	void SetUnique(bool unique);
	bool GetUnique() const;

	void SetAlias(const QString &alias);
	QString GetAlias() const;

	void SetTableName(const QString &table_name);
	QString GetTableName() const;

	void SetFieldName(const QString &field_name);
	QString GetFieldName() const;

	void AddField(const QString &Field);
	QVectorString GetFields();

private:
	bool Unique;
	QString Alias;
	QString TableName;
	QString FieldName;
	QVectorString Fields;
};
//-----------------------------------------------------------------------------
#endif
