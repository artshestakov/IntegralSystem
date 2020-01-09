#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class CGHelper : public QObject
{
	Q_OBJECT

public:
	CGHelper();
	virtual ~CGHelper();

	static bool CheckExistColumn(PMetaClassTable *MetaTable, const QString &ColumnName); //Проверить существование поля в таблице базы
	static void CommentTable(const QString &TableName, const QString &Description); //Изменить описание у таблицы
	static void CommentField(const QString &TableName, const QString &FieldName, const QString &Description); //Изменить описание у поля
};
//-----------------------------------------------------------------------------
