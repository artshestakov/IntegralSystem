#pragma once
#ifndef _PMETACLASS_H_INCLUDED
#define _PMETACLASS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
struct PMetaBase
{
    PMetaBase(const std::string &type_object)
        : TypeObject(type_object.empty() ? "Unknown" : type_object) { }

    std::string TypeObject; //Тип объекта
};
//-----------------------------------------------------------------------------
struct PMetaIndex : public PMetaBase
{
    PMetaIndex(bool unique, const std::string &alias, const std::string &table_name, const std::string &field_name)
        : PMetaBase("Index"),
        Unique(unique), Alias(alias), TableName(table_name), FieldName(field_name) { }

    std::string GetName() //Получить имя индекса
    {
        std::string IndexName;
        if (!Fields.empty()) //Если индекс составной
        {
            IndexName += TableName + '_';
            for (const std::string &String : Fields)
            {
                IndexName += String + '_';
            }
            ISAlgorithm::StringChop(IndexName, 1);
        }
        else
        {
            IndexName = TableName + '_' + FieldName;
        }
        return ISAlgorithm::StringToLowerGet(IndexName);
    }

    bool Unique;
    std::string Alias;
    std::string TableName;
    std::string FieldName;
    ISVectorString Fields;
};
//-----------------------------------------------------------------------------
struct PMetaForeign : public PMetaBase
{
    PMetaForeign(const std::string &field, const std::string &foreign_class, const std::string &foreign_field, const std::string &foreign_view_name_field, const std::string &table_name = std::string())
        : PMetaBase("Foreign"),
        Field(field), ForeignClass(foreign_class), ForeignField(foreign_field), ForeignViewNameField(foreign_view_name_field), TableName(table_name) { }

    std::string GetName() const //Получить имя внешнего ключа
    {
        return ISAlgorithm::StringToLowerGet(TableName + '_' + Field);
    }

    std::string Field; //Поле, на которое устанавливается внешний ключ
    std::string ForeignClass; //На какую таблицу ссылкается внешний ключ
    std::string ForeignField; //На какое поле ссылается внешний ключ
    std::string ForeignViewNameField; //Какое поле (поля) отображать в запросе на выборку

    std::string TableName; //Таблица, содержащая поле, на которое устанавливается внешний ключ
};
//-----------------------------------------------------------------------------
struct PMetaField : public PMetaBase
{
    PMetaField() : PMetaBase("Field"),
        Type(ISNamespace::FieldType::Unknown),
        Size(0),
        Upper(false),
        Lower(false),
        NotNull(false),
        ReadOnly(false),
        HideFromObject(false),
        HideFromList(false),
        NotSearch(false),
        IsSystem(false),
        Index(nullptr),
        Foreign(nullptr),
        Sequence(false),
        PrimaryKey(false)
    {

    }

    bool IsFieldID() const
    {
        return Type == ISNamespace::FieldType::ID;
    }

    std::string UID;
    ISNamespace::FieldType Type; //Тип
    std::string Name; //Название
    int Size; //Размер
    bool Upper; //Только верхний регистр
    bool Lower; //Только нижний регистр
    std::string DefaultValue; //Значение по умолчанию для базы
    std::string DefaultValueWidget; //Значение по умолчанию для виджета
    std::string LabelName; //Имя поля на форме объекта
    std::string LocalListName; //Имя поля в списке
    bool NotNull; //Поле не должно быть пустым
    bool ReadOnly; //Редактирование поля запрещено
    bool HideFromObject; //Поле должно быть скрыто в форме объекта
    bool HideFromList; //Поле не должно учавствовать в запросе
    bool NotSearch; //Участие поля в поиске
    std::string Hint; //Подсказка для поля
    std::string PlaceholderText; //Placeholder текст для виджета
    std::string ControlWidget; //Наименование виджета-редактора значения
    std::string RegExp; //Регулярное выражение для поля
    bool IsSystem; //Статус системного поля

    std::string QueryText; //Текст подзапроса

    PMetaIndex *Index; //Индекс
    PMetaForeign *Foreign; //Внешний ключ

    bool Sequence; //Последовательность поля
    bool PrimaryKey; //Первичный ключ
    std::string LayoutName; //Наименование компоновщика (для горизонтального размещения поля)
    std::string SeparatorName; //Наименование вкладки
};
//-----------------------------------------------------------------------------
struct PMetaEscort : public PMetaBase
{
    PMetaEscort() : PMetaBase("Escort") { }

    std::string LocalName;
    std::string TableName;
    std::string FilterField;
    std::string ClassName;
};
//-----------------------------------------------------------------------------
struct PMetaTable : public PMetaBase
{
    PMetaTable()
        : PMetaBase("Table"),
        IsSystem(false),
        ShowOnly(false)
    {

    }

    bool Exist(const std::string &FieldName)
    {
        for (PMetaField *MetaField : Fields)
        {
            if (MetaField->Name == FieldName)
            {
                return true;
            }
        }
        return false;
    }

    PMetaField* GetField(const std::string &FieldName)
    {
        for (PMetaField *MetaField : Fields)
        {
            if (ISAlgorithm::StringToLowerGet(MetaField->Name) == ISAlgorithm::StringToLowerGet(FieldName))
            {
                return MetaField;
            }
        }
        return nullptr;
    }

    std::string Name;
    std::string UID;
    std::string Alias;
    std::string LocalName;
    std::string LocalListName;
    std::string TitleName;
    bool IsSystem;
    bool ShowOnly;

    std::vector<PMetaField*> Fields;
    std::vector<PMetaIndex*> IndexesCompound; //Составные индексы таблицы
    std::vector<PMetaEscort*> Escorts; //Эскортные таблицы
};
//-----------------------------------------------------------------------------
struct PMetaFunction : public PMetaBase
{
    PMetaFunction(const std::string &name, const std::string &comment, const std::string &text)
        : PMetaBase("Function"),
        Name(name), Comment(comment), Text(text) { }

    std::string Name; //Имя функции
    std::string Comment; //Комментарий функции
    std::string Text; //Текст функции
};
//-----------------------------------------------------------------------------
struct PMetaResource : public PMetaBase
{
    PMetaResource() : PMetaBase("Resource") { }

    void AddField(const std::string &FieldName, const std::string &Value) //Добавить параметр и его значение в ресурс
    {
        std::string Temp = FieldName;
        ISAlgorithm::StringToLower(Temp);
        if (Temp != "uid")
        {
            Parameters.emplace(FieldName, Value);
        }
    }

    std::string TableName;
    std::string UID;
    ISMapString Parameters;
};
//-----------------------------------------------------------------------------
#endif
