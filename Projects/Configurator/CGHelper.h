#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGHelper
{
public:
	static bool CheckExistColumn(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //Проверить существование поля в таблице базы
	static bool CommentTable(PMetaTable *MetaTable, QString &ErrorString); //Изменить описание у таблицы
	static bool CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString); //Изменить описание у поля
};
//-----------------------------------------------------------------------------
