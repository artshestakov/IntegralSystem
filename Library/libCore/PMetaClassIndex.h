#pragma once
#ifndef _PMETACLASSINDEX_H_INCLUDED
#define _PMETACLASSINDEX_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT PMetaClassIndex : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool Unique READ GetUnique WRITE SetUnique)
	Q_PROPERTY(QString Alias READ GetAlias WRITE SetAlias)
	Q_PROPERTY(QString TableName READ GetTableName WRITE SetTableName)
	Q_PROPERTY(QString FieldName READ GetFieldName WRITE SetFieldName)

public:
	PMetaClassIndex(QObject *parent = 0);
	PMetaClassIndex(bool unique, const QString &alias, const QString &table_name, const QString &field_name, QObject *parent = 0);
	virtual ~PMetaClassIndex();

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
