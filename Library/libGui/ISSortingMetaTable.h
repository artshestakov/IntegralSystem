#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISSortingMetaTable : public QObject
{
	Q_OBJECT

public:
	ISSortingMetaTable();
	virtual ~ISSortingMetaTable();

	void SetTableName(const QString &table_name);
	QString GetTableName() const;

	void SetFieldName(const QString &field_name);
	QString GetFieldName() const;

	void SetSortingType(Qt::SortOrder sorting);
	Qt::SortOrder GetSortingType() const;

	void SetModificationFlag(bool modification_flag);
	bool GetModificationFlag() const;

private:
	QString TableName;
	QString FieldName;
	Qt::SortOrder Sorting;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
