#pragma once
#ifndef _CGDATABASE_H_INCLUDED
#define _CGDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGDatabase
{
public: //foreigns
	static bool CreateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //Создание внешнего ключа
	static bool UpdateForeign(PMetaForeign *MetaForeign, QString &ErrorString); //Обновление внешнего ключа
	static bool CheckExistForeign(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString); //Проверка существования внешнего ключа
	static QString GetForeignName(PMetaForeign *MetaForeign); //Получение имени внешнего ключа

public: //functions
	static bool CreateOrReplaceFunction(PMetaFunction *MetaFunction, QString &ErrorString); //Создание функции

public: //helper
	static bool CheckExistColumn(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //Проверить существование поля в таблице базы
	static bool CommentTable(PMetaTable *MetaTable, QString &ErrorString); //Изменить описание у таблицы
	static bool CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString); //Изменить описание у поля

public: //indexes
	static bool CreateIndex(PMetaIndex *Index, QString &ErrorString); //Создание индекса
	static bool UpdateIndex(PMetaIndex *Index, QString &ErrorString); //Обновление индекса
	static bool CheckExistIndex(PMetaIndex *Index, bool &Exist, QString &ErrorString); //Проверка существования индекса
	static bool CheckIndexForeign(PMetaIndex *Index); //Проверить ссылается ли на этого индекс внешний ключ
	static bool ReindexIndex(PMetaIndex *Index, QString &ErrorString); //Перестроить индекс

public: //resources
	static bool InsertResource(PMetaResource *MetaResource, QString &ErrorString);
	static bool UpdateResource(PMetaResource *MetaResource, QString &ErrorString);
	static bool CheckExistResource(PMetaResource *MetaResource, bool &Exist, QString &ErrorString);
	static bool ResetResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString); //Обнулить поле ресурса в таблице
	static bool UpdateResourceField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString); //Обновить поле ресурса в таблице

public: //sequence
	static bool CreateSequence(const QString &TableName, QString &ErrorString); //Создание последовательности
	static bool CheckExistSequence(const QString &TableName, bool &Exist, QString &ErrorString); //Проверка существования последовательности
	static QString GetSequenceNameForTable(const QString &TableName); //Получение имени последовательности

public: //tables
	static bool CreateTable(PMetaTable *MetaTable, QString &ErrorString);
	static bool UpdateTable(PMetaTable *MetaTable, QString &ErrorString);
	static bool CheckExistTable(PMetaTable *MetaTable, bool &Exist, QString &ErrorString);
	static bool AlterExistFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool CreateNewFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool DeleteOldFields(PMetaTable *MetaTable, QString &ErrorString);
};
//-----------------------------------------------------------------------------
#endif
