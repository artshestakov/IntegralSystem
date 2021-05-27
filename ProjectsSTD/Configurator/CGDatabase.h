#pragma once
#ifndef _CGDATABASE_H_INCLUDED
#define _CGDATABASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGDatabase
{
public: //Внешние ключи
    //static bool Foreign_Create(PMetaForeign *MetaForeign, QString &ErrorString); //Создание внешнего ключа
    //static bool Foreign_Update(PMetaForeign *MetaForeign, QString &ErrorString); //Обновление внешнего ключа
    //static bool Foreign_Exist(PMetaForeign *MetaForeign, bool &Exist, QString &ErrorString); //Проверка существования внешнего ключа

public: //Функции
    //static bool Function_Create(PMetaFunction *MetaFunction, QString &ErrorString); //Создание функции
    //static bool Function_Delete(QString &ErrorString); //Удаление устаревших функций

public: //Индексы
    //static bool Index_Create(PMetaIndex *Index, QString &ErrorString); //Создание индекса
    //static bool Index_Update(PMetaIndex *Index, QString &ErrorString); //Обновляем индекс
    //static bool Index_Delete(PMetaIndex *Index, QString &ErrorString); //Удалить индекс
    //static bool Index_Exist(PMetaIndex *Index, bool &Exist, QString &ErrorString); //Проверка существования индекса
    //static bool Index_CheckForeign(PMetaIndex *Index); //Проверить ссылается ли на этот индекс внешний ключ
    //static bool Index_ReIndex(PMetaIndex *Index, QString &ErrorString); //Перестроить индекс

public: //Ресурсы
    static bool Resource_Insert(PMetaResource *MetaResource, std::string &ErrorString);
    static bool Resource_Update(PMetaResource *MetaResource, std::string &ErrorString);
    static bool Resource_Exist(PMetaResource *MetaResource, bool &Exist, std::string &ErrorString);
    static bool Resource_ResetField(const std::string &TableName, const std::string &TableAlias, const std::string &FieldName, const std::string &ResourceUID, std::string &ErrorString); //Обнулить поле ресурса в таблице
    static bool Resource_UpdateField(const std::string &TableName, const std::string &TableAlias, const std::string &FieldName, const std::string &Value, const std::string &ResourceUID, std::string &ErrorString); //Обновить поле ресурса в таблице

public: //Последовательности
    //static bool Sequence_Create(const QString &TableName, QString &ErrorString); //Создание последовательности
    //static bool Sequence_Exist(const QString &TableName, bool &Exist, QString &ErrorString); //Проверка существования последовательности
    //static QString Sequence_GetName(const QString &TableName); //Получение имени последовательности

public: //Таблицы
    //static bool Table_Create(PMetaTable *MetaTable, QString &ErrorString);
    //static bool Table_Update(PMetaTable *MetaTable, QString &ErrorString);
    //static bool Table_Exist(PMetaTable *MetaTable, bool &Exist, QString &ErrorString);
    //static bool Table_AlterFields(PMetaTable *MetaTable, QString &ErrorString);
    //static bool Table_CreateFields(PMetaTable *MetaTable, QString &ErrorString);
    //static bool Table_DeleteFields(PMetaTable *MetaTable, QString &ErrorString);

public: //Служебные функции
    //static bool Helper_ExistField(PMetaTable *MetaTable, const QString &ColumnName, bool &Exist, QString &ErrorString); //Проверить существование поля в таблице базы
    //static bool Helper_CommentTable(PMetaTable *MetaTable, QString &ErrorString); //Изменить описание у таблицы
    //static bool Helper_CommentField(PMetaTable *MetaTable, PMetaField *MetaField, QString &ErrorString); //Изменить описание у поля
    //static bool Helper_CommentFunction(PMetaFunction *MetaFunction, QString &ErrorString); //Изменить описание функции
};
//-----------------------------------------------------------------------------
#endif
