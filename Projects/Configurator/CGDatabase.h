#pragma once
#ifndef _CGDATABASE_H_INCLUDED
#define _CGDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGDatabase
{
public: //Внешние ключи
	static bool Foreign_Create(PMetaForeign *MetaForeign, QString &ErrorString); //Создание внешнего ключа
	static bool Foreign_Update(PMetaForeign *MetaForeign, QString &ErrorString); //Обновление внешнего ключа
	static bool Foreign_Exist(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString); //Проверка существования внешнего ключа
	static QString Foreign_GetName(PMetaForeign *MetaForeign); //Получение имени внешнего ключа

public: //Функции
	static bool Function_CreateOrReplace(PMetaFunction *MetaFunction, QString &ErrorString); //Создание функции

public: //Индексы
	static bool Index_Create(PMetaIndex *Index, QString &ErrorString); //Создание индекса
	static bool Index_Update(PMetaIndex *Index, QString &ErrorString); //Обновление индекса
	static bool Index_Exist(PMetaIndex *Index, bool &Exist, QString &ErrorString); //Проверка существования индекса
	static bool Index_CheckForeign(PMetaIndex *Index); //Проверить ссылается ли на этот индекс внешний ключ
	static bool Index_ReIndex(PMetaIndex *Index, QString &ErrorString); //Перестроить индекс

public: //Ресурсы
	static bool Resource_Insert(PMetaResource *MetaResource, QString &ErrorString);
	static bool Resource_Update(PMetaResource *MetaResource, QString &ErrorString);
	static bool Resource_Exist(PMetaResource *MetaResource, bool &Exist, QString &ErrorString);
	static bool Resource_ResetField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QString &ResourceUID, QString &ErrorString); //Обнулить поле ресурса в таблице
	static bool Resource_UpdateField(const QString &TableName, const QString &TableAlias, const QString &FieldName, const QVariant &Value, const QString &ResourceUID, QString &ErrorString); //Обновить поле ресурса в таблице

public: //Последовательности
	static bool Sequence_Create(const QString &TableName, QString &ErrorString); //Создание последовательности
	static bool Sequence_Exist(const QString &TableName, bool &Exist, QString &ErrorString); //Проверка существования последовательности
	static QString Sequence_GetName(const QString &TableName); //Получение имени последовательности

public: //Таблицы
	static bool Table_Create(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_Update(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_Exist(PMetaTable *MetaTable, bool &Exist, QString &ErrorString);
	static bool Table_AlterFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_CreateFields(PMetaTable *MetaTable, QString &ErrorString);
	static bool Table_DeleteFields(PMetaTable *MetaTable, QString &ErrorString);

public: //Служебные функции
	static bool Helper_ExistField(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //Проверить существование поля в таблице базы
	static bool Helper_CommentTable(PMetaTable *MetaTable, QString &ErrorString); //Изменить описание у таблицы
	static bool Helper_CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString); //Изменить описание у поля
};
//-----------------------------------------------------------------------------
#endif
